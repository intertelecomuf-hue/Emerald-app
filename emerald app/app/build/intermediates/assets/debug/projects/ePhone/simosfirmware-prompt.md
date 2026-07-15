# ПРОМТ ДЛЯ AI АГЕНТА — Emerald Firmware

## ESP32 Yellow Display (CYD / ESP32-2432S028R)

-----

## РОЛЬ

Ти senior embedded C++ engineer.  
Твоє завдання — написати повну прошивку для ESP32 Yellow Display (Cheap Yellow Display, ESP32-2432S028R) під назвою **Emerald** — персональний гаджет юного програміста для 9-річного Федора.

-----

## ПЛАТФОРМА

- **Плата:** ESP32-2432S028R (Cheap Yellow Display / CYD)
- **Дисплей:** ST7789, 320×240 (ландшафт, rotation=1), HSPI (SPI2)
- **Touch:** XPT2046, SPI (спільна шина HSPI з дисплеєм, CS=21, IRQ=36)
- **Фреймворк:** Arduino
- **Build system:** PlatformIO
- **Бібліотеки:**
  - `bodmer/TFT_eSPI` — рендеринг дисплею (версія ^2.5.43)
  - `ESP32 Arduino WiFi` — вбудована
  - `ESP32 Arduino BLE` — вбудована

**ВАЖЛИВО:** Конфігурація TFT_eSPI виконується через файл [`src/tft_setup.h`](src/tft_setup.h), який автоматично підключається бібліотекою через `__has_include`. Всі піни, драйвер, частоту SPI та кольорові корекції задаються там.

> ⚠️ **Апаратні особливості саме цієї ревізії плати:**
> - Дисплей підключено до **HSPI** (SPI2: MOSI=13, MISO=12, SCLK=14), а не VSPI
> - Контролер дисплея — **ST7789**, а не ILI9341 (makefile визначає це через `ST7789_DRIVER`)
> - Потребує `TFT_RGB_ORDER TFT_BGR` — інакше канали R і B міняються місцями
> - Потребує `TFT_INVERSION_OFF` — інакше кольори інвертовані (чорний↔білий)
> - Touch (XPT2046) не може використовувати Z1/Z2 SPI команди (`0xB1`/`0xC1`) — вони повертають 0 на цій ревізії. Натомість — PENIRQ (GPIO36) для визначення натискання.

### platformio.ini

```ini
; Board: ESP32-2432S028 (Cheap Yellow Display)
; Project: Emerald — Personal gadget for young programmers
;
; Display: ST7789 320x240 (HSPI: MOSI=13, MISO=12, SCLK=14)
; Touch:   XPT2046 (CS=21, IRQ=36)
; RGB LED: R=17, G=26, B=16 (common anode)
;
; TFT_eSPI pins are configured in src/tft_setup.h
; (auto-included by TFT_eSPI.h via __has_include mechanism)

[env:esp32-2432S028]
platform = espressif32
board = esp32dev
framework = arduino
board_build.f_cpu = 240000000L
board_build.f_flash = 80000000L
board_build.flash_mode = qio
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

build_flags =
    -I src                              ; for tft_setup.h auto-detection

lib_deps =
    bodmer/TFT_eSPI @ ^2.5.43           ; display driver

    ; Touch (XPT2046) — реалізований вручну в TouchManager
    ; через спільну шину HSPI (tft.getSPIinstance()).
    ; Зовнішня бібліотека XPT2046_Touchscreen НЕ використовується.

upload_port = /dev/cu.usbserial-10
upload_speed = 115200
```

**Примітка:** Універсальний `[env:esp32dev]` працює, але `[env:esp32-2432S028]` дозволяє точніше контролювати flash_mode=qio та інші параметри.

-----

## КОЛЬОРОВА СХЕМА (обов'язково дотримуватись)

**ВАЖЛИВО:** Використовувати макрос `TFT_COLOR(r,g,b)` — він конвертує RGB888→RGB565 так само, як `tft.color565()`. Не використовувати хардкодні hex (`0x0821`, `0x1EEB` тощо), оскільки вони не відповідають реальним кольорам після `TFT_RGB_ORDER TFT_BGR`.

