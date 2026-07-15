/**
 * WiFiService.cpp — WiFi connection manager singleton
 */

#include "WiFiService.h"

// ============================================================
// Singleton
// ============================================================
WiFiService& WiFiService::instance() {
    static WiFiService inst;
    return inst;
}

// ============================================================
// Constructor
// ============================================================
WiFiService::WiFiService()
    : _state(WIFI_IDLE)
    , _numNetworks(0)
    , _connectStartTime(0)
    , _lastDataActivity(0)
    , _dataActive(false)
    , _dataToggle(false)
    , _lastCheckTime(0)
{
    _ssid[0] = '\0';
    _password[0] = '\0';
    _ip[0] = '\0';
}

// ============================================================
// begin()
// ============================================================
void WiFiService::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, false);   // wifioff=false — не вимикаємо повністю, лише скидаємо
    delay(100);
    WiFi.mode(WIFI_STA);            // переконуємось, що STA режим активний
    delay(50);
    Serial.println("[WiFiService] Initialized (STA mode)");
}

// ============================================================
// startScan() — async scan
// ============================================================
void WiFiService::startScan() {
    if (_state == WIFI_SCANNING) return;

    // Delete previous scan results to free memory
    WiFi.scanDelete();

    _state = WIFI_SCANNING;
    // Використовуємо синхронне сканування (false) — воно надійніше працює
    // на ESP32. Асинхронне (true) іноді зависає у стані WIFI_SCAN_RUNNING.
    _numNetworks = WiFi.scanNetworks(false);  // sync
    _state = WIFI_IDLE;
    Serial.printf("[WiFiService] Scan complete: %d networks (sync)\\n", _numNetworks);
}

// ============================================================
// scanComplete() — check if async scan finished
// Returns: number of networks or -1 if still scanning
// ============================================================
int WiFiService::scanComplete() {
    if (_state != WIFI_SCANNING) return _numNetworks;

    int result = WiFi.scanComplete();
    if (result >= 0) {
        _numNetworks = result;
        _state = WIFI_IDLE;
        Serial.printf("[WiFiService] Scan complete: %d networks\n", result);
        return result;
    } else if (result == WIFI_SCAN_FAILED) {
        _numNetworks = 0;
        _state = WIFI_IDLE;
        Serial.println("[WiFiService] Scan FAILED");
        return 0;
    }
    return -1;  // still scanning
}

// ============================================================
// connect()
// ============================================================
void WiFiService::connect(const char* ssid, const char* password) {
    strncpy(_ssid, ssid, sizeof(_ssid) - 1);
    _ssid[sizeof(_ssid) - 1] = '\0';
    if (password) {
        strncpy(_password, password, sizeof(_password) - 1);
        _password[sizeof(_password) - 1] = '\0';
    } else {
        _password[0] = '\0';
    }

    _state = WIFI_CONNECTING;
    _connectStartTime = millis();

    Serial.printf("[WiFiService] Connecting to '%s'...\n", _ssid);

    WiFi.begin(_ssid, _password);
}

// ============================================================
// disconnect()
// ============================================================
void WiFiService::disconnect() {
    WiFi.disconnect(true, true);
    _state = WIFI_IDLE;
    _ssid[0] = '\0';
    _ip[0] = '\0';
    Serial.println("[WiFiService] Disconnected");
}

// ============================================================
// State / Status getters
// ============================================================
WiFiState WiFiService::getState() const {
    return _state;
}

bool WiFiService::isConnected() const {
    return (_state == WIFI_CONNECTED);
}

const char* WiFiService::getSSID() const {
    return _ssid;
}

int8_t WiFiService::getRSSI() const {
    if (_state != WIFI_CONNECTED) return -127;
    return WiFi.RSSI();
}

uint8_t WiFiService::getSignalBars() const {
    if (_state != WIFI_CONNECTED) return 0;
    int8_t rssi = WiFi.RSSI();
    if (rssi > -50)      return 4;
    else if (rssi > -65) return 3;
    else if (rssi > -80) return 2;
    else if (rssi > -90) return 1;
    return 0;
}

const char* WiFiService::getIP() const {
    return _ip;
}

// ============================================================
// update() — call every loop iteration
// ============================================================
void WiFiService::update() {
    unsigned long now = millis();

    // --- Connecting timeout & status check ---
    if (_state == WIFI_CONNECTING) {
        // Check if connected
        if (WiFi.status() == WL_CONNECTED) {
            _state = WIFI_CONNECTED;
            strncpy(_ip, WiFi.localIP().toString().c_str(), sizeof(_ip) - 1);
            _ip[sizeof(_ip) - 1] = '\0';
            _lastDataActivity = now;
            _lastCheckTime = now;
            Serial.printf("[WiFiService] Connected! IP: %s\n", _ip);
        }
        // Timeout after 10 seconds
        else if (now - _connectStartTime > 10000) {
            _state = WIFI_FAILED;
            WiFi.disconnect(true, true);
            Serial.println("[WiFiService] Connection timeout");
        }
    }

    // --- Connected: periodic connection health check ---
    if (_state == WIFI_CONNECTED) {
        if (now - _lastCheckTime >= CHECK_INTERVAL) {
            _lastCheckTime = now;
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("[WiFiService] Connection lost!");
                _state = WIFI_FAILED;
                _ssid[0] = '\0';
                _ip[0] = '\0';
            }
        }

        // Data activity simulation — pulse every ~2 seconds to show "traffic"
        if (now - _lastDataActivity >= 2000) {
            _lastDataActivity = now;
            _dataActive = true;
            _dataToggle = !_dataToggle;
        } else if (now - _lastDataActivity >= 1800) {
            // Short active window: 200ms blink every 2s
            _dataActive = (now - _lastDataActivity) < 200;
        }
    }

    // --- Failed: auto-reset after 3 seconds so user can retry ---
    if (_state == WIFI_FAILED && now - _connectStartTime > 13000) {
        _state = WIFI_IDLE;
    }
}

bool WiFiService::isDataActive() const {
    return _dataActive;
}
