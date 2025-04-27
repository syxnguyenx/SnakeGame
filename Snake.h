#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <SDL.h>
#include "Constants.h"

class Snake {
public:
    Snake();

    void handleInput(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);
    void grow();
    void reset();

    bool checkCollision() const;
    bool checkFoodCollision(const SDL_Point& foodPos) const;

    void increaseSpeed() { m_speedBoosted = true; m_speedBoostTimer = SDL_GetTicks(); }
    void resetSpeed() { m_speedBoosted = false; }
    bool isSpeedBoosted() const { return m_speedBoosted; }

    Direction getDirection() const { return m_direction; }
    const std::vector<SDL_Point>& getBody() const { return m_body; }
    Uint32 getSpeedBoostTimer() const;
private:
    std::vector<SDL_Point> m_body;
    Direction m_direction;
    Direction m_nextDirection;
    float m_moveDelay;
    float m_normalDelay;
    float m_boostedDelay;
    bool m_speedBoosted;
    Uint32 m_speedBoostTimer;
};

#endif
