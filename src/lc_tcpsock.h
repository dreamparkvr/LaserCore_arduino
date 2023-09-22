#ifndef LC_TCPSOCK_H
#define LC_TCPSOCK_H

#include <Arduino.h>
#include <WiFi.h>

void clientTask(void * parameters);
void sendMessageToServer(int messageType, const char* payload);
#endif