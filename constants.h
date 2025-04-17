#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 600;
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

enum GameState {
    MENU;
    PLAYING;
    PAUSED;
    GAME_OVER;
};

enum Direction {
    UP;
    DOWN;
    RIGHT;
    LEFT;
};

#endif // CONSTANTS_H_INCLUDED