```cpp
// Конвертація RGB888 → RGB565 (еквівалент tft.color565(r,g,b))
#define TFT_COLOR(r,g,b) (((uint16_t)((r) & 0xF8) << 8) | ((uint16_t)((g) & 0xFC) << 3) | ((uint16_t)(b) >> 3))

// Основні кольори — зелена тема, dark cyberpunk
#define COLOR_BG        TFT_COLOR(6,10,7)         // дуже темний зелено-чорний фон
#define COLOR_PANEL     TFT_COLOR(0,138,16)       // темна панель
#define COLOR_NEON      TFT_COLOR(57,217,122)     // основний зелений неон  #39d97a
#define COLOR_NEON2     TFT_COLOR(168,224,99)     // світліший зелений акцент  #a8e063
#define COLOR_TEXT      TFT_COLOR(208,240,216)    // світло-зелений текст  #d0f0d8
#define COLOR_DIM       TFT_COLOR(58,106,74)      // приглушений зелений  #3a6a4a
#define COLOR_HIGHLIGHT TFT_COLOR(182,255,204)    // яскравий білувато-зелений для bold  #b6ffcc
#define COLOR_YELLOW    TFT_COLOR(255,225,0)      // акцент жовтий  #ffe100
#define COLOR_RED       TFT_COLOR(220,40,40)      // червоний для помилок
#define COLOR_BLACK     TFT_COLOR(0,0,0)
#define COLOR_WHITE     TFT_COLOR(255,255,255)
```

**Пояснення кольорів:**
- `COLOR_BG` (6,10,7) — майже чорний з легким зеленим відтінком
- `COLOR_NEON` (57,217,122) — головний акцентний зелений
- `COLOR_TEXT` (208,240,216) — основний текст, м'який світло-зелений
- `COLOR_DIM` (58,106,74) — другорядні елементи, рамки

-----

## АРХІТЕКТУРА — STATE MACHINE

```
AppState:
  STATE_BOOT
  STATE_STORY
  STATE_MENU
  STATE_PROFILE
  STATE_ARCHIVE
  STATE_SPY_TOOLS
  STATE_GAMES
  STATE_TOOLBOX
  STATE_CONSOLE
  STATE_ABOUT
```

Головний loop:

```cpp
void loop() {
  touch.getTouch();       // опитування PENIRQ + читання X/Y через SPI
  handleTouch();          // обробка tap подій (200ms debounce)
  switch(currentState) {
    case STATE_BOOT:    bootUpdate(); break;
    case STATE_STORY:   storyUpdate(); break;
    case STATE_MENU:    menuUpdate(); break;
    // ...
  }
}
```

Без `delay()` — тільки `millis()` для таймінгу.

-----

## ФАЙЛОВА СТРУКТУРА

```
/src
  main.cpp              — setup(), loop(), state machine
  config.h              — назва OS, ім'я юзера, кольори, піни, таймінги
  tft_setup.h           — конфігурація TFT_eSPI (драйвер, піни, SPI, кольорокорекція)

/src/screens
  BootScreen.h/.cpp     — boot sequence
  StoryScreen.h/.cpp    — achievement story mode
  MenuScreen.h/.cpp     — головне меню
  ProfileScreen.h/.cpp  — developer profile
  ArchiveScreen.h/.cpp  — mission archive / timeline
  SpyScreen.h/.cpp      — wifi scanner UI (реалізація сканування — TODO)
  GamesScreen.h/.cpp    — mini games menu
  ConsoleScreen.h/.cpp  — fake terminal
  AboutScreen.h/.cpp    — повідомлення від батьків

/src/ui
  UIComponents.h/.cpp   — кнопки, карточки, progress bar, status bar
  Animations.h/.cpp     — fade, slide, scanline, typewriter ефекти

  // 🚧 Fonts.h — НЕ створено (шрифти завантажуються через tft_setup.h:
  //               LOAD_GLCD, LOAD_FONT2/4/6/7/8, LOAD_GFXFF, SMOOTH_FONT)

/src/apps
  Snake.h/.cpp          — гра змійка (реалізовано)

  // 🚧 ReactionTest.h/.cpp   — НЕ створено
  // 🚧 Calculator.h/.cpp     — НЕ створено
  // 🚧 Stopwatch.h/.cpp      — НЕ створено

/src/services
  TouchManager.h/.cpp   — debounce, PENIRQ, калібрування координат, тапи
  // 🚧 WiFiScanner.h/.cpp    — НЕ створено
  // 🚧 BTScanner.h/.cpp      — НЕ створено
```

