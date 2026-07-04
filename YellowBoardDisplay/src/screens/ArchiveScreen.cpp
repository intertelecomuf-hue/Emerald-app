/**
 * ArchiveScreen.cpp — [MAP] My Missions
 *
 * Shows 18 numbered missions with alternating row backgrounds,
 * checkmark icons, and a back button at the bottom.
 */

#include "ArchiveScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

// ============================================================
// Mission data
// ============================================================
static const Mission MISSIONS[] = {
    { "[1]",  "Random Numbers + Digital Dice" },
    { "[2]",  "RGB LED Control"               },
    { "[3]",  "Servo Motors"                  },
    { "[4]",  "Math Trainer"                  },
    { "[5]",  "DHT11 Temp + Humidity"         },
    { "[6]",  "Ultrasonic Distance Sensor"    },
    { "[7]",  "Gas Sensor"                    },
    { "[8]",  "Cooling System"                },
    { "[9]",  "Relay Module"                  },
    { "[10]", "IoT + ThingSpeak"              },
    { "[11]", "MIT App Inventor"              },
    { "[12]", "Space Shooter Game"            },
    { "[13]", "Accelerometer"                 },
    { "[14]", "Bluetooth HC-05"               },
    { "[15]", "TX/RX Communication"           },
    { "[16]", "Raspberry Pi Pico Console"     },
    { "[17]", "Web App (todo/clock)"          },
    { "[18]", "Launched on iPhone 4s"         },
};
static const int MISSION_COUNT = 18;

ArchiveScreen::ArchiveScreen() {}

void ArchiveScreen::start() {
    tft.fillScreen(COLOR_BG);
    drawHeader("[ MY MISSIONS ]", false);
    drawContent();
    // No status bar — back button at bottom instead
}

bool ArchiveScreen::update() { return false; }

void ArchiveScreen::handleTap(int x, int y) {
    (void)x; (void)y;
}

void ArchiveScreen::drawContent() {
    // Sub-header: "18 completed" on the right side of header area
    // (drawn after drawHeader which fills the header bar)
    tft.setTextColor(COLOR_DIM, COLOR_PANEL);
    tft.setTextDatum(TR_DATUM);
    tft.setTextSize(1);
    tft.drawString("18 completed", DISP_WIDTH - 8, 7, 1);
    tft.setTextDatum(TL_DATUM);

    // Mission list
    int y = 30;
    int visible = 18;

    for (int i = 0; i < MISSION_COUNT && i < visible; i++) {
        // Alternating row background
        uint16_t rowBg = (i % 2 == 0) ? COLOR_BG : TFT_COLOR(10, 18, 12);
        tft.fillRect(0, y, DISP_WIDTH, 14, rowBg);

        // Checkmark "+"
        tft.setTextColor(COLOR_NEON, rowBg);
        tft.setTextDatum(TL_DATUM);
        tft.setTextSize(1);
        tft.drawString("+", 6, y + 3, 1);

        // Number badge
        tft.setTextColor(COLOR_DIM, rowBg);
        tft.drawString(MISSIONS[i].icon, 18, y + 3, 1);

        // Mission name
        tft.setTextColor(COLOR_TEXT, rowBg);
        tft.drawString(MISSIONS[i].name, 46, y + 3, 1);

        y += 14;
    }

    // Back button at bottom
    int btnY = DISP_HEIGHT - 26;
    tft.drawRoundRect(50, btnY, 140, 18, 4, COLOR_DIM);
    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.setTextDatum(CC_DATUM);
    tft.setTextSize(1);
    tft.drawString("< BACK", 120, btnY + 9, 1);
    tft.setTextDatum(TL_DATUM);
}
