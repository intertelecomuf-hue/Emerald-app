# SIM OS — 9 візуальних правок

## Промт для агента — Emerald

-----

## ПРАВКА 1 — Boot: додати лого SIM OS перед рядками

Знайди функцію `bootInit()`.
На самому початку після `fillScreen` додай лого:

```cpp
void bootInit() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // Лого SIM OS — великий зелений текст
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(4);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("SIM OS", 160, 70);

  // Підзаголовок
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("v1.0 - Developer Edition", 160, 100);

  // Пауза щоб лого побачили
  delay(1500);

  // Очистити і починати рядки логу
  tft.fillScreen(tft.color565(6, 10, 7));

  // ... далі існуючий код рядків
}
```

-----

## ПРАВКА 2 — Boot: колір тексту рядків зелений

Знайди де малюються рядки boot логу.
Замінити колір тексту:

```cpp
// Звичайні рядки
tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));

// Останній рядок "OK SIM OS ready"
tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
```

Фон другий параметр завжди `tft.color565(6, 10, 7)` — не чорний, не білий.

-----

## ПРАВКА 3 — Boot: прогрес бар заповнюється разом з рядками

Знайди де оновлюється прогрес бар.
Прогрес має рахуватись від поточного рядка:

```cpp
// Після кожного нового рядка:
int progress = (bootLineIndex + 1) * 200 / BOOT_LINE_COUNT;
tft.fillRect(21, 291, progress, 10, tft.color565(57, 217, 122));

// Відсоток
int pct = (bootLineIndex + 1) * 100 / BOOT_LINE_COUNT;
char buf[8];
sprintf(buf, "%d%%", pct);
tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
tft.drawString(buf, 220, 277);
```

Перевір що `bootLineIndex` збільшується ДО малювання прогресу, не після.

-----

## ПРАВКА 4 — Карточки: прибрати яскраво-зелений фон

Знайди де малюється прямокутник карточки у StoryScreen.
Замінити повністю:

```cpp
// Фон всього екрану
tft.fillScreen(tft.color565(6, 10, 7));

// Фон карточки — темний
tft.fillRoundRect(10, 35, 220, 230, 8, tft.color565(13, 30, 18));

// Рамка — зелений неон
tft.drawRoundRect(10, 35, 220, 230, 8, tft.color565(57, 217, 122));
```

Весь текст всередині карточки — bg колір `tft.color565(13, 30, 18)`:

```cpp
tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));
```

-----

## ПРАВКА 5 — Карточки: українські тексти від першої особи

Знайди масив `ACHIEVEMENTS[]`. Замінити повністю:

```cpp
{ "[DICE]",  "Random Inventor",
  "Я написав",          "генератор чисел",
  "i запустив цифровий кубик."                },

{ "[LED]",   "LED Commander",
  "Я запрограмував",    "RGB LEDs",
  "i змусив їх світити як хочу."              },

{ "[GEAR]",  "Motor Mechanic",
  "Я підключив",        "servo motors",
  "- мiй перший крок у роботику."             },

{ "[WIFI]",  "IoT Pioneer",
  "Я вiдправив данi",   "в iнтернет",
  "через ThingSpeak."                         },

{ "[PHONE]", "App Inventor",
  "Я створив власний",  "мобiльний додаток",
  "i запустив на телефонi."                   },

{ "[GAME]",  "Space Shooter Dev",
  "Я розробив гру",     "Space Shooter",
  "з керуванням через акселерометр."          },

{ "[BT]",    "Bluetooth Engineer",
  "Я налаштував",       "HC-05",
  "i зробив бездротове керування."            },

{ "[CTRL]",  "Console Engineer",
  "Я сам припаяв",      "власну приставку",
  "на Raspberry Pi Pico."                     },

{ "[WEB]",   "Web Developer",
  "Я написав",          "web app з нуля",
  "i запустив на iPhone 4s."                  },

{ "[CUP]",   "Kahoot Champion",
  "Я перемагав у",      "Kahoot",
  "- ноутбук весь у стiкерах!"               },
```

