#include "Snake.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "TextureManager.h"
#include "Game.h"
#include "tail.h"
#include <iostream>
#include <vector>

Snake::Snake() :
    m_headPosition({0,0}), // Sẽ được đặt lại trong reset() hoặc Setup()
    m_direction(RIGHT),
    m_nextDirection(RIGHT),
    m_moveTimer(0.0f), // Khởi tạo m_moveTimer
    m_normalDelay(0.2f),
    m_boostedDelay(0.1f),
    m_speedBoosted(false),
    m_speedBoostTimer(0),
    // m_justGrew(false), // Không cần nếu Tails xử lý tốt
    m_gridSize(GRID_SIZE), // Giả sử GRID_SIZE được định nghĩa trong Constants.h
    m_texHeadUp(nullptr),   // Khởi tạo các con trỏ texture là nullptr
    m_texHeadDown(nullptr),
    m_texHeadLeft(nullptr),
    m_texHeadRight(nullptr)
{
    std::cout << "Snake constructor called." << std::endl;
}

Snake::~Snake() {
    // Giải phóng texture đầu rắn
    if (m_texHeadUp) SDL_DestroyTexture(m_texHeadUp);
    if (m_texHeadDown) SDL_DestroyTexture(m_texHeadDown);
    if (m_texHeadLeft) SDL_DestroyTexture(m_texHeadLeft);
    if (m_texHeadRight) SDL_DestroyTexture(m_texHeadRight);
    // m_tails sẽ tự gọi destructor của nó (Tails::~Tails() sẽ gọi FreeTexture())
}
// Snake.cpp
void Snake::Setup(SDL_Renderer* renderer,
                  const std::string& headPath, // Đường dẫn cho ảnh đầu
                  const std::string& tailBodyPath,
                  const std::string& tailEndPath,
                  const std::string& tailCurvePath,
                  const std::string& tailCurveEndPath) {
    this->m_renderer_snake = renderer;
    this->m_gridSize = GRID_SIZE; // Hoặc giá trị bạn muốn

    m_headTextureID = "snake_head"; // Đặt ID
    if (!TextureManager::Instance()->load(headPath, m_headTextureID, renderer)) {
        std::cerr << "Loi load texture dau ran: " << headPath << std::endl;
    }

    // Gọi Tails::Setup
    m_tails.Setup(renderer,
                  /* bodyPath cho body.png */ "body.png", "tails_body_straight",
                  /* tailPath cho last_tail.png */ "last_tail.png", "tails_last_straight",
                  /* curvePath cho curve.png */ "curve.png", "tails_body_curve",
                  /* curveTailPath cho curve_tail.png */ "curve_tail.png", "tails_curve_end",
                  3, this->m_gridSize);
    reset();
}
// Snake.cpp
void Snake::reset() {
    m_direction = RIGHT;
    m_nextDirection = RIGHT;
    m_speedBoosted = false;
    m_speedBoostTimer = 0;
    m_moveTimer = 0.0f;

   m_headPosition.x = static_cast<int>(5.0f * m_gridSize);
    m_headPosition.y = static_cast<int>(5.0f * m_gridSize);
    m_direction = RIGHT;
    m_nextDirection = RIGHT;
    // ...

    int initialTailSegments = 3;
    m_tails.InitializeBody(m_headPosition, convertDirectionToAngle(m_direction), initialTailSegments, m_gridSize);

    for (int i = 0; i < m_tails.getTotalSegments(); ++i) {
        Vector2D segPos = m_tails.getSegmentPosition(i);
    }
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
    m_moveTimer += 0.016f; // Giả sử vẫn dùng fixed delta time cho logic di chuyển
    float currentActualMoveDelay = m_speedBoosted ? m_boostedDelay : m_normalDelay;

    if (m_moveTimer >= currentActualMoveDelay) {
        m_moveTimer = 0.0f;
        m_direction = m_nextDirection;

        // Lưu vị trí đầu cũ để các đốt thân theo sau
        Vector2D oldHeadPosition = m_headPosition;
        int oldHeadAngle = convertDirectionToAngle(m_direction); // Góc của đầu trước khi nó di chuyển theo hướng mới này

        // Cập nhật vị trí đầu rắn mới
        switch (m_direction) {
            case UP:    m_headPosition.y -= m_gridSize; break;
            case DOWN:  m_headPosition.y += m_gridSize; break;
            case LEFT:  m_headPosition.x -= m_gridSize; break;
            case RIGHT: m_headPosition.x += m_gridSize; break;
        }

        // Xử lý xuyên tường cho đầu rắn
        if (m_headPosition.x < 0) m_headPosition.x = (GRID_WIDTH - 1) * m_gridSize;
        else if (m_headPosition.x >= SCREEN_WIDTH) m_headPosition.x = 0;
        if (m_headPosition.y < 0) m_headPosition.y = (GRID_HEIGHT - 1) * m_gridSize;
        else if (m_headPosition.y >= SCREEN_HEIGHT) m_headPosition.y = 0;

        // Ra lệnh cho các đốt thân/đuôi (Tails) di chuyển theo vị trí ĐẦU CŨ và GÓC CŨ
        // Vì đốt thân đầu tiên sẽ di chuyển vào vị trí mà đầu rắn vừa rời đi.
        m_tails.MoveBody(oldHeadPosition, oldHeadAngle);
    }

    if (m_speedBoosted && SDL_GetTicks() - m_speedBoostTimer > 3000) {
        resetSpeed();
    }
}

