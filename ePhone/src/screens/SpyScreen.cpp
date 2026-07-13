/**
 * SpyScreen.cpp — Spy Terminal: WiFi Scanner + Network Selection + Connect
 *
 * State machine: SCAN -> LIST -> (tap network) -> (password if needed)
 *              -> CONNECTING -> CONNECTED | FAILED
 */
#include "SpyScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"
#include "../services/WiFiService.h"
#include <WiFi.h>

extern TFT_eSPI tft;

#define TERM_GREEN      TFT_COLOR(0, 255, 65)
#define TERM_GREEN_DIM  TFT_COLOR(0, 180, 40)
#define TERM_ACCENT     TFT_COLOR(0, 255, 180)
#define TERM_RED        TFT_COLOR(255, 80, 80)

static const char* encTypeStr(uint8_t enc) {
    switch (enc) {
        case WIFI_AUTH_OPEN:            return "OPEN";
        case WIFI_AUTH_WPA_PSK:         return "WPA";
        case WIFI_AUTH_WPA2_PSK:        return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-E";
        case WIFI_AUTH_WPA3_PSK:        return "WPA3";
        default:                        return "???";
    }
}

SpyScreen::SpyScreen()
    : _mode(SPY_MODE_SCAN), _scanStartTime(0), _scanning(false)
    , _numNetworks(0), _selectedNetwork(-1)
    , _lastCursorBlink(0), _cursorVisible(true), _cursorX(10), _cursorY(70)
    , _connectStartTime(0), _connectingDots(false)
{ _connectedIP[0] = '\0'; }

