# SIM OS — Final Slide + Українські тексти

## Промт для агента — Emerald

-----

## ЗАВДАННЯ 1 — Final Slide між карточками і меню

Після останньої карточки досягнень перед переходом у меню показати Final Slide.

### Де додати:

Знайди функцію `storyNext()`. Коли `storyIndex >= ACH_COUNT` — замість одразу переходити в меню, встановити `storyFinal = true` і показати фінальний екран.

```cpp
bool storyNext() {
  storyIndex++;
  if (storyIndex >= ACH_COUNT) {
    storyFinal = true;
    storyDrawn = false;
    return false;  // ще не переходимо в меню
  }
  storyDrawn = false;
  return false;
}
```

Якщо `storyFinal == true` і юзер тапнув — тоді `return true` (перехід у меню).

### Як малювати Final Slide:

```cpp
void drawFinalSlide() {
  // Чорний фон
  tft.fillScreen(tft.color565(6, 10, 7));

  // Ім'я — максимально великий шрифт по центру
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(4);
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("Fedir.", 160, 90);

  // Підзаголовок
  tft.setTextSize(1);
  tft.setTextColor(tft.color565(208, 240, 216), tft.color565(6, 10, 7));
  tft.drawString("This is only the beginning.", 160, 135);

  // Кнопка переходу — рамка
  tft.drawRoundRect(50, 165, 220, 36, 6, tft.color565(57, 217, 122));
  tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
  tft.drawString("> Welcome to SIM OS", 160, 183);

  // Підказка внизу
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.drawString("[ tap to continue ]", 160, 220);
}
```

### Логіка показу:

```cpp
// У storyDraw():
void storyDraw() {
  if (storyDrawn) return;
  storyDrawn = true;
  if (storyFinal) {
    drawFinalSlide();
  } else {
    drawAchievementCard(storyIndex);
  }
}

// У loop() STATE_STORY:
case STATE_STORY:
  storyDraw();
  if (touched) {
    bool done = storyNext();
    if (done) {
      menuInit();
      currentState = STATE_MENU;
    }
  }
  break;
```

-----

## ЗАВДАННЯ 2 — Українські тексти на всіх екранах

### Boot екран — рядки логу українською:

Знайди масив `BOOT_LINES[]`. Замінити на:

```cpp
const char* BOOT_LINES[] = {
  "> Запуск ядра SIM OS...",
  "> Завантаження драйверів...",
  "> Монтування файлової системи...",
  "> Завантаження профiлю...     [Fedir]",
  "> Вiдновлення досягнень...    [21 знайдено]",
  "> Калiбрування сенсорiв...",
  "> Пiдключення до майбутнього...",
  "OK  SIM OS готовий."
};
```

-----

### Final Slide — текст:

```cpp
// Рядок 1 — велике ім'я
tft.drawString("Fedir.", 160, 90);

// Рядок 2
tft.drawString("Це лише початок.", 160, 135);

// Кнопка
tft.drawString("> Ласкаво просимо до SIM OS", 160, 183);

// Підказка
tft.drawString("[ торкнись щоб продовжити ]", 160, 220);
```

-----

### Меню — хедер і тайли українською:

Знайди масив `MENU_ITEMS[]`. Замінити назви і описи:

```cpp
const MenuItem MENU_ITEMS[] = {
  { "[USR]", "Мiй Профiль",      "Рiвень, досягнення, бейджi",  false, ""     },
  { "[MAP]", "Мої Мiсiї",        "Повна timeline навчання",      false, ""     },
  { "[SPY]", "Шпигунськi Iнстр", "WiFi + Bluetooth сканер",      true,  "LIVE" },
  { "[GAM]", "Мiнi Iгри",        "Змiйка, Space Shooter",        false, ""     },
  { "[BOX]", "Iнструменти",      "Калькулятор, секундомiр",      false, ""     },
  { "[MSG]", "Вiд Родини",       "Повiдомлення вiд батькiв",     false, ""     },
};
```

Хедер:

```cpp
tft.drawString("SIM OS", 8, 6);        // лого — без змін
tft.drawString("Fedir LVL9", 312, 10); // без змін
```

Статус бар:

```cpp
tft.drawString("* В МЕРЕЖI", 6, 212);   // замість "* ONLINE"
tft.drawString("SIM OS", 160, 212);
```

-----

### About Device екран — текст від батьків:

Знайди де малюється екран About / Вiд Родини.
Замінити текст на:

```cpp
const char* ABOUT_LINES[] = {
  "Федоре,",
  "",
  "Ти пройшов великий шлях.",
  "Вiд першої LED лампочки —",
  "до власної консолi.",
  "Вiд першого рядка коду —",
  "до гри Space Shooter.",
  "",
  "Ми пишаємося тобою",
  "кожен день.",
  "",
  "Продовжуй створювати.",
  "Це лише початок",
  "великої iсторiї.",
  "",
  "— Тато i Мама",
};
```

Малювати рядок за рядком з відступом 18px по вертикалі:

```cpp
void drawAboutScreen() {
  tft.fillScreen(tft.color565(6, 10, 7));

  // Лінія зліва — акцент
  tft.fillRect(8, 20, 3, 200, tft.color565(57, 217, 122));

  int y = 22;
  for (int i = 0; i < ABOUT_LINES_COUNT; i++) {
    if (strlen(ABOUT_LINES[i]) == 0) { y += 10; continue; }

    // Перший рядок "Федоре," — яскравіший
    if (i == 0) {
      tft.setTextColor(tft.color565(57, 217, 122), tft.color565(6, 10, 7));
      tft.setTextSize(2);
    }
    // Підпис "— Тато і Мама" — акцентний
    else if (i == ABOUT_LINES_COUNT - 1) {
      tft.setTextColor(tft.color565(168, 224, 99), tft.color565(6, 10, 7));
      tft.setTextSize(1);
    }
    // Решта — звичайний текст
    else {
      tft.setTextColor(tft.color565(208, 240, 216), tft.color565(6, 10, 7));
      tft.setTextSize(1);
    }

    tft.setTextDatum(TL_DATUM);
    tft.drawString(ABOUT_LINES[i], 18, y);
    y += (i == 0) ? 28 : 18;
  }

  // Кнопка назад внизу
  tft.drawRoundRect(80, 295, 160, 22, 4, tft.color565(58, 106, 74));
  tft.setTextColor(tft.color565(58, 106, 74), tft.color565(6, 10, 7));
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.drawString("< назад", 160, 306);
}
```

-----

## ПІДСУМОК — що змінити

|Екран      |Що змінити                              |
|-----------|----------------------------------------|
|Boot       |Масив `BOOT_LINES[]` → українською      |
|Final Slide|Додати новий екран між story і меню     |
|Final Slide|Текст → українською                     |
|Меню       |Масив `MENU_ITEMS[]` → назви українською|
|Статус бар |“ONLINE” → “В МЕРЕЖI”                   |
|About      |Додати екран з текстом від батьків      |