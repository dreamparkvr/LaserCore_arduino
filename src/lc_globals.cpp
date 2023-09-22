#include "lc_globals.h"
#include <cstring>  // for strncpy()
// Define and initialize global variables

WifiInfo wifiInfo = { DISCONNECTED, "" };
ServerInfo serverInfo = { DISCONNECTED };

bool gameInProgress = false;

String playerName = "";
int playerTeam = 0;
int playerAmmo = 0;
int playerMags = 0;
int playerHealth = 100;

void initializeLcGlobalsMutex() {
    lcGlobalsMutex = xSemaphoreCreateMutex();
    
    if (lcGlobalsMutex == NULL) {
        // Handle the error - Mutex creation failed.
    }
}


void SetWifiStatus(ConnectionState status, const char* ip) {
    wifiInfo.status = status;
    strncpy(wifiInfo.ip, ip, sizeof(wifiInfo.ip) - 1);  // Ensure null-termination
    wifiInfo.ip[sizeof(wifiInfo.ip) - 1] = '\0';  // Null-terminate just in case
}

ConnectionState getWifiState() {
    return wifiInfo.status;
}

const char* getIpAddress() {
    return wifiInfo.ip;
}

void setServerStatus(ConnectionState status) {
    serverInfo.status = status;
}

ConnectionState getServerStatus() {
    return serverInfo.status;
}



SemaphoreHandle_t lcGlobalsMutex;



