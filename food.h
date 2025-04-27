#ifndef FOOD_H
#define FOOD_H

#include <SDL.h>
#include "Constants.h"

class Food {
public:
    Food();
    void generateNormalFood();
    void generateSpeedFood();
    void render(SDL_Renderer* renderer);
    void update();

    SDL_Point getPosition() const { return m_position; }
    bool isSpeedFood() const { return m_isSpeedFood; }
    bool isActive() const { return m_active; }

private:
    SDL_Point m_position;
    bool m_isSpeedFood;
    bool m_active;
    Uint32 m_speedFoodTimer;
};

#endif
