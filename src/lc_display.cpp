#include <lc_display.h>
#include <lc_globals.h>
#include <string.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite footer = TFT_eSprite(&tft);
TFT_eSprite wifi_area = TFT_eSprite(&tft);
TFT_eSprite ammo_area = TFT_eSprite(&tft);
TFT_eSprite loading_scr = TFT_eSprite(&tft);


void SetLoadingScreen()
{
    loading_scr.setTextSize(2);
    loading_scr.fillSprite(TFT_BLACK);
    loading_scr.drawCentreString("LaserCORE",67, 80, 2);
    loading_scr.setTextSize(1);
    loading_scr.setTextColor(TFT_RED);
    loading_scr.drawCentreString("ChaOS v0.0",67, 110, 2);
    loading_scr.setTextColor(TFT_WHITE);
    loading_scr.drawCentreString("By Tom Whalen",67, 140, 2);
    loading_scr.pushSprite(0,0);
}

void SetConnIdScreen(int id)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("READY", 67, 10, 2);
    tft.setTextSize(5);
    String str = String(id);
    tft.drawCentreString(str, 67, 65, 2);

}

void SetPlayerWaitScreen(String playerName, int teamId)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);


    if(teamId == 1)
    {
        tft.setTextColor(TFT_RED);
        tft.drawCentreString("RED",67,130,2);
    }

    if(teamId == 2)
    {
        tft.setTextColor(TFT_BLUE);
        tft.drawCentreString("BLUE",67,130,2);
        
    }
    tft.drawCentreString("TEAM",67,170,2);
    tft.drawCentreString(playerName, 67, 10, 2);


}

void SetGameScreen()
{
    tft.fillScreen(TFT_BLACK);
    SetAmmoLevel(playerAmmo, playerMags);
}


void SetGameStatus(int statusId, const char* statusMsg)
{
    footer.setTextFont(2);
    footer.fillSprite(TFT_BLACK);
    footer.drawString("SRV:",0,0);
    footer.drawString(statusMsg,50,0);
    footer.pushSprite(0,225);
}

void SetWifiStatusArea()
{
    wifi_area.setTextFont(2);
    wifi_area.fillSprite(TFT_BLACK);
    wifi_area.setTextColor(TFT_WHITE);
    wifi_area.drawString("WIFI:",0,0);

    if(wifiInfo.status == ConnectionState::CONNECTED)
    {
        wifi_area.setTextColor(TFT_GREEN);
        wifi_area.drawString(wifiInfo.ip,40,0);
    } else if (wifiInfo.status == ConnectionState::CONNECTING) {
        wifi_area.setTextColor(TFT_YELLOW);
        wifi_area.drawString("[CONNECTING]",40,0);

    } else if (wifiInfo.status == ConnectionState::DISCONNECTED) {
        wifi_area.setTextColor(TFT_RED);
        wifi_area.drawString("[DISCONNECTED]",40,0);

    } else if (wifiInfo.status == ConnectionState::FAULT) {
        wifi_area.setTextColor(TFT_RED);
    } else {
        wifi_area.setTextColor(TFT_RED);
    }
    
    wifi_area.pushSprite(0,205);
}

void SetAmmoLevel(int rounds, int mags)
{
   // Serial.println(rounds + " d " + mags);
    ammo_area.setTextFont(4);
    ammo_area.fillSprite(TFT_BLACK);
    ammo_area.setTextColor(TFT_WHITE);
    if(rounds > 0)
    {
        String s = "R " + String(rounds) + " :  M " + String(mags);
        ammo_area.drawString(s,0,0);
    } else {
        ammo_area.setTextColor(TFT_RED);
        ammo_area.drawString("[RELOAD]", 10, 2);
    }
    
    ammo_area.pushSprite(0,0);
}


void initializeDisplay()
{
    tft.init();
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setTextFont(2);    
    footer.createSprite(135, 15);
    wifi_area.createSprite(135, 15);
    ammo_area.createSprite(135,25);
    loading_scr.createSprite(135,240);

    SetLoadingScreen();
}

