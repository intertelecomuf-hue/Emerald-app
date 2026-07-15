# SIM OS — Final Slide fix + screens MSG, USR, MAP

## Промт для агента — Emerald

-----

## ПРАВКА 1 — Final Slide: виправити відображення “Fedir.”

Проблема — великий текст `Fedir.` з `setTextSize(4)` не відображається,
видно тільки маленький піксель. Причина — `setTextSize` на великих розмірах
може не працювати з `MC_DATUM` коректно на деяких версіях TFT_eSPI.

Замінити `drawFinalSlide()` повністю:

```cpp
void drawFinalSlide() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // "Fedir." — малювати двічі для glow ефекту
  // Спочатку приглушений фон-glow
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(tft.color565(20, 80, 40), tft.color565(6, 10, 7));
  tft.drawString("Fedir.", 161, 71);
  tft.drawString("Fedir.", 159, 69);

  // Основний текст
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("Fedir.", 160, 70);

  // Підзаголовок
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(208, 240, 216), tft.color565(6, 10, 7));
  tft.drawString("This is only the beginning.", 160, 115);

  // Кнопка з рамкою
  tft.drawRoundRect(40, 145, 240, 30, 6, tft.color565(57, 217, 122));
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("> Welcome to SIM OS", 160, 160);

  // Підказка
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.drawString("[ tap to continue ]", 160, 200);
}
```

-----

## ПРАВКА 2 — Final Slide: додати новий state

У `main.cpp` додати новий стан між story і menu:

```cpp
enum AppState {
  STATE_BOOT,
  STATE_STORY,
  STATE_FINAL,   // <- новий
  STATE_MENU,
  STATE_PROFILE,
  STATE_ARCHIVE,
  STATE_ABOUT
};
```

У `loop()`:

```cpp
case STATE_STORY:
  storyDraw();
  if (touched) {
    bool done = storyNext();
    if (done) {
      finalDrawn = false;
      currentState = STATE_FINAL;  // -> Final, не одразу меню
    }
  }
  break;

case STATE_FINAL:
  if (!finalDrawn) {
    drawFinalSlide();
    finalDrawn = true;
  }
  if (touched) {
    menuInit();
    currentState = STATE_MENU;
  }
  break;
```

Додати глобальну змінну: `bool finalDrawn = false;`

-----

## ЕКРАН 1 — [MSG] From Family

Створити файл `screens/AboutScreen.h`:

```cpp
#pragma once
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern uint16_t C_BG, C_PANEL, C_NEON, C_NEON2, C_TEXT, C_DIM;

const char* MSG_LINES[] = {
  "Fedir,",
  "",
  "You came a long way.",
  "From your first LED blink",
  "to your own console.",
  "From your first line of code",
  "to Space Shooter game.",
  "",
  "We are proud of you",
  "every single day.",
  "",
  "Keep building.",
  "Keep dreaming.",
  "This is just the beginning",
  "of a great story.",
  "",
  "- Dad and Mom"
};
const int MSG_LINES_COUNT = 17;

bool aboutDrawn = false;

void drawAboutScreen() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // Вертикальна лінія зліва — акцент
  tft.fillRect(6, 8, 3, 220, tft.color565(57, 217, 122));

  // Заголовок
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
  tft.drawString("[ FROM FAMILY ]", 16, 8);

  tft.drawLine(0, 20, 320, 20, tft.color565(13, 30, 18));

  // Рядки тексту
  int y = 28;
  for (int i = 0; i < MSG_LINES_COUNT; i++) {
    if (strlen(MSG_LINES[i]) == 0) { y += 8; continue; }

    if (i == 0) {
      // "Fedir," — більший акцентний
      tft.setTextSize(2);
      tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
      tft.drawString(MSG_LINES[i], 16, y);
      y += 22;
    } else if (i == MSG_LINES_COUNT - 1) {
      // Підпис — акцентний колір
      tft.setTextSize(1);
      tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
      tft.drawString(MSG_LINES[i], 16, y);
      y += 14;
    } else {
      tft.setTextSize(1);
      tft.setTextColor(tft.color565(208, 240, 216), tft.color565(6, 10, 7));
      tft.drawString(MSG_LINES[i], 16, y);
      y += 14;
    }
  }

  // Кнопка назад
  tft.drawRoundRect(90, 218, 140, 18, 4, tft.color565(58, 106, 74));
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("< BACK", 160, 227);
}

void aboutInit() { aboutDrawn = false; }

void aboutHandleTouch(int tx, int ty) {
  // Кнопка назад: y 218-236, x 90-230
  if (ty >= 218 && ty <= 236 && tx >= 90 && tx <= 230) {
    // повернутись у меню — встановити у main.cpp
    extern AppState currentState;
    currentState = STATE_MENU;
    menuInit();
  }
}
```

