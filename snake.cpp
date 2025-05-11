#include "Snake.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "TextureManager.h"
#include "Game.h"
#include <iostream>
#include <vector>

Snake::Snake() :
    m_direction(RIGHT),
    m_nextDirection(RIGHT),
    m_moveDelay(0.2f),
    m_normalDelay(0.2f),
    m_boostedDelay(0.1f),
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
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                if (m_direction != DOWN) m_nextDirection = UP;
                break;
            case SDLK_DOWN:
                if (m_direction != UP) m_nextDirection = DOWN;
                break;
            case SDLK_LEFT:
                if (m_direction != RIGHT) m_nextDirection = LEFT;
                break;
            case SDLK_RIGHT:
                if (m_direction != LEFT) m_nextDirection = RIGHT;
                break;
        }
    }
}

void Snake::update() {
    static float moveTimer = 0;
    float deltaTime = 0.016f;
    moveTimer += deltaTime;

    if (moveTimer >= m_moveDelay) {
        moveTimer = 0;
        m_direction = m_nextDirection;

        SDL_Point newHead = m_body.front();
        switch (m_direction) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        if (newHead.x < 0) newHead.x = GRID_WIDTH - 1;
        if (newHead.x >= GRID_WIDTH) newHead.x = 0;
        if (newHead.y < 0) newHead.y = GRID_HEIGHT - 1;
        if (newHead.y >= GRID_HEIGHT) newHead.y = 0;

        m_body.insert(m_body.begin(), newHead);
        m_body.pop_back();
    }

    if (m_speedBoosted && SDL_GetTicks() - m_speedBoostTimer > 3000) {
        resetSpeed();
    }
}

void Snake::render(SDL_Renderer* renderer) {
    for (size_t i = 0; i < m_body.size(); ++i) {
        SDL_Rect rect = {
            m_body[i].x * GRID_SIZE,
            m_body[i].y * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE
        };

        if (i == 0) {
            // Vẽ đầu (không thay đổi)
            switch (m_direction) {
                case UP:
                    TextureManager::Instance()->draw(headTextureIds[0], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case DOWN:
                    TextureManager::Instance()->draw(headTextureIds[1], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case LEFT:
                    TextureManager::Instance()->draw(headTextureIds[2], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case RIGHT:
                    TextureManager::Instance()->draw(headTextureIds[3], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
            }
        } else if (i == m_body.size() - 1) {
            // Vẽ đuôi (tương tự như logic đầu, dựa trên hướng đốt cuối cùng)
            Direction tailDir = getSegmentDirection(i - 1);
            switch (tailDir) {
                case UP:
                    TextureManager::Instance()->draw(tailTextureIds[0], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case DOWN:
                    TextureManager::Instance()->draw(tailTextureIds[1], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case LEFT:
                    TextureManager::Instance()->draw(tailTextureIds[2], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
                case RIGHT:
                    TextureManager::Instance()->draw(tailTextureIds[3], rect.x, rect.y, rect.w, rect.h, renderer);
                    break;
            }
        } else {
            // Vẽ thân - LOGIC THAY ĐỔI LỚN
            Direction prevDir = getSegmentDirection(i - 1);
            Direction currentDir = getSegmentDirection(i); // Hướng từ đốt i-1 đến đốt i

            if (prevDir == currentDir) {
                // Thân thẳng
                if (prevDir == LEFT || prevDir == RIGHT) {
                    TextureManager::Instance()->draw(bodyTextureIds[0], rect.x, rect.y, rect.w, rect.h, renderer); // horizontal
                } else {
                    TextureManager::Instance()->draw(bodyTextureIds[1], rect.x, rect.y, rect.w, rect.h, renderer); // vertical
                }
            } else {
                // Thân cong
                if ((prevDir == RIGHT && currentDir == DOWN) || (prevDir == UP && currentDir == LEFT)) {
                    TextureManager::Instance()->draw(bodyTextureIds[6], rect.x, rect.y, rect.w, rect.h, renderer); // right down / up left
                } else if ((prevDir == LEFT && currentDir == DOWN) || (prevDir == UP && currentDir == RIGHT)) {
                    TextureManager::Instance()->draw(bodyTextureIds[7], rect.x, rect.y, rect.w, rect.h, renderer); // left down / up right
                } else if ((prevDir == RIGHT && currentDir == UP) || (prevDir == DOWN && currentDir == LEFT)) {
                    TextureManager::Instance()->draw(bodyTextureIds[8], rect.x, rect.y, rect.w, rect.h, renderer); // right up / down left
                } else if ((prevDir == LEFT && currentDir == UP) || (prevDir == DOWN && currentDir == RIGHT)) {
                    TextureManager::Instance()->draw(bodyTextureIds[9], rect.x, rect.y, rect.w, rect.h, renderer); // left up / down right
                }
                // Các trường hợp khúc cua khác (nếu cần, dựa trên "body_topleft", etc.)
                else if ((prevDir == DOWN && currentDir == RIGHT) || (prevDir == LEFT && currentDir == UP)) {
                    TextureManager::Instance()->draw(bodyTextureIds[2], rect.x, rect.y, rect.w, rect.h, renderer); // down right / left up
                } else if ((prevDir == DOWN && currentDir == LEFT) || (prevDir == RIGHT && currentDir == UP)) {
                    TextureManager::Instance()->draw(bodyTextureIds[3], rect.x, rect.y, rect.w, rect.h, renderer); // down left / right up
                } else if ((prevDir == UP && currentDir == RIGHT) || (prevDir == LEFT && currentDir == DOWN)) {
                    TextureManager::Instance()->draw(bodyTextureIds[4], rect.x, rect.y, rect.w, rect.h, renderer); // up right / left down
                } else if ((prevDir == UP && currentDir == LEFT) || (prevDir == RIGHT && currentDir == DOWN)) {
                    TextureManager::Instance()->draw(bodyTextureIds[5], rect.x, rect.y, rect.w, rect.h, renderer); // up left / right down
                }
            }
        }
    }
}

Direction Snake::getSegmentDirection(size_t index) const {
    if (index <= 0 || index >= m_body.size()) {
        return m_direction; // Hoặc một hướng mặc định cho đốt đầu tiên
    }

    if (m_body[index].x < m_body[index - 1].x) {
        return RIGHT;
    } else if (m_body[index].x > m_body[index - 1].x) {
        return LEFT;
    } else if (m_body[index].y < m_body[index - 1].y) {
        return DOWN;
    } else if (m_body[index].y > m_body[index - 1].y) {
        return UP;
    }

    return m_direction; // Trường hợp không rõ ràng
}

void Snake::grow() {
    SDL_Point tail = m_body.back();
    m_body.push_back(tail);
}

bool Snake::checkCollision() const {
    for (size_t i = 1; i < m_body.size(); ++i) {
        if (m_body[0].x == m_body[i].x && m_body[0].y == m_body[i].y) {
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
