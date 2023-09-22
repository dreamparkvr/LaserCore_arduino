#ifndef LC_DISPLAY_H
#define LC_DISPLAY_H

#include <Arduino.h>
#include<TFT_eSPI.h>


extern TFT_eSPI tft;
void initializeDisplay();
void SetGameStatus(int statusId, const char* statusMsg);
void SetWifiStatusArea();
void SetAmmoLevel(int rounds, int mags);
void SetLoadingScreen();
void SetConnIdScreen(int id);
void SetPlayerWaitScreen(String playerName, int teamId);
void SetGameScreen();
#endif