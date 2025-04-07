#include "Game.h"
#include <cstdlib>
#include <ctime>

const int GRID_SIZE = 20;  // Kích thước mỗi ô lưới

void initialize(SDL_Window*& window, SDL_Renderer*& renderer) {
    SDL_Init(SDL_INIT_VIDEO);//khoi tao SDL
    window = SDL_CreateWindow("Snake game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);//tao cua so
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);//tao renderer
}
void close(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);//huy renderer
    SDL_DestroyWindow(window);//huy cua so
    SDL_Quit();//dong SDL
}
void placeFood(SDL_Point& food, const Snake& snake) {
    bool onSnake;
    do {
        onSnake = false;
        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;//tao thuc an ngau nhien tren x
        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;// tao thuc an ngau nhien tren y
        for(const auto& segment : snake.body) {
            if(segment.x == food.x || segment.y == food.y) {
                onSnake = true;
                break;
            }
        }
    }while(onSnake);
}
void renderFood(SDL_Renderer* renderer, const SDL_Point& food) {
     SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);//thuc an mau do
     SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};//tao hinhf chu nhat cho thuc an
     SDL_RenderFillRect(renderer, &foodRect);//ve hinh chu nhat cho thuc an
}