**Умовні позначення:**
- ✅ Реалізовано
- 🚧 TODO / не створено

-----

## config.h — КОНФІГУРАЦІЯ

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// OS Identity
// ============================================================
#define OS_NAME             "SIM OS"
#define OS_VERSION          "v1.0.0"
#define OS_EDITION          "Developer Edition"
#define USER_NAME           "Fedir"
#define USER_LEVEL          9
#define USER_TITLE          "Junior Developer"
#define ACHIEVEMENT_COUNT   21

// ============================================================
// CYD Pin Definitions (real board: ST7789 on HSPI)
// ============================================================
#define PIN_TFT_BL          32      // Backlight PWM
#define PIN_TOUCH_CS        21      // XPT2046 chip select
#define PIN_TOUCH_IRQ       36      // XPT2046 PENIRQ (active LOW)

// RGB LED pins (common anode: LOW = ON)
#define PIN_LED_R           17
#define PIN_LED_G           26
#define PIN_LED_B           16

// ============================================================
// Display dimensions (after rotation 1 = landscape)
// ============================================================
#define DISP_WIDTH           320
#define DISP_HEIGHT          240

// ============================================================
// Compile-time RGB888 → RGB565 converter
// ============================================================
#define TFT_COLOR(r,g,b) (((uint16_t)((r) & 0xF8) << 8) | \
                          ((uint16_t)((g) & 0xFC) << 3) | \
                          ((uint16_t)(b) >> 3))

// Color Scheme — dark green cyberpunk theme
// ============================================================
#define COLOR_BG        TFT_COLOR(6,10,7)         // very dark green-black
#define COLOR_PANEL     TFT_COLOR(0,138,16)       // dark panel
#define COLOR_NEON      TFT_COLOR(57,217,122)     // main green neon  #39d97a
#define COLOR_NEON2     TFT_COLOR(168,224,99)     // lighter green accent  #a8e063
#define COLOR_TEXT      TFT_COLOR(208,240,216)    // light green text  #d0f0d8
#define COLOR_DIM       TFT_COLOR(58,106,74)      // muted green  #3a6a4a
#define COLOR_HIGHLIGHT TFT_COLOR(182,255,204)    // bright white-green  #b6ffcc
#define COLOR_YELLOW    TFT_COLOR(255,225,0)      // accent yellow  #ffe100
#define COLOR_RED       TFT_COLOR(220,40,40)      // red for errors
#define COLOR_BLACK     TFT_COLOR(0,0,0)
#define COLOR_WHITE     TFT_COLOR(255,255,255)

// ============================================================
// Boot Timing (milliseconds)
// ============================================================
#define BOOT_LINE_DELAY     350
#define BOOT_TOTAL_TIME     4500
#define STORY_AUTO_NEXT     0      // 0 = only tap, >0 = auto advance ms

// ============================================================
// Touch Calibration (XPT2046 raw → screen coords)
// ============================================================
#define TOUCH_X_MIN         200
#define TOUCH_X_MAX         3700
#define TOUCH_Y_MIN         240
#define TOUCH_Y_MAX         3800
#define TOUCH_SWAP_XY       true
#define TOUCH_INVERT_X      false
#define TOUCH_INVERT_Y      true

// Touch filter
#define TOUCH_Z_MIN         200
#define TOUCH_Z_MAX         3500
#define TOUCH_DEBOUNCE_MS   200

