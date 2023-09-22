#include <lc_wifi.h>
#include <lc_globals.h>
#include <lc_display.h>
#define WIFI_NETWORK "Fios-rHCb9"
#define WIFI_PASS "dozen54wit68mesh"
#define WIFI_TIMEOUT_MS 20000

bool isWifiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void wifiKeepAlive(void * parameters)
{
  for(;;)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      // Compose the IP address message
      String ipMessage = WiFi.localIP().toString();
      // Use the SetFooter method
      SetWifiStatus(ConnectionState::CONNECTED, ipMessage.c_str());

      SetWifiStatusArea();
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;

    }
    SetWifiStatusArea();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASS);

    unsigned long startAttemptTime = millis();

    while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){}

    if(WiFi.status() != WL_CONNECTED)
    {
      SetWifiStatusArea();
      vTaskDelay(20000 / portTICK_PERIOD_MS);
      continue;
    }
  }
}