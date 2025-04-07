#ifndef SNAKE_H
#define SNAKE_H

#include <SDL.h>
#include <vector>

//dinh nghia cau truc con ran
struct Snake {
    std::vector<SDL_Point> body;
    SDL_Point direction ={1, 0};
    float normalSpeed = 0.1f;
    float boostedSpeed = 0.3f;
    float currentSpeed = 0.1f;
    Unit32 speedBoostEndTime = 0;
};

void initializeSnake(Snake& snake, int startX, int startY); //ham tao con ran
void updateSnake(Snake& snake, bool& running, SDL_Point& food, GameSettings& settings, GameState& state);//ham cap nhat con ran
void renderSnake(SDL_Renderer* renderer, const Snake& snake);//hamf ve con ran
void handleMouseControl(Snake& snake, int mouseX, int mouseY);

#endif // SNAKE_H
