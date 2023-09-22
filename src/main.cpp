#include <Arduino.h>
#include <Wifi.h>
#include <TFT_eSPI.h>
#include <lc_display.h>
#include <lc_wifi.h>
#include <lc_tcpsock.h>
#include <lc_triggerLogic.h>
#include <lc_gameManager.h>
#include <lc_globals.h>
#include <lc_ir.h>

#define WIFI_NETWORK "Fios-rHCb9"
#define WIFI_PASS "dozen54wit68mesh"
#define WIFI_TIMEOUT_MS 20000

#define SVR_IP "192.168.1.19"
#define SVR_PORT 12345



void setup() {
   Serial.begin(9600);
  initializeLcGlobalsMutex();
  initTrigger();
  initializeDisplay();


 

  xTaskCreatePinnedToCore(
    wifiKeepAlive,
    "Wifi KeepAlive",
    5000,
    NULL,
    2,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE

  );

    xTaskCreatePinnedToCore(
    clientTask,
    "client task",
    5000,
    NULL,
    2,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE

  );

  xTaskCreatePinnedToCore(
        rmt_lasercore_rx_task,          /* Task function */
        "RMT_RX_TASK",        /* Task name */
        2500,                 /* Stack size */
        NULL,                 /* Parameter */
        1,                    /* Task priority */
        NULL,                  /* Task handle */
        CONFIG_ARDUINO_RUNNING_CORE
    );
 
}

void loop() {

      checkAndHandleTrigger();

}

