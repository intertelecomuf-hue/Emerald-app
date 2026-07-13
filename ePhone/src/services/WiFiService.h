/**
 * WiFiService.h — WiFi connection manager singleton
 *
 * Handles scanning, connecting, and status tracking.
 * Provides signal strength, data activity simulation, and state machine.
 */

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>

// ============================================================
// WiFi States
// ============================================================
enum WiFiState {
    WIFI_IDLE,
    WIFI_SCANNING,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_FAILED
};

// ============================================================
// WiFi Service Singleton
// ============================================================
class WiFiService {
public:
    static WiFiService& instance();

    void begin();

    // Scan
    void startScan();
    int  scanComplete();     // returns count or -1 if still scanning

    // Connect
    void connect(const char* ssid, const char* password);
    void disconnect();

    // Status
    WiFiState getState() const;
    bool      isConnected() const;
    const char* getSSID() const;
    int8_t    getRSSI() const;
    uint8_t  getSignalBars() const;   // 0-4
    const char* getIP() const;

    // Data activity simulation (network check ping every N ms)
    void update();   // call every loop iteration
    bool isDataActive() const;  // true when blipping TX/RX indicator

private:
    WiFiService();
    WiFiService(const WiFiService&) = delete;
    WiFiService& operator=(const WiFiService&) = delete;

    WiFiState _state;
    char      _ssid[33];
    char      _password[65];
    char      _ip[16];

    // Scan
    int       _numNetworks;

    // Connection
    unsigned long _connectStartTime;
    unsigned long _lastDataActivity;
    bool          _dataActive;
    bool          _dataToggle;     // toggles every activity pulse

    // Connection monitoring
    unsigned long _lastCheckTime;
    static const unsigned long CHECK_INTERVAL = 5000;  // 5s between checks
};

#endif // WIFI_SERVICE_H
