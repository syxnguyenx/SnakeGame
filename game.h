#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#incldue <SDL_ttf.h>
#include <vector>

enum FoodType { NOMAL, SPEED_BOOST };

struct Food {
    SDL_Point position;
    FoodType type;
    Unit32 spawnTime;
};

struct GameSettings {
    float speed = 0.1f;
    int volume = 50;
    bool mouseControl = false;
    bool passWall = false;
};

struct GameState {
    int currentScore = 0;
    int hightScore = 0;
    bool gameOver = false;
};

//ham khoi tao
void initialize(SDL_Window*& window, SDL_Renderer*& renderer, TTF_Font*& font);

//ham menu
void showMainMenu(SDL_Renderer* renderer, GameSettings& settings, GameState& state, TTF_Font* font);
void showGameOver(SDL_Renderer* rederer, GameState& state, TTF_Font* font);

//hamf thuc an
void spawnFood(Food& food, const std::vector<SDL_point>& snakeBody, FoodType type = NORMAL);
void renderFood(SDL_Renderer* renderer, const SDL_Point& food);

//ham tien ich
void saveHighScore(int score);
int loadHighScore();

#endif // GAME_H
