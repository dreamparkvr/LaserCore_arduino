#ifndef LC_WIFI_H
#define LC_WIFI_H

#include <Arduino.h>
#include <Wifi.h>
#include <lc_display.h>
void wifiKeepAlive(void * parameters);
bool isWifiConnected();
#endif