-----

## ЕКРАН 2 — [USR] My Profile

Створити файл `screens/ProfileScreen.h`:

```cpp
#pragma once
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern uint16_t C_BG, C_PANEL, C_NEON, C_NEON2, C_TEXT, C_DIM;

const char* BADGES[] = {
  "IoT Pioneer",
  "Console Eng.",
  "Space Dev",
  "BT Expert",
  "Web Dev",
  "Kahoot Champ",
  "Self-Solderer",
};
const int BADGE_COUNT = 7;

bool profileDrawn = false;

void drawStatBox(int x, int y, int w, int h,
                 const char* label, const char* value) {
  tft.fillRoundRect(x, y, w, h, 4, tft.color565(13, 30, 18));
  tft.drawRoundRect(x, y, w, h, 4, tft.color565(58, 106, 74));

  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(13, 30, 18));
  tft.drawString(label, x + 5, y + 4);

  tft.setTextSize(2);
  tft.setTextColor(tft.color565(255, 225, 0), tft.color565(13, 30, 18));
  tft.drawString(value, x + 5, y + 16);
}

void drawProfileScreen() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // Хедер
  tft.fillRect(0, 0, 320, 22, tft.color565(13, 30, 18));
  tft.drawLine(0, 22, 320, 22, tft.color565(57, 217, 122));
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.drawString("[ MY PROFILE ]", 8, 7);

  // Аватар — коло
  tft.fillCircle(36, 54, 22, tft.color565(13, 30, 18));
  tft.drawCircle(36, 54, 22, tft.color565(57, 217, 122));
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.drawString("F", 36, 54);

  // Ім'я та рівень
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("Fedir", 68, 32);

  tft.setTextSize(1);
  tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
  tft.drawString("Level 9  Junior Developer", 68, 56);
  tft.drawString("robocode.ua  Intro Coding", 68, 68);

  // Розділювач
  tft.drawLine(0, 84, 320, 84, tft.color565(13, 30, 18));

  // Stat boxes — 2x2
  drawStatBox(8,   90, 72, 40, "Achievements", "21");
  drawStatBox(86,  90, 72, 40, "Technologies", "12+");
  drawStatBox(164, 90, 72, 40, "Projects", "8");
  drawStatBox(242, 90, 72, 40, "Kahoot Wins", ":)");

  // Бейджі
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.drawString("BADGES:", 8, 140);

  int bx = 8, by = 154;
  for (int i = 0; i < BADGE_COUNT; i++) {
    int bw = tft.textWidth(BADGES[i]) + 10;
    tft.drawRoundRect(bx, by, bw, 14, 3, tft.color565(57, 217, 122));
    tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
    tft.drawString(BADGES[i], bx + 5, by + 3);
    bx += bw + 5;
    if (bx > 260) { bx = 8; by += 18; }
  }

  // Кнопка назад
  tft.drawRoundRect(90, 218, 140, 18, 4, tft.color565(58, 106, 74));
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("< BACK", 160, 227);
}

void profileInit() { profileDrawn = false; }

void profileHandleTouch(int tx, int ty) {
  if (ty >= 218 && ty <= 236 && tx >= 90 && tx <= 230) {
    extern AppState currentState;
    currentState = STATE_MENU;
    menuInit();
  }
}
```

-----

## ЕКРАН 3 — [MAP] My Missions

Створити файл `screens/ArchiveScreen.h`:

