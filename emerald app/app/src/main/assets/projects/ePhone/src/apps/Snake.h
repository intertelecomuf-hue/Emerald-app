/**
 * Snake.h — Classic Snake game for Emerald
 *
 * Grid: 20x15 cells, 14px each
 * Controls: 4 directional touch buttons
 * Speed: starts at 200ms/tick, +10ms faster every 5 points
 */

#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include <TFT_eSPI.h>
class SnakeGame {
public:
    SnakeGame();

    void start();
    bool update();     // returns true when game is over
    void handleTap(int x, int y);

    int  getScore();
    bool isGameOver();

private:
    static const int GRID_W = 15;
    static const int GRID_H = 20;
    static const int CELL_SIZE = 12;

    enum Direction { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

    struct Point {
        int x, y;
    };

    Point _snake[GRID_W * GRID_H];
    int   _snakeLen;
    Direction _dir;
    Direction _nextDir;
    Point _food;

    int  _score;
    bool _gameOver;
    unsigned long _lastTick;
    int  _tickDelay;

    void reset();
    bool spawnFood();
    bool checkCollision(int x, int y);
    void drawGrid();
    void drawCell(int x, int y, uint16_t color);
    void drawControls();
    void drawGameOver();
};

#endif // SNAKE_H
