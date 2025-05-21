#ifndef SNAKE_H
#define SNAKE_H
#include <vector>
#include <SDL.h>
#include "Constants.h"
#include <string>
#include "tail.h"
#include "Vector2D.h"


class Snake {
public:
    Snake();
    ~Snake(); // Nên có destructor nếu bạn cấp phát động gì đó (hiện tại thì không)

    // Truyền SDL_Renderer vào đây để load texture đầu rắn và gọi Tails::Setup
        void Setup(SDL_Renderer* renderer,
               const std::string& headPath, // Chỉ cần một path cho đầu nếu là spritesheet hoặc dùng xoay
                                            // Hoặc 4 paths nếu là 4 ảnh riêng
               // Các đường dẫn cho Tails textures
               const std::string& tailBodyPath, const std::string& tailEndPath,
               const std::string& tailCurvePath, const std::string& tailCurveEndPath);

    void handleInput(SDL_Event& event);
    void update(); // Không còn truyền deltaTime nếu Tails và Snake dùng logic thời gian riêng
    void render(SDL_Renderer* renderer);
    void grow();
    void reset();

    bool checkCollisionWithSelf(); // Hàm kiểm tra va chạm mới
    bool checkFoodCollision(const SDL_Point& foodPos); // Sử dụng Vector2D

    Vector2D getHeadPosition() const;

    // Các hàm liên quan đến Speed Boost có thể giữ lại
    void increaseSpeed();
    void resetSpeed();
    bool isSpeedBoosted() const;
    Uint32 getSpeedBoostTimer() const;

private:
    Vector2D m_headPosition;    // Tọa độ pixel của đầu rắn
    Direction m_direction;       // Hướng hiện tại của đầu rắn
    Direction m_nextDirection;   // Hướng tiếp theo được người dùng nhập
    std::vector<SDL_Point> m_body;

    Tails m_tails; // <<<< ĐỐI TƯỢNG TAILS ĐỂ QUẢN LÝ THÂN VÀ ĐUÔI

    // Các texture cho đầu rắn (ví dụ, bạn có thể chọn cách quản lý khác)
    std::string m_headTextureID;
    std::string m_headTextureID_Up;
    std::string m_headTextureID_Down;
    std::string m_headTextureID_Left;
    std::string m_headTextureID_Right;

    SDL_Texture* m_texHeadUp;
    SDL_Texture* m_texHeadDown;
    SDL_Texture* m_texHeadLeft;
    SDL_Texture* m_texHeadRight;
    SDL_Renderer* m_renderer_snake;

    // Thông số di chuyển và tốc độ
    static float s_moveTimer; // Đưa moveTimer vào thành viên static hoặc non-static
                              // Nếu non-static, nó sẽ reset mỗi khi tạo đối tượng Snake mới (tốt hơn cho reset game)
                              // Nếu static, nó sẽ giữ giá trị giữa các lần tạo Snake (có thể không mong muốn)
                              // Mình sẽ đổi thành non-static:
    float m_moveTimer;

    float m_normalDelay;
    float m_boostedDelay;
    bool m_speedBoosted;
    Uint32 m_speedBoostTimer;
    // bool m_justGrew; // Cờ này có thể không cần nữa nếu Tails::Grow() và Tails::MoveBody() xử lý đúng

    int m_gridSize; // Nên lấy từ Constants.h hoặc truyền vào
    Direction getSegmentDirection(size_t index) const;
    int convertDirectionToAngle(Direction dir);
};


#endif
