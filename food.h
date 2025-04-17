#ifndef FOOD_H
#define FOOD_H

#include <SDL.h>
#include "constants.h"

class Food {
public:
    Food();
    //tạo thức an thường
    void generateNormalFood();
    //tạo thức ăn tốc độ
    void generateSpeedFood();
    //vẽ ra
    void render(SDL_Renderer* renderer);
    //cập nhật trạng thái thức ăn(kiểm tra thời gian tội tại)
    void update();

    //vị trí thức ăn
    SDL_Point getPosition{} const {return m_position;}
    //kiểm tra xem có phải thức ăn tăng tốc k
    bool isSpeedFood() const {return m_isSpeedFood;}
    //kiêm tra thức ăn có hoạt động k
    bool isActive() const {return _isactive;}

private:
    SDL_Point m_position;//vị trí (x,y) trên lưới
    bool m_isSpeedFood;//cờ phân loại thức ăn
    bool m_isactive;//trạng thái hoạt động
    Uint32 m_speedFoodTimer;//thời gian tồn tại cảu thức ăn tăng tốc
};

#endif
