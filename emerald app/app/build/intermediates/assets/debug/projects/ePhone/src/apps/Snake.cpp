/**
 * Snake.cpp — Classic Snake game
 */

#include "Snake.h"
#include "../config.h"

extern TFT_eSPI tft;

// Control button positions — below the grid in portrait mode
static const int BTN_UP_X    = 120, BTN_UP_Y    = 270;
static const int BTN_DOWN_X  = 120, BTN_DOWN_Y  = 290;
static const int BTN_LEFT_X  = 80,  BTN_LEFT_Y  = 280;
static const int BTN_RIGHT_X = 160, BTN_RIGHT_Y = 280;
static const int BTN_SIZE = 36;

SnakeGame::SnakeGame()
    : _snakeLen(0)
    , _dir(DIR_NONE)
    , _nextDir(DIR_NONE)
    , _score(0)
    , _gameOver(false)
    , _lastTick(0)
    , _tickDelay(200)
{
}

void SnakeGame::reset() {
    _snakeLen = 3;
    _snake[0] = { GRID_W / 2, GRID_H / 2 };
    _snake[1] = { GRID_W / 2 - 1, GRID_H / 2 };
    _snake[2] = { GRID_W / 2 - 2, GRID_H / 2 };
    _dir = DIR_RIGHT;
    _nextDir = DIR_RIGHT;
    _score = 0;
    _gameOver = false;
    _tickDelay = 200;
    _lastTick = millis();
    spawnFood();
}

void SnakeGame::start() {
    tft.fillScreen(COLOR_BG);

    // Score header
    tft.fillRect(0, 0, DISP_WIDTH, 20, COLOR_PANEL);
    tft.drawFastHLine(0, 20, DISP_WIDTH, COLOR_DIM);
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("ZMIYKA", DISP_WIDTH / 2, 4, 2);
    tft.setTextDatum(TL_DATUM);

    reset();
    drawGrid();
    drawControls();
}

bool SnakeGame::update() {
    if (_gameOver) return true;

    unsigned long now = millis();
    if (now - _lastTick < (unsigned long)_tickDelay) return false;
    _lastTick = now;

    // Apply queued direction
    _dir = _nextDir;

    // Calculate new head position
    Point head = _snake[0];
    Point newHead = head;
    switch (_dir) {
        case DIR_UP:    newHead.y--; break;
        case DIR_DOWN:  newHead.y++; break;
        case DIR_LEFT:  newHead.x--; break;
        case DIR_RIGHT: newHead.x++; break;
        default: break;
    }

    // Check wall collision
    if (newHead.x < 0 || newHead.x >= GRID_W ||
        newHead.y < 0 || newHead.y >= GRID_H) {
        _gameOver = true;
        drawGameOver();
        return true;
    }

    // Check self collision (skip tail since it will move)
    for (int i = 0; i < _snakeLen - 1; i++) {
        if (newHead.x == _snake[i].x && newHead.y == _snake[i].y) {
            _gameOver = true;
            drawGameOver();
            return true;
        }
    }

    // Move snake: shift body
    for (int i = _snakeLen - 1; i > 0; i--) {
        _snake[i] = _snake[i - 1];
    }
    _snake[0] = newHead;

    // Check food
    if (newHead.x == _food.x && newHead.y == _food.y) {
        _score++;
        // Add new segment at tail
        _snake[_snakeLen] = _snake[_snakeLen - 1];
        _snakeLen++;

        // Speed up every 5 points
        if (_score % 5 == 0 && _tickDelay > 60) {
            _tickDelay -= 10;
        }

        if (!spawnFood()) {
            // No space for food = you win!
            _gameOver = true;
            drawGameOver();
            return true;
        }
    }

    // Redraw
    drawGrid();

    // Score
    char buf[16];
    snprintf(buf, sizeof(buf), "Rahunok: %d", _score);
    tft.fillRect(0, 0, DISP_WIDTH, 20, COLOR_PANEL);
    tft.drawFastHLine(0, 20, DISP_WIDTH, COLOR_DIM);
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(buf, DISP_WIDTH / 2, 4, 2);
    tft.setTextDatum(TL_DATUM);

    return false;
}

void SnakeGame::drawGameOver() {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_NEON, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("GRA ZAKINCHYLAS", DISP_WIDTH / 2, 60, 6);
    tft.setTextDatum(TL_DATUM);

    char buf[32];
    snprintf(buf, sizeof(buf), "Rahunok: %d", _score);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(buf, DISP_WIDTH / 2, 110, 4);
    tft.setTextDatum(TL_DATUM);

    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("Nazad do Menu", DISP_WIDTH / 2, 160, 2);
    tft.setTextDatum(TL_DATUM);
}

