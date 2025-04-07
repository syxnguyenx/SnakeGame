#include "Snake.h"
#include <cstdlib>
#include <ctime>

const int GRID_SIZE = 20;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void initializeSnake(Snake& snake) {
    snake.body = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}}; //phan than con ran dat giua man hinh

    snake.direction = {1, 0};//huong di chuyen ban dau cua con ran laf sang phai

}

void updateSnake(Snake& snake, bool& running, SDL_Point& food) {
    SDL_Point newHead = {snake.body[0].x + snake.direction.x * GRID_SIZE, snake.body[0].y + snake.direction.y * GRID_SIZE};//cap nhat vi tri dau moi cua con ran

    //kiem tra va cham voi tuong
    if(newHead.x < 0 || newHead.x >= SCREEN_WIDTH || newHead.y < 0 || newHead.y >= SCREEN_HEIGHT) {
        running = false;
        return;
    }
    //kiem tra va cham voi chinh no
    for(auto& segment : snake.body) {
        if(segment.x == newHead.x || segment.y == newHead.y) {
            running = false;
            return;
        }
    }
    //them dau moi vao than con ran
    snake.body.insert(snake.body.begin(), newHead);

    //kiem tra xem con ran co an moi hay khong
    if(newHead.x == food.x || newHead.y == food.y) {
            //cap nhat thuc an o vi tri ngau nhien
        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    }else{
        //neu khong cham thuc an bor di phan duoi
        snake.body.pop_back();
    }
}
void renderSnake(SDL_Renderer* renderer, const Snake& snake) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);//mau ve con ran laf xanh la
    for(const auto& segment : snake.body) {
        SDL_Rect rect = {segment.x, segment.y, GRID_SIZE, GRID_SIZE};//tao hinh chu nhat cho moi phan than
        SDL_RenderFillRect(renderer, &rect);// ve hinh chu nhat len con ran
    }

}