// ============================================================
// UI Layout Constants
// ============================================================
#define STATUSBAR_HEIGHT    20
#define HEADER_HEIGHT       28
#define BACK_BUTTON_W       48
#define BACK_BUTTON_H       28
#define CARD_RADIUS          4

// ============================================================
// Animation Timing
// ============================================================
#define FADE_DURATION_MS    300
#define TYPEWRITER_CHAR_MS  30
#define SLIDE_DURATION_MS   400

#endif // CONFIG_H
```

-----

## SCREEN 1 — BOOT SEQUENCE

### Поведінка (без delay, через millis):

**Фаза 1 — Чорний екран** (300мс)

**Фаза 2 — Лого**

- По центру великий текст: `SIM OS`
- Шрифт: найбільший доступний, колір `COLOR_NEON`
- Glow ефект: намалювати текст 3 рази зі зміщенням +1px різними відтінками
- Під логом маленький текст: `v1.0.0 · Developer Edition`, колір `COLOR_DIM`

**Фаза 3 — Terminal рядки** (з'являються один за одним):

```
› Initializing SIM OS kernel…
› Loading hardware drivers…
› Mounting developer filesystem…
› Loading developer profile…        [Fedir]
› Restoring achievements…           [21 found]
› Calibrating sensors…
› Connecting to the future…
✓ SIM OS ready.                      ← COLOR_NEON2
```

Кожен рядок — typewriter або fade-in зліва.

**Фаза 4 — Progress bar**

- Внизу екрану, заповнюється паралельно з рядками
- Колір: градієнт `COLOR_NEON2` → `COLOR_NEON`
- Показувати відсоток справа

**Фаза 5 — Перехід**

- Fade out весь екран
- Перехід у `STATE_STORY`

-----

## SCREEN 2 — STORY MODE (Achievement Cards)

### Масив досягнень:

```cpp
struct Achievement {
  const char* icon;   // emoji або індекс bitmap
  const char* title;
  const char* desc;         // звичайний текст
  const char* descBold;     // 2-3 слова для виділення COLOR_HIGHLIGHT
};

Achievement achievements[] = {
  { "DICE",  "Random Inventor",
    "Я написав",           "генератор чисел",
    "і запустив власний",  "цифровий кубик"  },

  { "LED",   "LED Commander",
    "Я запрограмував",     "RGB LEDs",
    "і змусив їх",         "світити як хочу" },

  { "GEAR",  "Motor Mechanic",
    "Я підключив і керував","servo motors",
    "— мій перший крок у", "роботику"        },

  { "TEMP",  "Sensor Explorer",
    "Я зчитував",          "температуру і вологість",
    "з датчика",           "DHT11"           },

  { "WIFI",  "IoT Pioneer",
    "Я відправив дані",    "в інтернет",
    "через",               "ThingSpeak"      },

  { "PHONE", "App Inventor",
    "Я створив власний",   "мобільний додаток",
    "і запустив його",     "на телефоні"     },

  { "ROCKET","Space Shooter Dev",
    "Я розробив гру",      "Space Shooter",
    "— керування через",   "акселерометр"    },

  { "BT",    "Bluetooth Engineer",
    "Я налаштував",        "HC-05",
    "і побудував",         "бездротове керування" },

  { "CTRL",  "Console Engineer",
    "Я сам припаяв деталі,","зібрав корпус",
    "і запустив",          "власну приставку" },

  { "WEB",   "Web Developer",
    "Я написав",           "web app з нуля",
    "і запустив на",       "iPhone 4s"       },

  { "CUP",   "Kahoot Champion",
    "Я перемагав у",       "Kahoot",
    "— ноутбук весь",      "у стікерах"      },
};
```

### Відображення карточки:

```
┌─────────────────────────────────┐
│  ACHIEVEMENT UNLOCKED           │  ← COLOR_NEON2, маленький
│                                 │
│  🎲  (велика іконка/bitmap)     │
│                                 │
│  Random Inventor                │  ← COLOR_NEON, великий шрифт
│                                 │
│  Я написав ГЕНЕРАТОР ЧИСЕЛ      │  ← звичайний + bold COLOR_HIGHLIGHT
│  і запустив власний             │
│  ЦИФРОВИЙ КУБИК                 │
│                                 │
│  [ tap to continue ]            │  ← блимає, COLOR_DIM
│                                 │
│  ● ● ● ○ ○ ○ ○ ○ ○ ○ ○         │  ← прогрес крапки
└─────────────────────────────────┘
```

- Анімація появи: slide up + fade in
- Touch будь-де → наступна карточка
- Остання карточка → FINAL SLIDE

### Final Slide:

```
         Fedir.
  This is only the beginning.

     [ ► Welcome to SIM OS ]
