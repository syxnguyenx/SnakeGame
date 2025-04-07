#include "Game.h"
#include "Snake.h"
#include <SDL.h>
#include <iostream>
#include <ctime>

const int GRID_SIZE = 20;  // Kích thước mỗi ô lưới




int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));  // Khởi tạo seed cho hàm rand

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    initialize(window, renderer);  // Khởi tạo SDL và tạo cửa sổ, renderer

    Snake snake;
    initializeSnake(snake);  // Khởi tạo con rắn

    SDL_Point food;
    placeFood(food, snake);  // Đặt thức ăn ở vị trí ngẫu nhiên

    bool running = true;
    SDL_Event e;

    // Vòng lặp chính của trò chơi
    while (running) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;  // Nếu nhấn nút đóng cửa sổ, kết thúc trò chơi
            } else if (e.type == SDL_KEYDOWN) {
                // Thay đổi hướng di chuyển của con rắn dựa trên phím mũi tên
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.direction.y == 0) snake.direction = {0, -1};  // Di chuyển lên
                        break;
                    case SDLK_DOWN:
                        if (snake.direction.y == 0) snake.direction = {0, 1};  // Di chuyển xuống
                        break;
                    case SDLK_LEFT:
                        if (snake.direction.x == 0) snake.direction = {-1, 0};  // Di chuyển sang trái
                        break;
                    case SDLK_RIGHT:
                        if (snake.direction.x == 0) snake.direction = {1, 0};  // Di chuyển sang phải
                        break;
                }
            }
        }

        updateSnake(snake, running, food);  // Cập nhật trạng thái con rắn

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Đặt màu nền là màu đen
        SDL_RenderClear(renderer);  // Xóa màn hình

        renderSnake(renderer, snake);  // Vẽ con rắn
        renderFood(renderer, food);  // Vẽ thức ăn

        SDL_RenderPresent(renderer);  // Hiển thị màn hình
        SDL_Delay(100);  // Tạm dừng 100ms để làm chậm trò chơi
    }

    close(window, renderer);  // Giải phóng tài nguyên và đóng SDL
    return 0;
}
