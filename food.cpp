#include "Food.h"
#include "TextureManager.h" // Đảm bảo include header này
#include "Constants.h"

Food::Food(int gridWidth, int gridHeight) :
    m_position({0, 0}), // Khởi tạo mặc định hoặc bạn có thể tính toán
    m_isActive(false),
    m_isSpeedFood(false),
    m_gridWidth(gridWidth),
    m_gridHeight(gridHeight),
    m_type(BALL)
{
    generateFood();
}

Food::~Food() {}

void Food::update() {
    // Hiện tại không có logic cập nhật đặc biệt cho thức ăn
}

void Food::render(SDL_Renderer* renderer) {
    if (m_isActive) {
        std::string textureID;
        switch (m_type) {
            case BALL:
                textureID = "ball"; // ID cho mồi thường
                break;
            case SPEED_BOOST:
                textureID = "speed_food"; // <<< ID cho mồi tăng tốc (quả bóng đỏ)
                break;
        }
        TextureManager::Instance()->draw(textureID, m_position.x * GRID_SIZE, m_position.y * GRID_SIZE, GRID_SIZE, GRID_SIZE, renderer);
    }
}

void Food::generateFood() {
    m_position.x = rand() % m_gridWidth;
    m_position.y = rand() % m_gridHeight;
    m_isActive = true;
    m_isSpeedFood = false;
    m_type = BALL;
}

void Food::generateSpeedFood() {
    m_position.x = rand() % m_gridWidth;
    m_position.y = rand() % m_gridHeight;
    m_isActive = true;
    m_isSpeedFood = true;
    m_type = SPEED_BOOST;
}
