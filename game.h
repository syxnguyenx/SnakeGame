#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "food.h"
#include "Snake.h"
#include "constants.h"

class Game {
public:
    //constructor khởi tạo
    Game();
    ~Game();//destructor giải phóng tài nguyên

    void init();// khởi tạo game
    void run();//vòng lặp game
    void clean();//giải phóng tài nguyên

private:
    void handleEvent();//xử lí sk
    void update();//câppj nhật trnagj thái game
    void render();//vẽ lên màn hình
    void renderMenu();//vẽ menu
    void renderPause();//vẽ màn hình páue
    void renderGameOver();//vẽ màn hình game over
    void renderSpeedBoostBar();//thanh hiển thị tăng tốc

    SDL_Window* m_window;//cửa sổ game
    SDL_Renderer* renderer;//Renderer để vẽ
    TTF_Font* m_font;//font chữ hiển thị

    Snake m_snake;//đối tượng con rắn
    Food m_food;//thức ăn
    GameState m_gameState;//trạng thái game

    int m_score;//điểm số
    bool m_running;//kiểm tra game đag chạy
};

#endif
