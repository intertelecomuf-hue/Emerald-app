/**
 * VirtualKeyboard.cpp — On-screen keyboard for WiFi password entry
 */
#include "VirtualKeyboard.h"
#include "../config.h"
#include "UIComponents.h"

extern TFT_eSPI tft;

#define KB_BG        TFT_COLOR(10, 14, 11)
#define KB_KEY_BG    TFT_COLOR(20, 35, 25)
#define KB_KEY_TEXT  TFT_COLOR(200, 235, 210)
#define KB_SPECIAL   TFT_COLOR(40, 60, 45)
#define KB_OK_BG     TFT_COLOR(57, 217, 122)
#define KB_OK_TEXT   TFT_COLOR(6, 10, 7)
#define KB_INPUT_BG  TFT_COLOR(6, 10, 7)
#define KB_INPUT_BORDER TFT_COLOR(57, 217, 122)

VirtualKeyboard::VirtualKeyboard() : _keyCount(0), _passLen(0), _done(false) {
    _ssid[0] = '\0'; _password[0] = '\0';
}

void VirtualKeyboard::begin(const char* ssid) {
    strncpy(_ssid, ssid, sizeof(_ssid)-1);
    _ssid[sizeof(_ssid)-1] = '\0';
    _password[0] = '\0'; _passLen = 0; _done = false;
    buildLayout(); draw();
}

void VirtualKeyboard::buildLayout() {
    _keyCount = 0;
    _inputY = HEADER_HEIGHT + 4;
    _kbX = 2; _kbW = DISP_WIDTH - 4;
    _kbY = _inputY + 28;
    _kbH = DISP_HEIGHT - STATUSBAR_HEIGHT - _kbY - 2;

    const char* rows[5] = {
        "Q W E R T Y U I O P",
        "A S D F G H J K L ",
        "Z X C V B N M     ",
        "1 2 3 4 5 6 7 8 9 0",
        ". @ - _ ! BS CLR OK"
    };
    int rowH = _kbH / 5;
    if (rowH > 30) rowH = 30;
    int gap = 2;

    for (int r = 0; r < 5; r++) {
        const char* s = rows[r];
        int nk = 0;
        for (const char* p = s; *p; p++) if (*p != ' ' && (p == s || *(p-1) == ' ')) nk++;
        int kw = (_kbW - (nk-1)*gap) / nk;
        if (kw > 30) kw = 30;
        int rw = nk * kw + (nk-1)*gap;
        int sx = _kbX + (_kbW - rw)/2;

        int ci = 0;
        const char* p = s;
        while (*p) {
            if (*p == ' ') { p++; continue; }
            char lb[4] = {0,0,0,0}; int li=0;
            while (*p && *p != ' ' && li < 3) lb[li++] = *p++;
            if (*p == ' ') p++;
            KeyDef& k = _keys[_keyCount];
            strncpy(k.label, lb, 3); k.label[3]='\0';
            if (r==4) {
                if (strcmp(lb,"BS")==0) k.value=1;
                else if (strcmp(lb,"CLR")==0) k.value=2;
                else if (strcmp(lb,"OK")==0) k.value=3;
                else k.value=lb[0];
            } else { k.value=lb[0]; }
            k.x = sx + ci*(kw+gap);
            k.y = _kbY + r*rowH + 2;
            k.w = kw; k.h = rowH-4;
            if (r==4 && (k.value==1||k.value==2||k.value==3)) k.w = kw+6;
            _keyCount++; ci++;
        }
    }
}


void VirtualKeyboard::draw() {
    tft.fillRect(0, HEADER_HEIGHT, DISP_WIDTH, DISP_HEIGHT-HEADER_HEIGHT, KB_BG);
    drawHeader("ПАРОЛЬ WI-FI", true);
    tft.fillRect(6, _inputY, DISP_WIDTH-12, 22, KB_INPUT_BG);
    tft.drawRect(6, _inputY, DISP_WIDTH-12, 22, KB_INPUT_BORDER);
    tft.setTextColor(COLOR_NEON2, KB_BG);
    tft.setTextSize(1);
    char buf[64];
    snprintf(buf,sizeof(buf),"Введіть пароль вiд мережi: %s",_ssid);
    tft.drawString(buf,10,_inputY-16,1);
    drawInputField(); drawKeys();
    drawStatusBarType("ВВЕДІТЬ ПАРОЛЬ", COLOR_YELLOW);
}

