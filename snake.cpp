#include "Snake.h"
#include <SDL.h>
#include <SDL_mixer.h>
Snake::Snake() :
    m_direction(RIGHT),
    m_nextDirection(RIGHT),
    m_normalDelay(0.2f),
    m_boostedDelay(0.1f),
    m_moveDelay(m_normalDelay),
    m_speedBoosted(false)
{
    reset();
}

void Snake::reset() {
    m_body.clear();
    m_body.push_back({5, 5});
    m_body.push_back({4, 5});
    m_body.push_back({3, 5});
    m_direction = RIGHT;
    m_nextDirection = RIGHT;
    m_speedBoosted = false;
}

void Snake::handleInput(SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_UP:
                if(m_direction != DOWN) m_nextDirection = UP;
                break;
            case SDLK_DOWN:
                if(m_direction != UP) m_nextDirection = DOWN;
                break;
            case SDLK_LEFT:
                if(m_direction != RIGHT) m_nextDirection = LEFT;
                break;
            case SDLK_RIGHT:
                if(m_direction != LEFT) m_nextDirection = RIGHT;
                break;
        }
    }
}

void Snake::update() {
    static float moveTimer = 0;
    float deltaTime = 0.016f; // Approximate for 60 FPS

    moveTimer += deltaTime;
    if(moveTimer >= m_moveDelay) {
        moveTimer = 0;
        m_direction = m_nextDirection;

        // Move head
        SDL_Point newHead = m_body.front();
        switch(m_direction) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        // Check boundaries
        if(newHead.x < 0) newHead.x = GRID_WIDTH - 1;
        if(newHead.x >= GRID_WIDTH) newHead.x = 0;
        if(newHead.y < 0) newHead.y = GRID_HEIGHT - 1;
        if(newHead.y >= GRID_HEIGHT) newHead.y = 0;

        m_body.insert(m_body.begin(), newHead);
        m_body.pop_back();
    }

    // Check speed boost timer
    if(m_speedBoosted && SDL_GetTicks() - m_speedBoostTimer > 3000) {
        resetSpeed();
    }
}

void Snake::render(SDL_Renderer* renderer) {
    for(size_t i = 0; i < m_body.size(); ++i) {
        SDL_Rect rect = {
            m_body[i].x * GRID_SIZE,
            m_body[i].y * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE
        };

        // Head is green, body is darker green
        if(i == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Snake::grow() {
    SDL_Point tail = m_body.back();
    m_body.push_back(tail);
}

bool Snake::checkCollision() const {
    // Check collision with itself
    for(size_t i = 1; i < m_body.size(); ++i) {
        if(m_body[0].x == m_body[i].x && m_body[0].y == m_body[i].y) {
            return true;
        }
    }
    return false;
}

bool Snake::checkFoodCollision(const SDL_Point& foodPos) const {
    return (m_body[0].x == foodPos.x && m_body[0].y == foodPos.y);
}
Uint32 Snake::getSpeedBoostTimer() const {
    return m_speedBoostTimer;
}

