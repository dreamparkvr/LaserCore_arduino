#include <lc_triggerLogic.h>
#include <lc_globals.h>
#include <Arduino.h>
#include <lc_tcpsock.h>
#include <lc_display.h>
#include <lc_ir.h>
#include <driver/rmt.h>
#define TRIGGER_PIN 12


volatile bool triggerPressed = false;
int channel = RMT_TX_CHANNEL; // RMT channel for transmission
uint16_t cmd = 0x0;  // The command to be sent
uint16_t addr = 0x11; // The address to be sent


void handleTriggerPull() {
    triggerPressed = true;  // This is an ISR; keep it short and don't use mutexes here.
}

// Convert a 16-bit integer to binary string for debugging
String toBinaryString(uint16_t val) {
    String binaryString = "";
    for (int i = 15; i >= 0; i--) {
        binaryString += (val & (1 << i)) ? '1' : '0';
    }
    return binaryString;
}



void checkAndHandleTrigger() {
    if (xSemaphoreTake(lcGlobalsMutex, portMAX_DELAY) == pdTRUE) { 
        // Now it's safe to access the global variables
        if (gameInProgress) {
            if (triggerPressed) {
                if (playerAmmo > 0) {

                    size_t size = (sizeof(rmt_item32_t) * (1 + 16)); // 1 for header + 16 for data bits
                    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
                    memset((void*) item, 0, size);

                    // Prepare a 16-bit Lasercore message with all 1s except for the highest bit (assuming the provided value)
                    uint16_t lasercoreData = 0x7FFF;

                    // Debug: Print the binary representation of lasercoreData before sending it
                    Serial.print("Sending Lasercore Data: ");
                    Serial.println(toBinaryString(lasercoreData));

                    int item_count = lasercore_build_items(channel, item, 17, lasercoreData);

                   if (item_count > 0) { 
            // Debug: Print the items before sending them
            for (int i = 0; i < item_count; i++) {
                Serial.print("Item ");
                Serial.print(i);
                Serial.print(": High Level: ");
                Serial.print(item[i].level0);
                Serial.print(", High Duration: ");
                Serial.print(item[i].duration0); // assuming you have this function
                Serial.print(", Low Level: ");
                Serial.print(item[i].level1);
                Serial.print(", Low Duration: ");
                Serial.println(item[i].duration1); // assuming you have this function
            }

            Serial.println("should be shooting...");
            rmt_write_items(RMT_TX_CHANNEL, item, item_count, true);
            rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
        }
        free(item);

                    sendMessageToServer(1, "bang");
                    playerAmmo--;
                    SetAmmoLevel(playerAmmo, playerMags); 
                } else {
                    sendMessageToServer(1, "click");
                }
                triggerPressed = false; // Reset the flag
            }
        }

        xSemaphoreGive(lcGlobalsMutex);
    }
}



void initTrigger() {
    pinMode(TRIGGER_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), handleTriggerPull, FALLING);
    nec_tx_init();  // Initialize NEC transmission settings


}

bool wasTriggerPressed() {
    return triggerPressed;
}

void resetTriggerPress() {
    triggerPressed = false; 
}
