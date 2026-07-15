/**
 * SpyScreen.h — Spy Terminal: WiFi Scanner + Bluetooth Scanner
 *
 * Modes: SCAN → LIST → (PASSWORD via external keyboard) → CONNECTING → CONNECTED/FAILED
 * Bluetooth: BT_SCAN → BT_LIST → BT_CONNECTING → BT_CONNECTED/BT_FAILED
 */

#ifndef SPY_SCREEN_H
#define SPY_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

enum SpyMode {
    SPY_MODE_SCAN,
    SPY_MODE_LIST,
    SPY_MODE_PASSWORD,  // Enter password via keyboard
    SPY_MODE_CONNECTING,
    SPY_MODE_CONNECTED,
    SPY_MODE_FAILED,
    SPY_MODE_EXIT,      // Request to exit back to menu
    // Bluetooth modes
    SPY_MODE_BT_SCAN,
    SPY_MODE_BT_LIST,
    SPY_MODE_BT_CONNECTING,
    SPY_MODE_BT_CONNECTED,
    SPY_MODE_BT_FAILED
};

class SpyScreen {
public:
    SpyScreen();

    void start();
    bool update();
    void handleTap(int x, int y);
    void handleKey(char key);  // Keyboard control

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

    // Connection
    unsigned long _connectStartTime;
    bool _connectingDots;
    char _connectedIP[16];
    char _password[65];  // Password buffer for keyboard input

    // Drawing helpers
    void drawScanning();
    void drawList();
    void drawPasswordScreen();
    void updatePasswordField();
    void updateConnectingDots();
    void drawConnecting();
    void drawConnected();
    void drawFailed();
    void drawNetworkLine(int index, int y, bool highlighted);

    // Bluetooth helpers
    void startBTScreen();
    void drawBTList();
    void drawBTDeviceLine(int index, int y, bool highlighted);
    void drawBTConnecting();
    void drawBTConnected();
    void drawBTFailed();
};

#endif // SPY_SCREEN_H
