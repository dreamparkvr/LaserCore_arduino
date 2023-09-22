#include <driver/rmt.h>
#ifndef LC_IR_H
#define LC_IR_H

//RMT Specific
#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */



#define RMT_TX_CHANNEL    RMT_CHANNEL_0     
#define RMT_TX_GPIO_NUM   GPIO_NUM_27    /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    RMT_CHANNEL_1/*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM   GPIO_NUM_26
#define RMT_CLK_DIV       100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    80000000/RMT_CLK_DIV/100000  /*!< RMT counter value for 10 us.(Source clock is APB clock) */


#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

//MilesTag Specific
#define LASERCORE_START_PULSE 2400 //uS
#define LASERCORE_ONE_PULSE 1200 //uS
#define LASERCORE_ZERO_PULSE 600 //uS
#define LASERCORE_GAP 600 //uS
#define LASERCORE_BIT_MARGIN 300 //uS

extern SemaphoreHandle_t rmt_semaphore;
 
void nec_tx_init();
void nec_rx_init();
void initializeRmtMutex();
void rmt_rx_task(void * parameter);
void rmt_lasercore_rx_task(void * parameter);
int lasercore_build_items(int chamnel, rmt_item32_t* item, int item_num, uint16_t data);
int lasercore_parse_items(rmt_item32_t* item, int item_num, uint32_t* data);
#endif