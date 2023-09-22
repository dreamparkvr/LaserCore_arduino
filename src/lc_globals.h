#ifndef LC_GLOBALS_H
#define LC_GLOBALS_H

#include <Arduino.h>
#include "freertos/semphr.h"

extern SemaphoreHandle_t lcGlobalsMutex;

enum ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED, 
    FAULT
};

struct WifiInfo {
    ConnectionState status;
    char ip[16];  // Assuming IPv4 address
};

struct ServerInfo {
    ConnectionState status;
};

extern WifiInfo wifiInfo;
extern ServerInfo serverInfo;

// Function declarations for managing WiFi info
void SetWifiStatus(ConnectionState status, const char* ip);
ConnectionState getWifiState();
const char* getIpAddress();

// Function declarations for managing server info
void setServerStatus(ConnectionState status);
ConnectionState getServerStatus();

void initializeLcGlobalsMutex();
// Global variables for player data (can be encapsulated later if desired)
extern String playerName;
extern int playerTeam;
extern int playerAmmo;
extern int playerMags;
extern int playerHealth;
extern bool gameInProgress;
#endif // LC_GLOBALS_H
