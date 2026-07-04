/**
 * UIComponents.h — Reusable UI drawing primitives
 *
 * Buttons, cards, progress bars, status bar, signal bars, dots.
 */

#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft; // defined in main.cpp

// ============================================================
// Back Button
// ============================================================
void drawBackButton();
bool isBackButtonPressed(int x, int y);

// ============================================================
// Card (rounded rectangle with border)
// ============================================================
void drawCard(int x, int y, int w, int h, uint16_t borderColor);

// ============================================================
// Progress Bar
// ============================================================
void drawProgressBar(int x, int y, int w, int h,
                     int percent, uint16_t color);

// ============================================================
// Status Bar (bottom of screen)
// ============================================================
void drawStatusBar(const char* status, const char* timeStr);

// ============================================================
// Status Bar with color (for password entry, connecting, etc.)
// ============================================================
void drawStatusBarType(const char* status, uint16_t color);

// ============================================================
// WiFi Status Bar — shows signal bars + SSID + data activity
// ============================================================
void drawWiFiStatusBar(const char* ssid, int signalBars,
                       bool dataActive, const char* timeStr);

// ============================================================
// Dot Indicators (for story mode cards)
// ============================================================
void drawDots(int total, int current, int y);

// ============================================================
// Signal Bars (for WiFi scanner)
// ============================================================
void drawSignalBars(int x, int y, int strength); // strength 1-4

// ============================================================
// Header with title + optional back button area
// ============================================================
void drawHeader(const char* title, bool showBack);

// ============================================================
// Highlight a tile on press
// ============================================================
void highlightTile(int x, int y, int w, int h);

#endif // UI_COMPONENTS_H