```cpp
#pragma once
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern uint16_t C_BG, C_PANEL, C_NEON, C_NEON2, C_TEXT, C_DIM;

struct Mission {
  const char* icon;
  const char* name;
};

const Mission MISSIONS[] = {
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
const int MISSION_COUNT = 18;

bool archiveDrawn = false;
int archiveScroll = 0;  // для майбутнього скролу

void drawArchiveScreen() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // Хедер
  tft.fillRect(0, 0, 320, 22, tft.color565(13, 30, 18));
  tft.drawLine(0, 22, 320, 22, tft.color565(57, 217, 122));
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.drawString("[ MY MISSIONS ]", 8, 7);
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(13, 30, 18));
  tft.setTextDatum(TR_DATUM);
  tft.drawString("18 completed", 312, 7);

  // Список місій
  int y = 28;
  int visible = 13; // скільки влізає на екран
  for (int i = archiveScroll;
       i < MISSION_COUNT && i < archiveScroll + visible; i++) {
    // Чергування фону рядків
    uint16_t rowBg = (i % 2 == 0)
      ? tft.color565(6, 10, 7)
      : tft.color565(10, 18, 12);

    tft.fillRect(0, y, 320, 14, rowBg);

    // Галочка
    tft.setTextColor(tft.color565(57, 217, 122), rowBg);
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.drawString("+", 6, y + 3);

    // Номер
    tft.setTextColor(tft.color565(58, 106, 74), rowBg);
    tft.drawString(MISSIONS[i].icon, 18, y + 3);

    // Назва
    tft.setTextColor(tft.color565(208, 240, 216), rowBg);
    tft.drawString(MISSIONS[i].name, 46, y + 3);

    y += 14;
  }

  // Кнопка назад
  tft.fillRect(0, 218, 320, 22, tft.color565(6, 10, 7));
  tft.drawRoundRect(90, 218, 140, 18, 4, tft.color565(58, 106, 74));
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("< BACK", 160, 227);
}

void archiveInit() { archiveDrawn = false; archiveScroll = 0; }

void archiveHandleTouch(int tx, int ty) {
  if (ty >= 218 && ty <= 236 && tx >= 90 && tx <= 230) {
    extern AppState currentState;
    currentState = STATE_MENU;
    menuInit();
  }
}
```

-----

## ПІДКЛЮЧЕННЯ В main.cpp

Додати нові states:

```cpp
enum AppState {
  STATE_BOOT, STATE_STORY, STATE_FINAL,
  STATE_MENU,
  STATE_PROFILE,   // USR
  STATE_ARCHIVE,   // MAP
  STATE_ABOUT      // MSG
};
```

Додати includes:

```cpp
#include "screens/ProfileScreen.h"
#include "screens/ArchiveScreen.h"
#include "screens/AboutScreen.h"
```

У `menuHandleTouch()` додати переходи:

```cpp
void menuHandleTouch(int tx, int ty) {
  for (int i = 0; i < MENU_COUNT; i++) {
    int x = tileX(i);
    int y = tileY(i);
    if (tx >= x && tx <= x+140 && ty >= y && ty <= y+60) {
      drawMenuTile(i, true);
      delay(150);
      switch(i) {
        case 0: profileInit(); currentState = STATE_PROFILE; break;
        case 1: archiveInit(); currentState = STATE_ARCHIVE; break;
        case 6: aboutInit();   currentState = STATE_ABOUT;   break;
        default: drawMenuTile(i, false); break;
      }
      return;
    }
  }
}
```

У `loop()` додати нові case:

```cpp
case STATE_PROFILE:
  if (!profileDrawn) { drawProfileScreen(); profileDrawn = true; }
  if (touched) profileHandleTouch(tx, ty);
  break;

case STATE_ARCHIVE:
  if (!archiveDrawn) { drawArchiveScreen(); archiveDrawn = true; }
  if (touched) archiveHandleTouch(tx, ty);
  break;

case STATE_ABOUT:
  if (!aboutDrawn) { drawAboutScreen(); aboutDrawn = true; }
  if (touched) aboutHandleTouch(tx, ty);
  break;
```