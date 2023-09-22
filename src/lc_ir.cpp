#include <arduino.h>
#include <lc_ir.h>
#include <driver/rmt.h>

inline float compute_duration(int duration_ticks) {
    float masked = static_cast<float>(duration_ticks & 0x7FFF);
    float result = (masked * 10.0) / static_cast<float>(RMT_TICK_10_US);
    return result;
}

inline bool check_in_range(int duration_ticks, int target_us, int margin_us) {
    float float_duration = compute_duration(duration_ticks);
    int rounded_duration = static_cast<int>(round(float_duration));

    if((rounded_duration < (target_us + margin_us))
        && (rounded_duration > (target_us - margin_us))) {
        return true;
    }
    return false;
}



//lasercore send methods
void lasercore_fill_item_level(rmt_item32_t* item, int high_us, int low_us){
    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;

    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;

}

void lasercore_fill_item_header(rmt_item32_t* item) {
    lasercore_fill_item_level(item, LASERCORE_START_PULSE, LASERCORE_GAP);
}

void lasercore_fill_item_bit_one(rmt_item32_t* item) {
    lasercore_fill_item_level(item, LASERCORE_ONE_PULSE, LASERCORE_GAP);
}

void lasercore_fill_item_bit_zero(rmt_item32_t* item) {
    lasercore_fill_item_level(item, LASERCORE_ZERO_PULSE, LASERCORE_GAP);
}

int lasercore_build_items(int chamnel, rmt_item32_t* item, int item_num, uint16_t data) {
    int i = 0;
    if(item_num < 17) { // 1 for header + 16 for data
        return -1;
    }
    lasercore_fill_item_header(item++);
    i++;
    for(int j = 15; j >= 0; j--) {
        if(data &(1 << j)) {
            lasercore_fill_item_bit_one(item);
        } else {
            lasercore_fill_item_bit_zero(item);
        }
        item++;
        i++;
    }
    return i;
}
 
void print_received_timings(rmt_item32_t* items, int item_count) {
    Serial.println("Received items:");
    for(int i = 0; i < item_count; i++) {
        Serial.print("Item ");
        Serial.print(i);
        Serial.print(": High Level: ");
        Serial.print(items[i].level0);
        Serial.print(", High Duration: ");
        Serial.print(compute_duration(items[i].duration0));
        Serial.print(", Low Level: ");
        Serial.print(items[i].level1);
        Serial.print(", Low Duration: ");
        Serial.println(compute_duration(items[i].duration1));
    }
}

//lasercore receiver methods

bool lasercore_header_if(rmt_item32_t* item) {
    bool valid = (item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) &&
                 check_in_range(item->duration0, LASERCORE_START_PULSE, LASERCORE_BIT_MARGIN) &&
                 check_in_range(item->duration1, LASERCORE_GAP, LASERCORE_BIT_MARGIN);
    if (!valid) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return valid;
}

bool lasercore_bit_one_if(rmt_item32_t* item) {
    bool valid = (item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) &&
                 check_in_range(item->duration0, LASERCORE_ONE_PULSE, LASERCORE_BIT_MARGIN) &&
                 (item->duration1 == 0 || check_in_range(item->duration1, LASERCORE_GAP, LASERCORE_BIT_MARGIN));

    if (!valid) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return valid;
}

bool lasercore_bit_zero_if(rmt_item32_t* item) {
    bool valid = (item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) &&
                 check_in_range(item->duration0, LASERCORE_ZERO_PULSE, LASERCORE_BIT_MARGIN) &&
                 (item->duration1 == 0 || check_in_range(item->duration1, LASERCORE_GAP, LASERCORE_BIT_MARGIN));

    if (!valid) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return valid;
}


int lasercore_parse_items(rmt_item32_t* items, int item_num, uint32_t* data) {
  
   // print_received_timings(items, item_num);
  
    // Initialization and Input Validation
    Serial.print("itemcount=");
    Serial.println(item_num);
    if (item_num < 17) {  // Minimum size for 16-bit data + header
        return -1;
    }

    // Header Check
    if (!lasercore_header_if(&items[0])) {  // Use indexing here
        Serial.println("Invalid header.");
        return -1;
    }
    Serial.println("Header detected.");

    // Determine Message Size
    uint32_t data_t = 0;
    int bits_to_process;
    int index = 1;  // Start from the second item

   // Serial.print("Duration of first bit (high): ");
   // Serial.println(compute_duration(items[index].duration0));
   // Serial.print("Duration of first bit (low): ");
   // Serial.println(compute_duration(items[index].duration1));

    if (lasercore_bit_zero_if(&items[index])) {
        bits_to_process = 16;
        Serial.println("Detected 16-bit data.");
    } else if (lasercore_bit_one_if(&items[index])) {
        if (item_num < 33) {  // Check if there's enough items for a 32-bit message
            Serial.println("Detected 32-bit data but not enough items for 32-bit data.");
            return -1;
        }
        bits_to_process = 32;
        Serial.println("Detected 32-bit data.");
    } else {
        Serial.println("Unexpected bit pattern after header.");
        return -1;  // Invalid bit after header
    }
    index++;  // Move to the first data bit

    // Data Bit Processing
    for (int j = bits_to_process - 1; j >= 0; j--, index++) {
        if (lasercore_bit_one_if(&items[index])) {
            data_t |= (1UL << j);
        } else if (lasercore_bit_zero_if(&items[index])) {
            // Do nothing as zero is the default value
        } else {
            Serial.print("Unexpected bit pattern at bit position ");
            Serial.println(j);
            Serial.print("High duration: "); Serial.println(items[index].duration0);
            Serial.print("Low duration: "); Serial.println(items[index].duration1);
            Serial.print("High level: "); Serial.println(items[index].level0);
            Serial.print("Low level: "); Serial.println(items[index].level1);
            return -1;  // Unexpected bit pattern
        }
    }

    *data = data_t;
    return bits_to_process;  // Return the number of bits processed
}


/*
 * @brief RMT transmitter initialization
 */
void nec_tx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;

    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}

/*
 * @brief RMT receiver initialization
 */
void nec_rx_init()
{
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
}

void rmt_lasercore_rx_task(void * parameter) {
    rmt_channel_t channel = RMT_RX_CHANNEL;
    nec_rx_init();  // You'll need a separate initialization for lasercore if any parameters are different
    RingbufHandle_t rb = NULL;
    
    rmt_get_ringbuf_handle(channel, &rb);
    if (!rb) {
        Serial.println("Failed to get ring buffer handle!");
        vTaskDelete(NULL);
        return;
    }

    rmt_rx_start(channel, 1);

    while(1) {
        size_t rx_size = 0;

        rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
        if(item) {
            Serial.println("Data received from ring buffer!");
            uint32_t rmt_data;  // Handling up to 32-bit data for lasercore
            int offset = 0;
            while(1) {
                int res = lasercore_parse_items(item + offset, (rx_size / sizeof(rmt_item32_t)) - offset, &rmt_data);
                if(res > 0) {
                    offset += (res / 8);  // Increment by number of rmt_item32_t processed. Each bit is one rmt_item32_t.
                    char log_msg[100]; // Create a buffer for the log message
                    sprintf(log_msg, "RMT RCV --- data: 0x%08x", rmt_data);
                    Serial.println(log_msg);
                } else {
                    break;
                }
            }
            vRingbufferReturnItem(rb, (void*) item);
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Add a small delay
    }
}