void SnakeGame::handleTap(int x, int y) {
    if (_gameOver) return;

    // Control buttons
    if (x >= BTN_UP_X && x <= BTN_UP_X + BTN_SIZE &&
        y >= BTN_UP_Y && y <= BTN_UP_Y + BTN_SIZE) {
        if (_dir != DIR_DOWN) _nextDir = DIR_UP;
    }
    else if (x >= BTN_DOWN_X && x <= BTN_DOWN_X + BTN_SIZE &&
             y >= BTN_DOWN_Y && y <= BTN_DOWN_Y + BTN_SIZE) {
        if (_dir != DIR_UP) _nextDir = DIR_DOWN;
    }
    else if (x >= BTN_LEFT_X && x <= BTN_LEFT_X + BTN_SIZE &&
             y >= BTN_LEFT_Y && y <= BTN_LEFT_Y + BTN_SIZE) {
        if (_dir != DIR_RIGHT) _nextDir = DIR_LEFT;
    }
    else if (x >= BTN_RIGHT_X && x <= BTN_RIGHT_X + BTN_SIZE &&
             y >= BTN_RIGHT_Y && y <= BTN_RIGHT_Y + BTN_SIZE) {
        if (_dir != DIR_LEFT) _nextDir = DIR_RIGHT;
    }
}

// handleGesture removed — swipe gestures not supported in current TouchManager

int SnakeGame::getScore() { return _score; }
bool SnakeGame::isGameOver() { return _gameOver; }

bool SnakeGame::spawnFood() {
    // Find all free cells
    int freeCells[GRID_W * GRID_H][2];
    int freeCount = 0;

    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            bool occupied = false;
            for (int i = 0; i < _snakeLen; i++) {
                if (_snake[i].x == x && _snake[i].y == y) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                freeCells[freeCount][0] = x;
                freeCells[freeCount][1] = y;
                freeCount++;
            }
        }
    }

    if (freeCount == 0) return false;

    int idx = random(0, freeCount);
    _food.x = freeCells[idx][0];
    _food.y = freeCells[idx][1];
    return true;
}

void SnakeGame::drawGrid() {
    // Game area offset — centered horizontally, below header
    int offsetX = (DISP_WIDTH - GRID_W * CELL_SIZE) / 2;
    int offsetY = 24;

    // Clear game area
    tft.fillRect(offsetX, offsetY,
                 GRID_W * CELL_SIZE, GRID_H * CELL_SIZE,
                 COLOR_BLACK);

    // Draw grid lines
    for (int x = 0; x <= GRID_W; x++) {
        tft.drawFastVLine(offsetX + x * CELL_SIZE, offsetY,
                          GRID_H * CELL_SIZE, COLOR_DIM);
    }
    for (int y = 0; y <= GRID_H; y++) {
        tft.drawFastHLine(offsetX, offsetY + y * CELL_SIZE,
                          GRID_W * CELL_SIZE, COLOR_DIM);
    }

    // Draw food
    drawCell(_food.x, _food.y, COLOR_YELLOW);

    // Draw snake
    for (int i = 0; i < _snakeLen; i++) {
        uint16_t c = (i == 0) ? COLOR_NEON : COLOR_NEON2;
        drawCell(_snake[i].x, _snake[i].y, c);
    }
}

void SnakeGame::drawCell(int x, int y, uint16_t color) {
    int offsetX = (DISP_WIDTH - GRID_W * CELL_SIZE) / 2;
    int offsetY = 24;
    tft.fillRect(offsetX + x * CELL_SIZE + 1,
                 offsetY + y * CELL_SIZE + 1,
                 CELL_SIZE - 2, CELL_SIZE - 2, color);
}

void SnakeGame::drawControls() {
    // Draw directional buttons
    auto drawBtn = [](int x, int y, const char* label) {
        tft.fillRoundRect(x, y, BTN_SIZE, BTN_SIZE, 4, COLOR_PANEL);
        tft.drawRoundRect(x, y, BTN_SIZE, BTN_SIZE, 4, COLOR_DIM);
        tft.setTextColor(COLOR_NEON, COLOR_PANEL);
        tft.setTextSize(1);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(label, x + BTN_SIZE / 2, y + BTN_SIZE / 2, 2);
        tft.setTextDatum(TL_DATUM);
    };

    drawBtn(BTN_UP_X, BTN_UP_Y,    "^");
    drawBtn(BTN_DOWN_X, BTN_DOWN_Y,  "v");
    drawBtn(BTN_LEFT_X, BTN_LEFT_Y,  "<");
    drawBtn(BTN_RIGHT_X, BTN_RIGHT_Y, ">");
}
