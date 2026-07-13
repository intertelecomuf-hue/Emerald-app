/**
 * SpyScreen.h — Spy Terminal: WiFi Scanner, Network Selection & Connect
 *
 * Modes: SCAN → LIST → (PASSWORD via external keyboard) → CONNECTING → CONNECTED/FAILED
 */

#ifndef SPY_SCREEN_H
#define SPY_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

enum SpyMode {
    SPY_MODE_SCAN,
    SPY_MODE_LIST,
    SPY_MODE_CONNECTING,
    SPY_MODE_CONNECTED,
    SPY_MODE_FAILED
};

class SpyScreen {
public:
    SpyScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

    SpyMode getMode() const;
    void getSelectedSSID(char* buf, int maxLen) const;
    bool isPasswordNeeded() const; // encrypted network?
    void setConnecting();
    void setConnected(const char* ip);
    void setFailed();

private:
    SpyMode _mode;
    unsigned long _scanStartTime;
    bool _scanning;
    int  _numNetworks;
    int  _selectedNetwork;

    // Blinking cursor
    unsigned long _lastCursorBlink;
    bool _cursorVisible;
    int  _cursorX, _cursorY;

    // Connection animation
    unsigned long _connectStartTime;
    bool _connectingDots;
    char _connectedIP[16];

    // Drawing helpers
    void drawScanning();
    void drawList();
    void drawConnecting();
    void drawConnected();
    void drawFailed();
    void drawNetworkLine(int index, int y, bool highlighted);
};

#endif // SPY_SCREEN_H
