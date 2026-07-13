# SIM OS — Фікс кирилиці, тексту карточок, прогрес бару

## Промт для агента — Emerald

-----

## ГОЛОВНА ПРОБЛЕМА — Кирилиця не відображається

TFT_eSPI не підтримує UTF-8 кирилицю напряму.
Символи типу `і`, `є`, `ї`, `а`, `н` показуються як `i`, `ii`, `...`.

### Рішення — використовувати тільки латиницю і транслітерацію

**НЕ намагатись виводити кирилицю через `drawString`.**
Замінити всі українські тексти на латиницю або англійську.
Це єдиний надійний спосіб без підключення кастомних шрифтів.

-----

## ПРАВКА 1 — Boot рядки: замінити на латиницю

Знайди масив `BOOT_LINES[]`. Замінити на:

```cpp
const char* BOOT_LINES[] = {
  "> Zapusk yadra SIM OS...",
  "> Zavantazhennya drayveriv...",
  "> Montazh faylovoyi systemy...",
  "> Zavantazhennya profilyu...   [Fedir]",
  "> Vidnovlennya dosyagnen...    [21 found]",
  "> Kalibrovka sensoriv...",
  "> Pidklyuchennya do maybutnogo...",
  "OK  SIM OS gotovyy."
};
const int BOOT_LINE_COUNT = 8;
```

-----

## ПРАВКА 2 — Прогрес бар: виправити таймінг

Знайди де оновлюється прогрес бар у `bootUpdate()`.
Проблема — `bootLineIndex` збільшується після малювання прогресу.

Переконайся що порядок такий:

```cpp
void bootUpdate() {
  if (bootDone) return;
  unsigned long now = millis();
  if (now - bootLastLine < (unsigned long)BOOT_LINE_DELAY) return;
  bootLastLine = now;

  if (bootLineIndex >= BOOT_LINE_COUNT) {
    bootDone = true;
    return;
  }

  // 1. Спочатку малюємо рядок
  int y = 30 + bootLineIndex * 22;
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  if (bootLineIndex == BOOT_LINE_COUNT - 1) {
    tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
  } else {
    tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  }
  tft.drawString(BOOT_LINES[bootLineIndex], 10, y);

  // 2. Збільшуємо індекс
  bootLineIndex++;

  // 3. Тепер рахуємо прогрес від нового індексу
  int progress = bootLineIndex * 200 / BOOT_LINE_COUNT;
  tft.fillRect(10, 210, progress, 8, tft.color565(57, 217, 122));

  int pct = bootLineIndex * 100 / BOOT_LINE_COUNT;
  tft.fillRect(215, 205, 45, 12, tft.color565(6, 10, 7));
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.setTextDatum(TR_DATUM);
  char buf[8];
  sprintf(buf, "%d%%", pct);
  tft.drawString(buf, 318, 206);
}
```

-----

## ПРАВКА 3 — Карточки: виправити відображення всіх рядків тексту

Проблема — у `drawAchievementCard()` малюється тільки `bold1` (виділене слово),
але `line1` і `line2` не відображаються або зливаються.

Замінити блок малювання тексту карточки повністю:

```cpp
void drawAchievementCard(int idx) {
  const Achievement& a = ACHIEVEMENTS[idx];

  tft.fillScreen(tft.color565(6, 10, 7));

  // Заголовок зверху
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
  tft.drawString("~ ACHIEVEMENT UNLOCKED ~", 160, 10);

  // Рамка карточки
  tft.fillRoundRect(10, 28, 300, 170, 8, tft.color565(13, 30, 18));
  tft.drawRoundRect(10, 28, 300, 170, 8, tft.color565(57, 217, 122));

  // Іконка
  tft.setTextColor(tft.color565(168, 224, 99), tft.color565(13, 30, 18));
  tft.setTextSize(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(a.icon, 160, 40);

  // Заголовок досягнення
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.setTextSize(2);
  tft.drawString(a.title, 160, 80);

  // Рядок 1: line1 + bold1 на одному рядку
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);

  // Спочатку line1
  tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));
  tft.drawString(a.line1, 20, 130);

  // Потім bold1 одразу після — зміщення по X
  int x1 = 20 + tft.textWidth(a.line1) + 4;
  tft.setTextColor(tft.color565(182, 255, 204), tft.color565(13, 30, 18));
  tft.drawString(a.bold1, x1, 130);

  // Рядок 2: line2 на наступному рядку
  tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));
  tft.drawString(a.line2, 20, 150);

  // Tap підказка
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(BC_DATUM);
  tft.drawString("[ tap to continue ]", 160, 215);

  // Крапки прогресу
  drawStoryDots(idx, ACH_COUNT);
}
```