Виділене слово (друге поле) малювати яскравішим кольором:

```cpp
// bold слово
tft.setTextColor(tft.color565(182, 255, 204), tft.color565(13, 30, 18));

// звичайний текст
tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));
```

-----

## ПРАВКА 6 — Карточки: орієнтація portrait

Перевір у `setup()` або `bootInit()`:

```cpp
tft.setRotation(0);  // portrait — 240x320
```

Якщо стоїть `setRotation(1)` або `setRotation(3)` — це landscape.
Замінити на `0`.

Якщо після цього текст іде вертикально — спробувати `setRotation(2)`.

-----

## ПРАВКА 7 — Меню: іконка і назва не накладаються

Знайди функцію `drawMenuTile()`.
Проблема — іконка `[DEV]` і назва `Developer` малюються на одній позиції.
Розвести по вертикалі:

```cpp
void drawMenuTile(int idx, bool highlighted) {
  int x = tileX(idx);
  int y = tileY(idx);

  uint16_t bgCol = highlighted ? tft.color565(13, 30, 18) : tft.color565(6, 10, 7);
  uint16_t borderCol = highlighted ? tft.color565(57, 217, 122) : tft.color565(58, 106, 74);

  tft.fillRoundRect(x, y, 110, 88, 5, bgCol);
  tft.drawRoundRect(x, y, 110, 88, 5, borderCol);

  // Іконка — верхній лівий кут
  tft.setTextColor(tft.color565(168, 224, 99), bgCol);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(MENU_ITEMS[idx].icon, x + 6, y + 8);

  // Назва — середина тайлу
  tft.setTextColor(tft.color565(57, 217, 122), bgCol);
  tft.setTextSize(1);
  tft.drawString(MENU_ITEMS[idx].name, x + 6, y + 38);

  // Підпис — нижня частина
  tft.setTextColor(tft.color565(58, 106, 74), bgCol);
  tft.drawString(MENU_ITEMS[idx].desc, x + 6, y + 56);
}
```

-----

## ПРАВКА 8 — Меню: хедер темний з рамкою

Знайди `drawMenuHeader()`.
Замінити яскравий зелений фон на темний:

```cpp
void drawMenuHeader() {
  // Темний фон хедера
  tft.fillRect(0, 0, 320, 30, tft.color565(13, 30, 18));
  // Нижня лінія хедера
  tft.drawLine(0, 30, 320, 30, tft.color565(57, 217, 122));

  // Лого зліва
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.drawString("SIM OS", 8, 6);

  // Юзер справа
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(13, 30, 18));
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.drawString("Fedir LVL9", 312, 10);
}
```

-----

## ПРАВКА 9 — Меню: статус бар темний

Знайди `drawStatusBar()`.
Замінити яскравий фон на темний:

```cpp
void drawStatusBar() {
  // Темний фон
  tft.fillRect(0, 210, 320, 14, tft.color565(13, 30, 18));
  // Верхня лінія
  tft.drawLine(0, 210, 320, 210, tft.color565(57, 217, 122));

  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);

  // ONLINE зліва зеленим
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.drawString("* ONLINE", 6, 212);

  // SIM OS по центру
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(13, 30, 18));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("SIM OS", 160, 212);

  // Час справа
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));
  tft.setTextDatum(TR_DATUM);
  tft.drawString("09:41", 314, 212);
}
```

-----

## ПІДСУМОК КОЛЬОРІВ — використовувати скрізь

```
Фон екрану:      tft.color565(6,   10,  7)
Фон панелі:      tft.color565(13,  30,  18)
Зелений неон:    tft.color565(57,  217, 122)
Акцент світлий:  tft.color565(168, 224, 99)
Текст звичайний: tft.color565(208, 240, 216)
Приглушений:     tft.color565(58,  106, 74)
Bold виділення:  tft.color565(182, 255, 204)
```

Яскраво-зелений `color565(0, 255, 0)` або подібний — ніде не використовувати.