#include "food.h"
#include <SDL2/SDL.h>
#include <random>

Food::Food() : m_isSpeedFood(false), m_active(false) {}

void Food::generateNormalFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> yDIst(0, GRID_HEIGHT - 1);

    m_position.x = xDist(gen);
    m_position.y = yDIst(gen);
    m_isSpeedFood = false;
    m_active = true;
}

void Food::generateSpeedFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> yDIst(0, GRID_HEIGHT - 1);

    m_position.x = xDist(gen);
    m_position.y = yDIst(gen);
    m_isSpeedFood = true;
    m_active = true;
    m_speedFoodTimer = SDL_GetTicks();
}