void SpyScreen::start() {
    tft.fillScreen(TFT_BLACK);
    _mode = SPY_MODE_SCAN;
    _scanning = true;
    _selectedNetwork = -1;
    _scanStartTime = millis();
    _numNetworks = 0;
    _lastCursorBlink = 0;
    _cursorVisible = true;
    _connectedIP[0] = '\0';

    drawHeader("СКАНЕР WI-FI", true);

    tft.setTextColor(TERM_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("> Запуск Wi-Fi сканера...", 10, HEADER_HEIGHT + 10, 2);
    tft.drawString("> Пошук мереж...",       10, HEADER_HEIGHT + 30, 2);

    Serial.println("[SpyScreen] Starting WiFi scan...");
    Serial.printf("[SpyScreen] WiFi mode: %d, status: %d\n",
                  WiFi.getMode(), WiFi.status());

    _cursorX = 10;
    _cursorY = HEADER_HEIGHT + 50;

    WiFiService::instance().startScan();
}

bool SpyScreen::update() {
    unsigned long now = millis();

    switch (_mode) {
        case SPY_MODE_SCAN: {
            if (now - _lastCursorBlink >= 500) {
                _lastCursorBlink = now;
                _cursorVisible = !_cursorVisible;
                tft.setTextColor(_cursorVisible ? TERM_GREEN : TFT_BLACK, TFT_BLACK);
                tft.setTextSize(1);
                tft.drawChar('_', _cursorX, _cursorY, 2);
            }
            int result = WiFiService::instance().scanComplete();
            if (result >= 0) {
                _numNetworks = result;
                _scanning = false;
                _mode = SPY_MODE_LIST;
                drawList();
            }
            break;
        }
        case SPY_MODE_LIST:
            break;
        case SPY_MODE_CONNECTING:
            drawConnecting();
            break;
        case SPY_MODE_CONNECTED:
            break;
        case SPY_MODE_FAILED:
            break;
    }
    return false;
}

void SpyScreen::handleTap(int x, int y) {
    if (y <= HEADER_HEIGHT) return;

    switch (_mode) {
        case SPY_MODE_LIST: {
            int startY = HEADER_HEIGHT + 26;
            int lineH = 18;
            int idx = (y - startY) / lineH;
            if (idx >= 0 && idx < _numNetworks) {
                _selectedNetwork = idx;
                drawList(); // redraw with highlight
            }
            break;
        }
        case SPY_MODE_CONNECTED:
        case SPY_MODE_FAILED:
            start();
            break;
        default:
            break;
    }
}

SpyMode SpyScreen::getMode() const { return _mode; }

void SpyScreen::getSelectedSSID(char* buf, int maxLen) const {
    buf[0] = '\0';
    if (_selectedNetwork >= 0 && _selectedNetwork < _numNetworks) {
        String ssid = WiFi.SSID(_selectedNetwork);
        strncpy(buf, ssid.c_str(), maxLen - 1);
        buf[maxLen - 1] = '\0';
    }
}

bool SpyScreen::isPasswordNeeded() const {
    if (_selectedNetwork < 0 || _selectedNetwork >= _numNetworks) return false;
    return WiFi.encryptionType(_selectedNetwork) != WIFI_AUTH_OPEN;
}

void SpyScreen::setConnecting() {
    _mode = SPY_MODE_CONNECTING;
    _connectStartTime = millis();
    _connectingDots = false;
    tft.fillScreen(TFT_BLACK);
    drawHeader("ПIДКЛ. WI-FI", true);
}

void SpyScreen::setConnected(const char* ip) {
    _mode = SPY_MODE_CONNECTED;
    strncpy(_connectedIP, ip, sizeof(_connectedIP) - 1);
    _connectedIP[sizeof(_connectedIP) - 1] = '\0';
    drawConnected();
}

void SpyScreen::drawList() {
    tft.fillScreen(TFT_BLACK);
    drawHeader("СКАНЕР WI-FI", true);

    tft.setTextColor(TERM_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    char buf[48];
    snprintf(buf, sizeof(buf), "> Знайдено мереж: %d", _numNetworks);
    tft.drawString(buf, 10, HEADER_HEIGHT + 4, 2);

    int startY = HEADER_HEIGHT + 24;
    int lineH = 18;
    int maxLines = (DISP_HEIGHT - startY - STATUSBAR_HEIGHT - 6) / lineH;
    int lines = (_numNetworks < maxLines) ? _numNetworks : maxLines;

    for (int i = 0; i < lines; i++) {
        drawNetworkLine(i, startY + i * lineH, (i == _selectedNetwork));
    }

    if (_numNetworks == 0) {
        tft.setTextColor(TERM_GREEN_DIM, TFT_BLACK);
        tft.drawString("> Мереж не знайдено", 10, startY, 2);
    }

    tft.setTextColor(TERM_GREEN_DIM, TFT_BLACK);
    tft.drawString("> Натисни на мережу щоб пiдключитись", 10, DISP_HEIGHT - STATUSBAR_HEIGHT - 12, 1);

    drawStatusBar("ТАП ЩОБ ПIДКЛ.", "09:41");
    WiFi.scanDelete();
}

void SpyScreen::drawNetworkLine(int index, int y, bool highlighted) {
    String ssid = WiFi.SSID(index);
    int rssi = WiFi.RSSI(index);
    uint8_t enc = WiFi.encryptionType(index);

    int bars = 0;
    if (rssi > -50)      bars = 4;
    else if (rssi > -65) bars = 3;
    else if (rssi > -80) bars = 2;
    else if (rssi > -90) bars = 1;

    char barStr[5];
    memset(barStr, '|', bars);
    barStr[bars] = '\0';

    if (highlighted) {
        tft.fillRect(6, y - 1, DISP_WIDTH - 12, 17, TFT_COLOR(0, 40, 15));
    }

    char line[64];
    snprintf(line, sizeof(line), "> [%-4s] %s", barStr, ssid.c_str());
    tft.setTextColor(highlighted ? TERM_ACCENT : TERM_GREEN,
                     highlighted ? TFT_COLOR(0,40,15) : TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString(line, 10, y, 2);

    tft.setTextColor(TERM_GREEN_DIM, highlighted ? TFT_COLOR(0,40,15) : TFT_BLACK);
    char info[28];
    const char* lock = (enc != WIFI_AUTH_OPEN) ? "LOCK" : "";
    snprintf(info, sizeof(info), "%s(%s,%ddBm)", lock, encTypeStr(enc), rssi);
    tft.drawString(info, 230, y, 1);
}


void SpyScreen::setFailed() {
    _mode = SPY_MODE_FAILED;
    drawFailed();
}


void SpyScreen::drawConnecting() {
    unsigned long now = millis();
    unsigned long elapsed = now - _connectStartTime;

    int dotCount = (elapsed / 500) % 4;
    char dots[4] = {0,0,0,0};
    for (int i = 0; i < dotCount; i++) dots[i] = '.';

    tft.fillScreen(TFT_BLACK);
    drawHeader("ПIДКЛ. WI-FI", true);

    tft.setTextColor(TERM_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("> Пiдключення до мережi", 10, HEADER_HEIGHT + 20, 2);

    char buf[64];
    char ssidBuf[33];
    getSelectedSSID(ssidBuf, sizeof(ssidBuf));
    snprintf(buf, sizeof(buf),"> %s%s", ssidBuf, dots);
    tft.drawString(buf, 10, HEADER_HEIGHT + 42, 2);

    int progress = (elapsed / 100) % 100;
    drawProgressBar(10, HEADER_HEIGHT + 70, 200, 12, progress, TERM_GREEN);

    drawStatusBarType("ПIДКЛЮЧЕННЯ...", COLOR_YELLOW);
}

void SpyScreen::drawConnected() {
    tft.fillScreen(TFT_BLACK);
    drawHeader("ПIДКЛЮЧЕНО", true);

    tft.setTextColor(TERM_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    char buf[64];
    char ssidBuf[33];
    getSelectedSSID(ssidBuf, sizeof(ssidBuf));
    snprintf(buf, sizeof(buf), "> Пiдключено до: %s", ssidBuf);
    tft.drawString(buf, 10, HEADER_HEIGHT + 20, 2);

    tft.setTextColor(TERM_ACCENT, TFT_BLACK);
    snprintf(buf, sizeof(buf), "> IP: %s", _connectedIP);
    tft.drawString(buf, 10, HEADER_HEIGHT + 44, 2);

    tft.setTextColor(TERM_GREEN_DIM, TFT_BLACK);
    tft.drawString("> Сигнал: " + String(WiFiService::instance().getSignalBars()) + "/4",
                    10, HEADER_HEIGHT + 68, 2);

    tft.setTextColor(TERM_GREEN_DIM, TFT_BLACK);
    tft.drawString("> Натисни щоб вiдключитись", 10, DISP_HEIGHT - STATUSBAR_HEIGHT - 12, 1);

    drawWiFiStatusBar(ssidBuf, WiFiService::instance().getSignalBars(),
                      WiFiService::instance().isDataActive(), "ПIДКЛЮЧЕНО");
}

void SpyScreen::drawFailed() {
    tft.fillScreen(TFT_BLACK);
    drawHeader("ПОМИЛКА WI-FI", true);

    tft.setTextColor(TERM_RED, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("! Не вдалося пiдключитись", 10, HEADER_HEIGHT + 20, 2);

    tft.setTextColor(TERM_GREEN_DIM, TFT_BLACK);
    tft.drawString("> Перевiр пароль та спробуй ще", 10, HEADER_HEIGHT + 44, 2);
    tft.drawString("> Натисни щоб повернутись", 10, HEADER_HEIGHT + 66, 2);

    drawStatusBarType("ПОМИЛКА WI-FI", TERM_RED);
}