-----

## ПРАВКА 4 — Масив досягнень: повністю латиницею

Замінити масив `ACHIEVEMENTS[]` повністю:

```cpp
const Achievement ACHIEVEMENTS[] = {
  { "[DICE]",  "Random Inventor",
    "I wrote a",         "number generator",
    "and launched my digital dice."          },

  { "[LED]",   "LED Commander",
    "I programmed",      "RGB LEDs",
    "and made them shine my way."            },

  { "[GEAR]",  "Motor Mechanic",
    "I connected",       "servo motors",
    "- my first step into robotics."         },

  { "[TEMP]",  "Sensor Explorer",
    "I measured",        "temperature",
    "and humidity with DHT11."               },

  { "[WIFI]",  "IoT Pioneer",
    "I sent data",       "to the internet",
    "via ThingSpeak."                        },

  { "[PHONE]", "App Inventor",
    "I built my own",    "mobile app",
    "and launched it on a phone."            },

  { "[GAME]",  "Space Shooter Dev",
    "I developed",       "Space Shooter",
    "with accelerometer control."            },

  { "[BT]",    "Bluetooth Engineer",
    "I set up",          "HC-05",
    "and built wireless control."            },

  { "[CTRL]",  "Console Engineer",
    "I soldered",        "my own console",
    "on Raspberry Pi Pico."                  },

  { "[WEB]",   "Web Developer",
    "I built a",         "web app",
    "and ran it on iPhone 4s."               },

  { "[CUP]",   "Kahoot Champion",
    "I won at",          "Kahoot",
    "my laptop is full of stickers!"         },
};
```

-----

## ПРАВКА 5 — Меню тайли: латиниця

Знайди масив `MENU_ITEMS[]`. Замінити назви:

```cpp
const MenuItem MENU_ITEMS[] = {
  { "[USR]", "My Profile",    "Level, achievements",  false, ""     },
  { "[MAP]", "My Missions",   "Full learning timeline", false, ""   },
  { "[SPY]", "Spy Tools",     "WiFi + Bluetooth",     true,  "LIVE" },
  { "[GAM]", "Mini Games",    "Snake, Space Shooter", false, ""     },
  { "[BOX]", "Toolbox",       "Calculator, stopwatch", false, ""    },
  { "[MSG]", "From Family",   "Message from parents", false, ""     },
};
```

-----

## ПРАВКА 6 — Final Slide: латиниця

```cpp
void drawFinalSlide() {
  tft.fillScreen(tft.color565(6, 10, 7));

  tft.setTextDatum(MC_DATUM);

  // Ім'я велике
  tft.setTextSize(4);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("Fedir.", 160, 80);

  // Підзаголовок
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(208, 240, 216), tft.color565(6, 10, 7));
  tft.drawString("This is only the beginning.", 160, 125);

  // Кнопка
  tft.drawRoundRect(30, 155, 260, 30, 6, tft.color565(57, 217, 122));
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("> Welcome to SIM OS", 160, 170);

  // Підказка
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.drawString("[ tap to continue ]", 160, 210);
}
```

-----

## ПРАВКА 7 — About екран: латиниця

```cpp
const char* ABOUT_LINES[] = {
  "Fedir,",
  "",
  "You came a long way.",
  "From your first LED blink",
  "to your own console.",
  "From your first line of code",
  "to Space Shooter.",
  "",
  "We are proud of you",
  "every single day.",
  "",
  "Keep building.",
  "This is just the beginning",
  "of a great story.",
  "",
  "- Dad and Mom",
};
const int ABOUT_LINES_COUNT = 16;
```

-----

## ПІДСУМОК

Кирилиця в TFT_eSPI без кастомного шрифту — не працює.
Всі тексти мають бути латиницею або англійською.

Якщо в майбутньому потрібна кирилиця — окреме завдання:
підключити шрифт з підтримкою CP1251 через SPIFFS.
Але це окремий крок, не зараз.