```

- `Fedir.` — максимальний шрифт, COLOR_NEON, glow
- Tap → `STATE_MENU`

-----

## SCREEN 3 — ГОЛОВНЕ МЕНЮ

### Layout — сітка 2×4 (або 2×3 + 1):

```
┌──────────────────────────────┐
│ ⚡ SIM OS        FEDIR LVL9  │  ← header
├──────────┬───────────────────┤
│ 👤       │ 📜               │
│ Developer│ Mission           │
│ Profile  │ Archive           │
├──────────┼───────────────────┤
│ 🛰️  LIVE │ 🎮               │
│ Spy Tools│ Mini Games        │
├──────────┼───────────────────┤
│ 🔧       │ 💻               │
│ Toolbox  │ System Console    │
├──────────┴───────────────────┤
│ 💌  About Device             │
├──────────────────────────────┤
│ ● ONLINE │ SIM OS │ 09:41   │  ← status bar
└──────────────────────────────┘
```

Кожен тайл:

- border: `COLOR_DIM`
- при натисканні: highlight border `COLOR_NEON` + інвертований фон на 150мс
- іконка (bitmap 24×24 або великий символ)
- назва: `COLOR_NEON`
- підпис: `COLOR_DIM`

-----

## SCREEN — DEVELOPER PROFILE

```
┌──────────────────────────────┐
│ ← BACK    DEVELOPER PROFILE  │
├──────────────────────────────┤
│  [🧑‍💻]  Fedir                 │
│         Level 9              │
│         Junior Developer     │
├──────────┬───────────────────┤
│ Achievements │ Technologies  │
│     21       │     12+       │
├──────────┼───────────────────┤
│ Projects │ Kahoot Wins       │
│    8     │     🏆 ∞          │
├──────────────────────────────┤
│ BADGES:                      │
│ [IoT Pioneer] [Console Eng.] │
│ [Space Dev]   [BT Expert]    │
│ [Web Dev]     [Kahoot Champ] │
└──────────────────────────────┘
```

-----

## SCREEN — SPY TOOLS (WiFi Scanner)

```cpp
// WiFiScanner.cpp — 🚧 TODO: реалізувати сканування
void scanWiFi() {
  int n = WiFi.scanNetworks();
  // зберегти результати в масив структур:
  struct NetInfo { String ssid; int rssi; int enc; };
}
```

UI:

```
┌──────────────────────────────┐
│ ← BACK    SPY TOOLS          │
│           ● SCANNING...      │  ← блимає
├──────────────────────────────┤
│ ▓▓▓▓ HOME_NETWORK_5G  WPA2  │
│ ▓▓▓▓ RoboCode_WiFi    WPA3  │
│ ▓▓▓░ Neighbor_2.4G    WPA2  │
│ ▓▓░░ AndroidAP_F3A2   WPA2  │
│ ▓░░░ [Hidden Network] ???   │
├──────────────────────────────┤
│ BT: HC-05 · MacBook · iPhone │
└──────────────────────────────┘
```

Signal bars малювати як прямокутники різної висоти.

-----

## SCREEN — SYSTEM CONSOLE (Fake Terminal)

```cpp
// Підтримувані команди:
const char* commands[] = {
  "help", "whoami", "date", "status",
  "scan", "sensors", "about", "clear", "fedir"
};

