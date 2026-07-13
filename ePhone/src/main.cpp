/**
 * main.cpp — YellowBoardDisplay (Emerald OS)
 *
 * State machine: BOOT → CALIBRATION → MENU → (various screens)
 *
 * Touch Calibration runs FIRST after boot.
 * After calibration, test mode allows verification.
 * Long press in test mode → go to main menu.
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "tft_setup.h"
#include "services/TouchManager.h"
#include "ui/UIComponents.h"
#include "screens/BootScreen.h"
#include "screens/MenuScreen.h"
#include "screens/CalibrationScreen.h"
#include "screens/AboutScreen.h"
#include "screens/ArchiveScreen.h"
#include "screens/ConsoleScreen.h"
#include "screens/GamesScreen.h"
#include "screens/ProfileScreen.h"
#include "screens/SettingsScreen.h"
#include "screens/SpyScreen.h"
#include "screens/StoryScreen.h"

// ============================================================
// Global objects
// ============================================================
TFT_eSPI tft = TFT_eSPI();
TouchManager touch;

// Screen instances
BootScreen bootScreen;
MenuScreen menuScreen;
CalibrationScreen calibrationScreen;
AboutScreen aboutScreen;
ArchiveScreen archiveScreen;
ConsoleScreen consoleScreen;
GamesScreen gamesScreen;
ProfileScreen profileScreen;
SettingsScreen settingsScreen;
SpyScreen spyScreen;
StoryScreen storyScreen;

// ============================================================
// State machine
// ============================================================
enum AppState : uint8_t {
    STATE_BOOT         = 0,
    STATE_CALIBRATION  = 1,
    STATE_MENU         = 2,
    STATE_ABOUT        = 3,
    STATE_ARCHIVE      = 4,
    STATE_CONSOLE      = 5,
    STATE_GAMES        = 6,
    STATE_PROFILE      = 7,
    STATE_SETTINGS     = 8,
    STATE_SPY          = 9,
    STATE_STORY        = 10,
};

static AppState currentState = STATE_BOOT;
static AppState previousState = STATE_BOOT;

// ============================================================
// Theme
// ============================================================
BackgroundTheme g_backgroundTheme = BG_EMERALD;

// ============================================================
// State transitions
// ============================================================
void transitionTo(AppState newState) {
    previousState = currentState;
    currentState = newState;
}

// ============================================================
// Touch handling
// ============================================================
void handleTouch() {
    TouchEvent ev = touch.getTouch();
    if (ev.action == TOUCH_NONE) return;

    switch (currentState) {
        case STATE_CALIBRATION:
            if (ev.action == TOUCH_TAP) {
                calibrationScreen.handleTap(ev.x, ev.y);
            }
            // Long press in test mode → go to menu
            if (ev.action == TOUCH_LONG_PRESS && calibrationScreen.isTestMode()) {
                Serial.println("[Main] Long press in test mode — exiting to menu");
                calibrationScreen.reset();
                transitionTo(STATE_MENU);
                menuScreen.start();
            }
            break;

        case STATE_MENU:
            menuScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_BOOT:
            bootScreen.handleTap();
            break;

        case STATE_ABOUT:
            aboutScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_ARCHIVE:
            archiveScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_CONSOLE:
            consoleScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_GAMES:
            gamesScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_PROFILE:
            profileScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_SETTINGS:
            settingsScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_SPY:
            spyScreen.handleTap(ev.x, ev.y);
            break;

        case STATE_STORY:
            storyScreen.handleTap(ev.x, ev.y);
            break;

        default:
            break;
    }
}

// ============================================================
// Screen navigation callbacks
// ============================================================
void navigateToMenu() {
    transitionTo(STATE_MENU);
    menuScreen.start();
}

void navigateToAbout() {
    transitionTo(STATE_ABOUT);
    aboutScreen.start();
}

void navigateToArchive() {
    transitionTo(STATE_ARCHIVE);
    archiveScreen.start();
}

void navigateToConsole() {
    transitionTo(STATE_CONSOLE);
    consoleScreen.start();
}

void navigateToGames() {
    transitionTo(STATE_GAMES);
    gamesScreen.start();
}

void navigateToProfile() {
    transitionTo(STATE_PROFILE);
    profileScreen.start();
}

void navigateToSettings() {
    transitionTo(STATE_SETTINGS);
    settingsScreen.start();
}

void navigateToSpy() {
    transitionTo(STATE_SPY);
    spyScreen.start();
}

void navigateToStory() {
    transitionTo(STATE_STORY);
    storyScreen.start();
}

// ============================================================
// Setup
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n========================================");
    Serial.println("  " OS_NAME " " OS_VERSION " — " OS_EDITION);
    Serial.println("  Starting up...");
    Serial.println("========================================");
    Serial.flush();

    // Backlight on
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
    Serial.println("[SETUP] Backlight ON");
    Serial.flush();

    // Initialize TFT
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(COLOR_BLACK);
    Serial.println("[SETUP] TFT initialized");
    Serial.flush();

    // Preliminary touch calibration (will be overwritten after calibration)
    uint16_t calData[5] = { TOUCH_X_MIN, TOUCH_X_MAX, TOUCH_Y_MIN, TOUCH_Y_MAX, TOUCH_SWAP_XY ? 1 : 0 };
    tft.setTouch(calData);
    Serial.println("[SETUP] Preliminary touch calibration set");
    Serial.flush();

    // Initialize TouchManager
    touch.begin(&tft);
    Serial.println("[SETUP] TouchManager ready");
    Serial.flush();

    // UI components and callbacks are initialized by each screen as needed
    Serial.println("[SETUP] UI components ready");
    Serial.flush();

    // Start with boot screen
    transitionTo(STATE_BOOT);
    bootScreen.start();
    Serial.println("[SETUP] Boot screen started");
    Serial.flush();

    Serial.println("[SETUP] Setup complete — entering main loop");
    Serial.flush();
}

// ============================================================
// Main loop
// ============================================================
void loop() {
    handleTouch();

    switch (currentState) {
        case STATE_BOOT:
            if (bootScreen.update()) {
                transitionTo(STATE_CALIBRATION);
                calibrationScreen.start();
            }
            break;

        case STATE_CALIBRATION:
            calibrationScreen.update();
            break;

        case STATE_MENU:
            menuScreen.update();
            break;

        case STATE_ABOUT:
            aboutScreen.update();
            break;

        case STATE_ARCHIVE:
            archiveScreen.update();
            break;

        case STATE_CONSOLE:
            consoleScreen.update();
            break;

        case STATE_GAMES:
            gamesScreen.update();
            break;

        case STATE_PROFILE:
            profileScreen.update();
            break;

        case STATE_SETTINGS:
            settingsScreen.update();
            break;

        case STATE_SPY:
            spyScreen.update();
            break;

        case STATE_STORY:
            storyScreen.update();
            break;

        default:
            break;
    }

    delay(10);
}
