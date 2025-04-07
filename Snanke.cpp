#include "Snake.h"  // Include file header để sử dụng cấu trúc Snake và các hàm liên quan
#include <cstdlib>  // Thư viện cho hàm rand() và srand()
#include <ctime>    // Thư viện cho hàm time()

// Kích thước mỗi ô lưới và kích thước màn hình
const int GRID_SIZE = 20;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Hàm khởi tạo con rắn
void initializeSnake(Snake& snake) {
    // Đặt vị trí ban đầu của con rắn ở giữa màn hình
    snake.body = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}};
    // Đặt hướng di chuyển ban đầu là sang phải
    snake.direction = {1, 0};
}

// Hàm cập nhật trạng thái của con rắn
void updateSnake(Snake& snake, bool& running, SDL_Point& food) {
    // Tính toán vị trí mới của đầu rắn dựa trên hướng di chuyển
    SDL_Point newHead = {
        snake.body[0].x + snake.direction.x * GRID_SIZE,
        snake.body[0].y + snake.direction.y * GRID_SIZE
    };

    // Kiểm tra va chạm với tường
    if (newHead.x < 0 || newHead.x >= SCREEN_WIDTH || newHead.y < 0 || newHead.y >= SCREEN_HEIGHT) {
        running = false;  // Nếu va chạm, kết thúc trò chơi
        return;
    }

    // Kiểm tra va chạm với chính nó
    for (const auto& segment : snake.body) {
        if (segment.x == newHead.x && segment.y == newHead.y) {
            running = false;  // Nếu va chạm, kết thúc trò chơi
            return;
        }
    }

    // Thêm đầu mới vào đầu vector body
    snake.body.insert(snake.body.begin(), newHead);

    // Kiểm tra xem đầu rắn có chạm vào thức ăn không
    if (newHead.x == food.x && newHead.y == food.y) {
        // Nếu chạm vào thức ăn, đặt lại thức ăn ở vị trí ngẫu nhiên
        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    } else {
        // Nếu không chạm vào thức ăn, bỏ đi phần đuôi
        snake.body.pop_back();
    }
}

// Hàm vẽ con rắn lên màn hình
void renderSnake(SDL_Renderer* renderer, const Snake& snake) {
    // Đặt màu vẽ là màu xanh lá cây
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    // Vẽ từng phần của con rắn
    for (const auto& segment : snake.body) {
        // Tạo hình chữ nhật cho mỗi phần thân
        SDL_Rect rect = {segment.x, segment.y, GRID_SIZE, GRID_SIZE};
        // Vẽ hình chữ nhật lên màn hình
        SDL_RenderFillRect(renderer, &rect);
    }
}