// Відповіді:
"whoami"  → "fedir@emerald — Junior Developer · Level 9"
"status"  → "● System: ONLINE\n● CPU: 240MHz\n● Achievements: 21/21"
"scan"    → запустити WiFi scan + вивести результати
"sensors" → показати fake sensor дані
"fedir"   → "⚡ The future belongs to you, Fedir."
"help"    → список команд
"clear"   → очистити екран
```

Touch клавіатура: 26 букв + цифри + ENTER + DEL  
Або спрощена — тільки цифри та кілька команд як кнопки.

-----

## SCREEN — ABOUT DEVICE

Повноекранний текст, плавна поява рядок за рядком (typewriter):

```
Федоре,

Ти пройшов великий шлях.
Від першої LED лампочки —
до власної консолі.
Від першого рядка коду —
до Space Shooter гри.

Ми пишаємося тобою
кожен день.

Продовжуй створювати.
Продовжуй мріяти.
Це лише початок
великої історії. ⚡

— Тато і Мама 💙
```

Колір тексту: `COLOR_TEXT`, акцент рядки: `COLOR_NEON2`

-----

## UI КОМПОНЕНТИ (UIComponents.h)

```cpp
// Кнопка назад
void drawBackButton();
bool backButtonPressed(int x, int y);

// Карточка
void drawCard(int x, int y, int w, int h, uint16_t borderColor);

// Progress bar
void drawProgressBar(int x, int y, int w, int h, int percent, uint16_t color);

// Status bar (низ екрану, висота 20px)
void drawStatusBar(const char* status, const char* time);

// Dot indicators (для story mode)
void drawDots(int total, int current, int y);

// Signal bars (для wifi)
void drawSignalBars(int x, int y, int strength); // strength 1-4

// Typewriter effect
void typewriterPrint(int x, int y, const char* text, uint16_t color, int delayMs);
```

-----

## АНІМАЦІЇ (Animations.h)

```cpp
// Fade in/out (через яскравість PWM на BL піні або overlay)
void fadeIn(int durationMs);
void fadeOut(int durationMs);

// Slide transition
void slideInFromBottom(int durationMs);

// Scanline overlay (малювати горизонтальні лінії з opacity)
void drawScanlines();

// Glow text (малювати текст кілька разів зі зміщенням)
void drawGlowText(const char* text, int x, int y,
                  uint8_t fontSize, uint16_t color);

// Blinking cursor
void drawBlinker(int x, int y, bool visible);

// Boot progress
void animateBootLine(int lineIndex, const char* text, int x, int y);
```

-----

## TOUCH MANAGER (TouchManager.h)

**ВАЖЛИВО:** На цій платі XPT2046 не підтримує Z1/Z2 вимірювання тиску через SPI команди `0xB1`/`0xC1` — вони завжди повертають 0. Тому використовується **апаратний PENIRQ** (GPIO36, active LOW).

Алгоритм роботи:

1. Опитування `digitalRead(PIN_TOUCH_IRQ) == LOW` — ознака натискання
2. Якщо PENIRQ активний — читання X/Y через SPI на спільній шині HSPI:
   - SPI транзакція через `tft.getSPIinstance()` (той самий SPI, що й дисплей)
   - Команди: `0x91` (X), `0xD1` (Y) — 12 біт, результати >> 3
   - 3 альтернуючі виміри X/Y → best-two-average для X окремо, Y окремо
3. Калібрування: swapXY (якщо потрібно) → map(200..3700 → 0..320) → invertY
4. Tap detection: state machine press→release з 200ms debounce

```cpp
// TouchManager.h — ключові методи
void begin(SPIClass& spiBus);           // передати tft.getSPIinstance()
void setCalibration(/* params */);
bool getTouch();                        // опитування PENIRQ + читання X/Y
TouchEvent getTap();                    // debounced press-release подія

