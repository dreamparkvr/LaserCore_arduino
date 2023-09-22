#include <lc_tcpsock.h>
#include <Arduino.h>
#include <lc_display.h>
#include <lc_wifi.h>
#include <ByteBuffer.h>
#include <lc_globals.h>
#include <lc_triggerLogic.h>

#define SVR_IP "192.168.1.19"
#define SVR_PORT 12345
#define CONN_TIMEOUT 10000

WiFiClient client;

enum ReadState {
    READING_TYPE,
    READING_LENGTH,
    READING_PAYLOAD
};

ReadState currentState = READING_TYPE;
int pendingPayloadLength = 0;
int messageType = 0;

void handleMessage(int messageType, ByteBuffer& buffer) {
    if (xSemaphoreTake(lcGlobalsMutex, portMAX_DELAY) == pdTRUE) { 
        // Depending on the messageType, you'll read from the buffer accordingly
        if (messageType == 0) {
            int connectionId = buffer.ReadInt();
            SetConnIdScreen(connectionId);
        }

        if(messageType == 1) {
            playerName = buffer.ReadString();
            playerTeam = buffer.ReadInt();
            playerMags = buffer.ReadInt();
            playerAmmo = buffer.ReadInt();
            playerHealth = buffer.ReadInt();
            
            SetPlayerWaitScreen(playerName, playerTeam);

            Serial.println("Assigned " + playerName + " to team# " + playerTeam + " with " + playerAmmo + " ammo, " + playerMags + " mags, and " + playerHealth + " health.");
        }

        if(messageType == 2)
        {
            bool inProg = buffer.ReadBool();
            gameInProgress = inProg;
            SetGameScreen();
            initTrigger();
        }
        xSemaphoreGive(lcGlobalsMutex);
    }

}


void clientTask(void * parameters) {
    ByteBuffer recvBuffer(1024);  // buffer for incoming data

    while (true) {  
        // Check if WiFi is connected
        if (isWifiConnected()) {

            // Try to establish a connection with the server if the client isn't already connected
            if (!client.connected()) {
                unsigned long startAttemptTime = millis();
                bool connectionAttempt = client.connect(SVR_IP, SVR_PORT);
                SetGameStatus(0, "[CONNECTING]");

                // Wait for connection attempt to finish or timeout
                while (!connectionAttempt && millis() - startAttemptTime < CONN_TIMEOUT) {
                    connectionAttempt = client.connect(SVR_IP, SVR_PORT);
                    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for 100ms between connection attempts
                }

                if (!client.connected()) {  // Check if connection attempt failed
                    SetGameStatus(1, "[FAULT]");
                    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Wait and then try again
                } else {
                    SetGameStatus(0, "[CONNECTED]");
                }
            }
            
            if (client.connected()) {
                while (client.available()) {
                    switch (currentState) {
                        case READING_TYPE:
                            if (client.available() >= sizeof(int)) {
                                byte buffer[4];
                                client.readBytes(buffer, 4);
                                recvBuffer.WriteInt(*(int*)buffer);
                                messageType = recvBuffer.ReadInt();
                                currentState = READING_LENGTH;
                            }
                            break;

                        case READING_LENGTH:
                            if (client.available() >= sizeof(int)) {
                                byte buffer[4];
                                client.readBytes(buffer, 4);
                                recvBuffer.WriteInt(*(int*)buffer);
                                pendingPayloadLength = recvBuffer.ReadInt();
                                currentState = READING_PAYLOAD;
                            }
                            break;

                        case READING_PAYLOAD:
                            if (client.available() >= pendingPayloadLength) {
                                byte payload[pendingPayloadLength];
                                client.readBytes(payload, pendingPayloadLength);
                                recvBuffer.WriteBytes(payload, pendingPayloadLength);
                                handleMessage(messageType, recvBuffer);  // Pass ByteBuffer directly
                                currentState = READING_TYPE;
                            }
                            break;
                    }
                }
                vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to prevent hogging CPU time
            } else {
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        } else {
            vTaskDelay(5000 / portTICK_PERIOD_MS);  // Wait for a while if WiFi is not connected
        }
    }
}


void sendMessageToServer(int messageType, const char* payload) {
    if (client.connected()) {
        // Convert the message type to a byte array and send it
        client.write((const uint8_t*)&messageType, sizeof(int));

        // Convert the payload length to a byte array and send it
        int payloadLength = strlen(payload);
        client.write((const uint8_t*)&payloadLength, sizeof(int));

        // Send the payload itself
        client.write(payload);

        // Print for debugging purposes
        Serial.printf("Sent message of type %d with payload: %s\n", messageType, payload);
    }
}
