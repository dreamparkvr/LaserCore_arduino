#include <lc_gamemanager.h>
#include <lc_display.h>
// Initialize pointer to zero so that it can be initialized in the first call to getInstance

LC_GameManager* LC_GameManager::instance = 0;

LC_GameManager* LC_GameManager::getInstance() {
    if(!instance) {
        instance = new LC_GameManager();
    }
    return instance;
}

void LC_GameManager::Start() {
    isInProgress = true;
}

void LC_GameManager::Stop() {
    isInProgress = false;
}

void LC_GameManager::Reset() {
    isInProgress = false;
}

bool LC_GameManager::IsGameInProgress() const {
    return isInProgress;
}