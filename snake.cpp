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
    m_speedBoosted(false),
    m_justGrew(false)
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
    m_justGrew = false;
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

// Snake.cpp
void Snake::update() {
    static float moveTimer = 0;
    float currentMoveDelay = m_speedBoosted ? m_boostedDelay : m_normalDelay;
    // float deltaTime = 0.016f; // Xem xét việc truyền deltaTime thực từ Game::update()
    float actualDeltaTime = 0.016f; // Tạm thời giữ nguyên nếu bạn chưa muốn thay đổi lớn
    moveTimer += actualDeltaTime;

    if (moveTimer >= currentMoveDelay) { // <<< ĐÃ SỬA: Dùng currentMoveDelay
        moveTimer = 0;
        m_direction = m_nextDirection;

        SDL_Point newHead = m_body.front();
        switch (m_direction) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        // Xử lý xuyên tường
        if (newHead.x < 0) newHead.x = GRID_WIDTH - 1;
        if (newHead.x >= GRID_WIDTH) newHead.x = 0;
        if (newHead.y < 0) newHead.y = GRID_HEIGHT - 1;
        if (newHead.y >= GRID_HEIGHT) newHead.y = 0;

        m_body.insert(m_body.begin(), newHead); // Thêm đầu mới

        if (!m_justGrew) { // <<< THÊM LOGIC KIỂM TRA CỜ
            m_body.pop_back(); // Chỉ xóa đuôi nếu không phải vừa lớn lên
        }
        m_justGrew = false; // Reset cờ cho lần update tiếp theo
    }

    if (m_speedBoosted && SDL_GetTicks() - m_speedBoostTimer > 3000) {
        resetSpeed();
    }
}

void Snake::render(SDL_Renderer* renderer) {
    if (m_body.empty()) { // Đề phòng trường hợp m_body rỗng
        return;
    }

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
            Direction dirToTail = getSegmentDirection(i);
            switch (dirToTail) {
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
                default:
                    TextureManager::Instance()->draw(tailTextureIds[3], rect.x, rect.y, rect.w, rect.h, renderer); break;
            }
        } else { // Vẽ đốt thân ở giữa (body segment)

            Direction prevDir = getSegmentDirection(i - 1);
            Direction currentDir = getSegmentDirection(i);
            if (i == 1) { // Chỉ in ra cho đốt ngay sau đầu để dễ theo dõi
                std::cout << "Dot i=1: prevDir=" << prevDir << ", currentDir=" << currentDir << std::endl;
    }

            if (prevDir == currentDir) {
        // Nếu hướng của đốt S[i-1] và hướng của đốt S[i] là giống nhau, rắn đi thẳng.
        if (currentDir == LEFT || currentDir == RIGHT) { // Dùng currentDir hoặc prevDir đều được vì chúng bằng nhau
            // std::cout << "      Ve than ngang (bodyTextureIds[0])" << std::endl;
            TextureManager::Instance()->draw(bodyTextureIds[0], rect.x, rect.y, rect.w, rect.h, renderer); // body_horizontal
        } else { // currentDir là UP hoặc DOWN
            // std::cout << "      Ve than doc (bodyTextureIds[1])" << std::endl;
            TextureManager::Instance()->draw(bodyTextureIds[1], rect.x, rect.y, rect.w, rect.h, renderer); // body_vertical
        }
    }  else { // Thân cong
        if ((prevDir == RIGHT && currentDir == UP) || (prevDir == DOWN && currentDir == LEFT)) {
            TextureManager::Instance()->draw(bodyTextureIds[2], rect.x, rect.y, rect.w, rect.h, renderer);
        } else if ((prevDir == LEFT && currentDir == UP) || (prevDir == DOWN && currentDir == RIGHT)) {
            TextureManager::Instance()->draw(bodyTextureIds[3], rect.x, rect.y, rect.w, rect.h, renderer);
        } else if ((prevDir == RIGHT && currentDir == DOWN) || (prevDir == UP && currentDir == LEFT)) {
            TextureManager::Instance()->draw(bodyTextureIds[4], rect.x, rect.y, rect.w, rect.h, renderer);
        } else if ((prevDir == LEFT && currentDir == DOWN) || (prevDir == UP && currentDir == RIGHT)) {
            TextureManager::Instance()->draw(bodyTextureIds[5], rect.x, rect.y, rect.w, rect.h, renderer);
        } else {
            TextureManager::Instance()->draw(bodyTextureIds[0], rect.x, rect.y, rect.w, rect.h, renderer);
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
        return LEFT;
    } else if (m_body[index].x > m_body[index - 1].x) {
        return RIGHT;
    } else if (m_body[index].y < m_body[index - 1].y) {
        return UP;
    } else if (m_body[index].y > m_body[index - 1].y) {
        return DOWN;
    }

    return m_direction; // Trường hợp không rõ ràng
}

void Snake::grow() {
    SDL_Point tail = m_body.back();
    m_body.push_back(tail);
    m_justGrew = true;
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