void VirtualKeyboard::drawInputField() {
    tft.fillRect(8, _inputY+2, DISP_WIDTH-16, 18, KB_INPUT_BG);
    char m[KEYBOARD_MAX_PASS+1];
    for (int i=0;i<_passLen;i++) m[i]='*';
    m[_passLen]='\0';
    tft.setTextColor(COLOR_NEON, KB_INPUT_BG);
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(m,10,_inputY+12,2);
    bool cOn = (millis()/400)%2==0;
    if (cOn) {
        int cx = 10 + tft.textWidth(m,2) + 2;
        tft.fillRect(cx,_inputY+4,2,14,COLOR_NEON);
    }
    tft.setTextDatum(TL_DATUM);
}

void VirtualKeyboard::drawKeys() {
    for (int i=0;i<_keyCount;i++) {
        const KeyDef& k = _keys[i];
        bool isOK = (k.value==3);
        bool isSp = (k.value==1||k.value==2);
        uint16_t bg = isOK ? KB_OK_BG : (isSp ? KB_SPECIAL : KB_KEY_BG);
        uint16_t tc = isOK ? KB_OK_TEXT : KB_KEY_TEXT;
        tft.fillRoundRect(k.x,k.y,k.w,k.h,3,bg);
        tft.drawRoundRect(k.x,k.y,k.w,k.h,3,COLOR_DIM);
        tft.setTextColor(tc,bg); tft.setTextSize(1);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(k.label,k.x+k.w/2,k.y+k.h/2,1);
    }
    tft.setTextDatum(TL_DATUM);
}

void VirtualKeyboard::highlightKey(int idx) {
    if (idx<0||idx>=_keyCount) return;
    const KeyDef& k = _keys[idx];
    tft.fillRoundRect(k.x,k.y,k.w,k.h,3,COLOR_NEON);
    tft.setTextColor(KB_BG,COLOR_NEON);
    tft.setTextSize(1); tft.setTextDatum(CC_DATUM);
    tft.drawString(k.label,k.x+k.w/2,k.y+k.h/2,1);
    tft.setTextDatum(TL_DATUM);
    delay(80); drawKeys();
}

bool VirtualKeyboard::handleTap(int x, int y) {
    if (_done) return true;
    if (y <= HEADER_HEIGHT) {
        if (isBackButtonPressed(x,y)) { _done=true; _password[0]='\0'; return true; }
        return false;
    }
    for (int i=0;i<_keyCount;i++) {
        const KeyDef& k = _keys[i];
        if (x>=k.x&&x<=k.x+k.w&&y>=k.y&&y<=k.y+k.h) {
            highlightKey(i);
            if (k.value==1) backspace();
            else if (k.value==2) clearPassword();
            else if (k.value==3) { _done=true; return true; }
            else addChar(k.value);
            drawInputField();
            return false;
        }
    }
    return false;
}

void VirtualKeyboard::addChar(char c) {
    if (_passLen < KEYBOARD_MAX_PASS-1) {
        _password[_passLen++] = c;
        _password[_passLen] = '\0';
    }
}

void VirtualKeyboard::backspace() {
    if (_passLen > 0) { _passLen--; _password[_passLen]='\0'; }
}

void VirtualKeyboard::clearPassword() {
    _passLen=0; _password[0]='\0';
}

const char* VirtualKeyboard::getPassword() const { return _password; }
const char* VirtualKeyboard::getSSID() const { return _ssid; }

bool VirtualKeyboard::update() {
    if (!_done) drawInputField();
    return _done;
}
