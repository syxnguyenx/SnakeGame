#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

#endif
