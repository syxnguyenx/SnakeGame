#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <SDL.h>
#include "constants.h"

class Snake {
public:
    //constructor khởi tạo con rắn
    Snake();

    //hàm xử lí Input người chơi
    void handleInput();
    //cập nhật trạng thái con rắn
    void update();
    //vẽ lên màn hình
    void render(SDL_Renderer* renderer);
    //tăng độ dài con rắn
    void grow();
    //reset trạng thái con rắn về ban đầu
    void reset();

    //kiểm tra va chạm vs chính nó
    bool checkCollision() const;
    // kiểm tra va chạm với thức ăn
    bool checkCollisionFood(const SDL_Point& foodPos) const;

    //tăng tốc khi ăn thức ăn tăng tốc
    void increaSpeed() {
        m_speedBoosted = true;
        m_speedBoostTimer = SDL_GetTicks();
    }
    //reset tốc độ về bth
    void resetSpeed() {m_speedBoosted = false; }
    //kiểm tra trạng thái tnagw tốc
    bool isSpeedBoosted() const {return m_direction; }
    //getter thân rắn
    const std::vector<SDL_Point>& getBody() const {return m_body; }

private:
    std::vector<SDL_Point> m_body; //các đoạn thân
    Direction m_direction; //hướng di chuyển hiện tại
    Direction m_nextDirection; //hướng di chuyển tiếp theo
    float m_moveDelay; // thời gian delay giauwx mỗi lần di chuyển
    float m_normanlDelay; // delay khi di chuyển bth
    float m_boostedDelay;// deley khi tnagw tốc
    float m_speedBoosted; // trạng thái tăng tốc
    Uint32 m_speedBoostedTimer; // thời điểm bắt đầu tăng tốc
};

#endif
