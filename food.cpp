#include "Food.h"
#include <SDL.h>
#include <random>
#include <SDL_mixer.h>
Food::Food() : m_isSpeedFood(false), m_active(false) {}

void Food::generateNormalFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> yDist(0, GRID_HEIGHT - 1);

    m_position.x = xDist(gen);
    m_position.y = yDist(gen);
    m_isSpeedFood = false;
    m_active = true;
}

void Food::generateSpeedFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> yDist(0, GRID_HEIGHT - 1);

    m_position.x = xDist(gen);
    m_position.y = yDist(gen);
    m_isSpeedFood = true;
    m_active = true;
    m_speedFoodTimer = SDL_GetTicks();
}

void Food::render(SDL_Renderer* renderer) {
    if(!m_active) return;

    SDL_Rect rect = {
        m_position.x * GRID_SIZE,
        m_position.y * GRID_SIZE,
        GRID_SIZE,
        GRID_SIZE
    };

    if(m_isSpeedFood) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for speed food
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for normal food
    }
    SDL_RenderFillRect(renderer, &rect);
}

void Food::update() {
    if(m_isSpeedFood && m_active) {
        if(SDL_GetTicks() - m_speedFoodTimer > 5000) { // 5 seconds
            m_active = false;
        }
    }
}
