#pragma once

class LC_GameManager {
    private:
    bool isInProgress;
    static LC_GameManager* instance;

    LC_GameManager() : isInProgress(false) {}

    public:

    static LC_GameManager* getInstance();

    void Start();
    void Stop();
    void Reset();
    bool IsGameInProgress() const; 



};