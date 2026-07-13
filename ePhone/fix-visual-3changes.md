# Промт для агента — 3 правки візуалу

-----

## ПРАВКА 1 — Boot лого Emerald не видно

Знайди де малюється лого `Emerald` на boot екрані.
Проблема — текст не видно бо колір зливається з фоном.

Заміни на:

```cpp
// Спочатку залити весь екран темним
tft.fillScreen(tft.color565(6, 10, 7));

// Лого SIM OS — великий зелений текст по центру
tft.setTextDatum(MC_DATUM);
tft.setTextSize(4);
tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
tft.drawString("SIM OS", 160, 80);

// Підзаголовок менший
tft.setTextSize(1);
tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
tft.drawString("v1.0 - Developer Edition", 160, 112);
```

-----

## ПРАВКА 2 — Карточка досягнення: прибрати яскраво-зелений фон

Знайди де малюється прямокутник карточки у StoryScreen.
Зараз він заливається яскраво-зеленим — це неправильно.

Заміни:

```cpp
// Фон карточки — темний панельний
tft.fillRoundRect(10, 35, 220, 230, 8, tft.color565(13, 30, 18));

// Рамка — зелений неон
tft.drawRoundRect(10, 35, 220, 230, 8, tft.color565(57, 217, 122));
```

ВАЖЛИВО: весь текст всередині карточки малювати з bg кольором
`tft.color565(13, 30, 18)` — не зеленим, не чорним, саме цим.

Тобто кожен `drawString` всередині карточки має виглядати так:

```cpp
tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));
```

-----

## ПРАВКА 3 — Тексти карточок: українська від першої особи

Знайди масив `ACHIEVEMENTS[]` у StoryScreen.
Заміни усі записи повністю на ці:

```cpp
{ "[DICE]",  "Random Inventor",
  "Я написав",          "генератор чисел",
  "i запустив цифровий кубик."                },

{ "[LED]",   "LED Commander",
  "Я запрограмував",    "RGB LEDs",
  "i змусив їх світити як хочу."              },

{ "[GEAR]",  "Motor Mechanic",
  "Я підключив",        "servo motors",
  "— мій перший крок у роботику."             },

{ "[WIFI]",  "IoT Pioneer",
  "Я відправив дані",   "в інтернет",
  "через ThingSpeak."                         },

{ "[PHONE]", "App Inventor",
  "Я створив власний",  "мобільний додаток",
  "i запустив на телефоні."                   },

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
  "— ноутбук весь у стікерах!"               },
```

### Кольори тексту всередині карточки:

```cpp
// Звичайний текст (line1, line2)
tft.setTextColor(tft.color565(208, 240, 216), tft.color565(13, 30, 18));

// Виділене слово bold1 — яскравіший колір
tft.setTextColor(tft.color565(182, 255, 204), tft.color565(13, 30, 18));

// Заголовок досягнення (title)
tft.setTextColor(tft.color565(57, 217, 122), tft.color565(13, 30, 18));

// "ACHIEVEMENT UNLOCKED" зверху
tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
```