struct TouchEvent {
  bool pressed;     // true коли tap завершено (release)
  int x, y;         // калібровані координати (320×240)
};
```

Калібрування для CYD (типові значення):

```cpp
#define TOUCH_X_MIN  200
#define TOUCH_X_MAX  3700
#define TOUCH_Y_MIN  240
#define TOUCH_Y_MAX  3800
#define TOUCH_SWAP_XY true
#define TOUCH_INVERT_X false
#define TOUCH_INVERT_Y true
```

⚠️ **Не використовувати Z-поріг для визначення натискання** — PENIRQ достатньо.

-----

## MINI GAMES — Snake (базова версія)

```
Поле: 20×15 клітинок по 14px
Керування: 4 кнопки-стрілки на екрані (touch)
Або: свайп по екрану
Швидкість: починає з 200мс/крок, +10мс кожні 5 очок
Score відображати вгорі
Game Over → пауза 1.5с → показати score → повернутись у меню ігор
```

-----

## ТЕХНІЧНІ ВИМОГИ

1. **Ніяких `delay()`** — тільки `millis()` і state machine
1. **Double buffering** якщо можливо, або малювати тільки змінені частини
1. **Touch debounce** — мінімум 200мс між подіями
1. **Memory** — не алокувати великі буфери в heap, використовувати static
1. **FPS** — цільовий 15-30fps для анімацій
1. **Модульність** — кожен screen в окремому файлі, main.cpp тільки координує

### Важливі вимоги до дисплея:

1. **ST7789_DRIVER** — обов'язково, не ILI9341
1. **TFT_RGB_ORDER TFT_BGR** — обов'язково, інакше червоний↔синій
1. **TFT_INVERSION_OFF** — обов'язково, інакше інверсія кольорів
1. **USE_HSPI_PORT** — дисплей на HSPI (SPI2), не VSPI
1. **TFT_ROTATION 1** — ландшафт, USB зліва
1. **SPI_FREQUENCY 4000000** — знижена частота для стабільності
1. **TOUCH_CS 21** — CS для XPT2046

-----

## ПОРЯДОК РЕАЛІЗАЦІЇ (для агента)

1. `config.h` — всі константи ✅
1. `tft_setup.h` — конфігурація дисплея ✅
1. `TouchManager.h/.cpp` — PENIRQ-based touch ✅
1. `UIComponents.h/.cpp` — базові примітиви ✅
1. `Animations.h/.cpp` — fade, typewriter ✅
1. `BootScreen.h/.cpp` — boot sequence ✅
1. `StoryScreen.h/.cpp` — achievement cards ✅
1. `MenuScreen.h/.cpp` — головне меню ✅
1. `main.cpp` — state machine, setup/loop ✅
1. Решта screens по черзі ✅

-----

## ДОДАТКОВО ДЛЯ АГЕНТА

- Якщо emoji не підтримуються — замінити на прості bitmap іконки 24×24 (намалювати через TFT примітиви)
- Шрифти: використовувати вбудовані TFT_eSPI (LOAD_GLCD, LOAD_FONT2/4/6/7/8, LOAD_GFXFF, SMOOTH_FONT)
- Для великого тексту заголовків (SIM OS на boot) — шрифт 4 або 6 з TFT_eSPI
- WiFi scan може займати 2-3 секунди — показувати animated spinner під час сканування
- BLE scan — використовувати `BLEScan` з esp32 arduino core (🚧 TODO)
- RGB LED (common anode, LOW=ON): R=17, G=26, B=16 — для індикації станів
- Backlight — PWM на GPIO32 (канал 0, частота 5000Гц, роздільність 8 біт) для fade ефектів
- `touch.getTouch()` викликається **кожен loop** перед `handleTouch()` — це оновлює координати та стан PENIRQ

-----

## ФІНАЛЬНА МЕТА

Коли Федір вмикає пристрій він має відчути:

> *"Це МІЙ комп'ютер. Я справжній програміст."*

Код має бути **чистий, з коментарями**, легко розширюваний.  
Кожен файл — не більше 200-300 рядків.  
Назву OS і ім'я юзера — міняти тільки через `config.h`.