// Snake.cpp
void Snake::render(SDL_Renderer* renderer) {
    // 1. Vẽ thân và đuôi do Tails quản lý
    m_tails.Render(renderer); // Gọi hàm Render đã sửa của Tails

    // 2. Vẽ đầu rắn (luôn ở trên cùng)
    double headAngle = static_cast<double>(convertDirectionToAngle(m_direction));
    SDL_RendererFlip headFlip = SDL_FLIP_NONE; // Đầu rắn thường không cần lật

    if (!m_headTextureID.empty()) { // m_headTextureID là ID của "snake.png"
        TextureManager::Instance()->draw(m_headTextureID,
                                         (int)m_headPosition.x, (int)m_headPosition.y,
                                         m_gridSize, m_gridSize,
                                         renderer, nullptr, headAngle, nullptr, headFlip);
    } else {
        SDL_Rect headDestRect = {(int)m_headPosition.x, (int)m_headPosition.y, m_gridSize, m_gridSize};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &headDestRect);
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
    m_tails.Grow(); // Gọi hàm Grow của Tails
}

bool Snake::checkCollisionWithSelf() {
    // ... (các std::cout debug) ...
    Vector2D headPos = getHeadPosition();
    for (int i = 0; i < m_tails.getTotalSegments(); ++i) {
        Vector2D segmentPos = m_tails.getSegmentPosition(i);
        if (headPos.x == segmentPos.x && headPos.y == segmentPos.y) {
            std::cout << "   !!! VA CHAM VOI THAN TAI DOT " << i << " CUA TAILS !!!"
                      << " Dau (" << headPos.x << "," << headPos.y << ")"
                      << " vs Dot (" << segmentPos.x << "," << segmentPos.y << ")" << std::endl;
            return true;
        }
    }
    return false;
}


bool Snake::checkFoodCollision(const SDL_Point& foodPos) {
    int headGridX = m_headPosition.x / GRID_SIZE;
    int headGridY = m_headPosition.y / GRID_SIZE;

    return (headGridX == foodPos.x && headGridY == foodPos.y);
}

Vector2D Snake::getHeadPosition() const {
    return m_headPosition;
}

int Snake::convertDirectionToAngle(Direction dir) {
    switch (dir) {
        case UP:    return 270; // Hoặc -90, tùy thuộc vào TextureManager::draw và ảnh của bạn
        case DOWN:  return 90;
        case LEFT:  return 180;
        case RIGHT: return 0;
        default:    return 0;
    }
}

// Trong tail.cpp (hoặc file chứa hàm này)
// (Đảm bảo UNKNOWN_DIRECTION đã được thêm vào enum Direction trong Constants.h)

void Snake::increaseSpeed() {
    m_speedBoosted = true;
    m_speedBoostTimer = SDL_GetTicks(); // SDL_GetTicks() cần <SDL.h> đã được include
    std::cout << "Snake::increaseSpeed() called. m_speedBoosted = " << m_speedBoosted << std::endl;
}

void Snake::resetSpeed() {
    m_speedBoosted = false;
    std::cout << "Snake::resetSpeed() called. m_speedBoosted = " << m_speedBoosted << std::endl;
}

bool Snake::isSpeedBoosted() const {
    return m_speedBoosted;
}

Uint32 Snake::getSpeedBoostTimer() const {
    return m_speedBoostTimer;
}
