#include "Game.h"
#include "HighScoreManager.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include "TextureManager.h"

Game::Game() :
    m_window(nullptr),
    m_renderer(nullptr),
    m_font(nullptr),
    m_gameState(MENU),
    m_score(0),
    m_running(true),
    m_food(GRID_WIDTH, GRID_HEIGHT) {
    // Khởi tạo vector chứa IDs và paths
    m_snakeHeadTextureIds = {"head_up", "head_down", "head_left", "head_right"};
    m_snakeBodyTextureIds = {"body_horizontal", "body_vertical", "body_topleft", "body_topright", "body_bottomleft", "body_bottomright"};
    m_snakeTailTextureIds = {"tail_up", "tail_down", "tail_left", "tail_right"}; // Thêm

    m_snakeHeadTexturePaths = {"head_up.png", "head_down.png", "head_left.png", "head_right.png"};
    m_snakeBodyTexturePaths = {"body_horizontal.png", "body_vertical.png", "body_topleft.png", "body_topright.png", "body_bottomleft.png", "body_bottomright.png"};
    m_snakeTailTexturePaths = {"tail_up.png", "tail_down.png", "tail_left.png", "tail_right.png"}; // Thêm
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    m_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    m_font = TTF_OpenFont("font.ttf", 24);
    if (!m_font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return false;
    }

    if (!TextureManager::Instance()->load("background.png", "background", m_renderer)) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return false;
    }
    if (!TextureManager::Instance()->load("normalfood.png", "ball", m_renderer)) {
        std::cerr << "Failed to load ball texture!" << std::endl;
        return false;
    }

    if (!TextureManager::Instance()->load("speedfood.png", "speed_food", m_renderer)) { // <<< THAY "assets/red_ball.png" bằng đường dẫn thực tế của bạn
        std::cerr << "Failed to load speed_food texture!" << std::endl;
        // return false; // Hoặc xử lý lỗi
    }

    int pauseButtonSize = 40; // Kích thước nút Pause
    int padding = 10;         // Khoảng cách từ mép màn hình
    m_pauseButtonRect = {SCREEN_WIDTH - pauseButtonSize - padding, padding, pauseButtonSize, pauseButtonSize};

    loadPauseButtonTexture();

    loadTextures(); // Load texture ở đây

    return true;
}

void Game::loadPauseButtonTexture() {
    if (!TextureManager::Instance()->load("assets/pause_icon.png", m_pauseIconID, m_renderer)) { // Thay "assets/pause_icon.png" bằng đường dẫn đúng
        std::cerr << "Failed to load pause icon texture!" << std::endl;
    }
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (m_running) {
        handleEvents();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        update(deltaTime);
        render();
        SDL_Delay(16);
        // Cap at ~60 FPS
    }
}

void Game::clean() {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_running = false;
        }

        // Xử lý click chuột
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = {mouseX, mouseY};

            if (m_gameState == PLAYING) {
                // Kiểm tra click vào nút Pause trên màn hình
                if (SDL_PointInRect(&mousePoint, &m_pauseButtonRect)) {
                    m_gameState = PAUSED;
                    // AudioManager::Instance()->playSound("pause_sound", 0); // Ví dụ âm thanh khi pause
                }
            } else if (m_gameState == PAUSED) {
                // Kiểm tra click vào các nút trong Pause Menu
                if (SDL_PointInRect(&mousePoint, &m_resumeButtonMenuRect)) {
                    m_gameState = PLAYING;
                    // AudioManager::Instance()->playSound("resume_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_replayButtonMenuRect)) {
                    resetGame(); // Hàm reset game bạn đã có
                    m_gameState = PLAYING;
                    // AudioManager::Instance()->playSound("replay_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_optionsButtonMenuRect)) {
                    // Xử lý khi click nút Options (ví dụ: mở màn hình options)
                    std::cout << "Options button clicked!" << std::endl;
                    // Hiện tại chưa có màn hình options, có thể thêm sau
                }
            }
        }

        // Xử lý bàn phím (có thể giữ lại phím P để pause/resume nếu muốn)
        if (event.type == SDL_KEYDOWN) {
            switch (m_gameState) {
                case MENU:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        m_gameState = PLAYING;
                    }
                    break;
                case PLAYING:
                    m_snake.handleInput(event); // Xử lý input cho rắn
                    if (event.key.keysym.sym == SDLK_p) { // Phím P để pause
                        m_gameState = PAUSED;
                    }
                    break;
                case PAUSED:
                    if (event.key.keysym.sym == SDLK_p) { // Phím P để resume
                        m_gameState = PLAYING;
                    }
                    // Có thể thêm các phím khác để điều khiển menu tạm dừng (lên, xuống, enter)
                    break;
                case GAME_OVER:
                    if (event.key.keysym.sym == SDLK_r) {
                        resetGame();
                        m_gameState = PLAYING;
                    } else if (event.key.keysym.sym == SDLK_m) { // Ví dụ: Quay về MENU chính
                        m_score = 0; // Reset điểm nếu cần
                        m_snake.reset();
                        m_food.generateFood();
                        m_gameState = MENU;
                    }
                    break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (m_gameState == GAME_OVER) {
        return;
    }

    if (m_gameState != PLAYING) return;

    m_snake.update(); // 1. Rắn di chuyển
    if (m_snake.checkCollision()) {
            m_gameState = GAME_OVER;
            HighScoreManager::Instance()->addScore(m_score);
        }
    m_food.update();

    if (m_food.isActive() && m_snake.checkFoodCollision(m_food.getPosition())) {
        bool foodWasSpeedBoost = (m_food.getType() == SPEED_BOOST);
        FoodType typeOfEatenFood = m_food.getType();
        m_snake.grow();
        m_score += foodWasSpeedBoost ? 20 : 10;
        if (foodWasSpeedBoost) {
            m_snake.increaseSpeed();
        }
        if (rand() % 5 == 0) {
            m_food.generateSpeedFood();
        } else {
            m_food.generateFood();
        }

    }
}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255); // Màu nền đen (hoặc màu của bạn)
    SDL_RenderClear(m_renderer);

    // Vẽ background game (nếu có)
    TextureManager::Instance()->draw("background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, m_renderer);

    switch (m_gameState) {
        case MENU:
            renderMenu(); // Hàm vẽ menu chính của bạn
            break;
        case PLAYING:
            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderScore();
            renderSpeedBoostBar(); // Nếu có
            renderPauseButton();   // Vẽ nút Pause
            break;
        case PAUSED:
            // Khi paused, vẫn vẽ lại trạng thái game phía sau menu cho đẹp
            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton(); // Vẫn vẽ nút Pause (có thể làm mờ hoặc đổi icon)
            renderPauseMenu();   // Vẽ menu tạm dừng lên trên
            break;
        case GAME_OVER:
            renderGameOver(); // Hàm vẽ màn hình game over của bạn
            break;
    }
    SDL_RenderPresent(m_renderer);
}

void Game::renderMenu() {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, "SNAKE GAME", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_Rect rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    surface = TTF_RenderText_Solid(m_font, "Press ENTER to Start", white);
    texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    rect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 50, 300, 30};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderPauseButton() {
    if (!TextureManager::Instance()->draw(m_pauseIconID, m_pauseButtonRect.x, m_pauseButtonRect.y, m_pauseButtonRect.w, m_pauseButtonRect.h, m_renderer)) {
         // Vẽ hình chữ nhật thay thế nếu không draw được (hoặc texture không tồn tại)
         SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255); // Màu xám
         SDL_RenderFillRect(m_renderer, &m_pauseButtonRect);
        // Vẽ 2 vạch đứng của biểu tượng Pause
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255); // Màu trắng
        int barWidth = m_pauseButtonRect.w / 6;
        int barHeight = m_pauseButtonRect.h / 2;
        int barPadding = m_pauseButtonRect.w / 5;
        SDL_Rect bar1 = {m_pauseButtonRect.x + barPadding, m_pauseButtonRect.y + (m_pauseButtonRect.h - barHeight) / 2, barWidth, barHeight};
        SDL_Rect bar2 = {m_pauseButtonRect.x + m_pauseButtonRect.w - barPadding - barWidth, m_pauseButtonRect.y + (m_pauseButtonRect.h - barHeight) / 2, barWidth, barHeight};
        SDL_RenderFillRect(m_renderer, &bar1);
        SDL_RenderFillRect(m_renderer, &bar2);
    }
}

void Game::renderGameOver() {
    SDL_Color white = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(m_score);
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_Rect rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    surface = TTF_RenderText_Solid(m_font, "Press R to Restart", white);
    texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 30};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderScore() {
    SDL_Color white = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(m_score);
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_Rect rect = {10, 10, 150, 30};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderSpeedBoostBar() {
    if (!m_snake.isSpeedBoosted()) return;
    Uint32 boostTime = SDL_GetTicks() - m_snake.getSpeedBoostTimer();
    float percent = 1.0f - (boostTime / 3000.0f); // 3 seconds duration
    SDL_Rect outline = {SCREEN_WIDTH - 210, 10, 200, 20};
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(m_renderer, &outline);
    SDL_Rect fill = {SCREEN_WIDTH - 208, 12, static_cast<int>(196 * percent), 16};
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(m_renderer, &fill);
}

void Game::resetGame() {
    m_snake.reset();
    m_score = 0;
    m_food.generateFood();;
}

void Game::loadTextures() {
    // Load snake textures
    TextureManager* tm = TextureManager::Instance();

    for (size_t i = 0; i < m_snakeHeadTextureIds.size(); ++i) {
        if (!tm->load(m_snakeHeadTexturePaths[i], m_snakeHeadTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeHeadTexturePaths[i] << std::endl;
        }
    }
    for (size_t i = 0; i < m_snakeBodyTextureIds.size(); ++i) {
        if (!tm->load(m_snakeBodyTexturePaths[i], m_snakeBodyTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeBodyTexturePaths[i] << std::endl;
        }
    }
    for (size_t i = 0; i < m_snakeTailTextureIds.size(); ++i) {
        if (!tm->load(m_snakeTailTexturePaths[i], m_snakeTailTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeTailTexturePaths[i] << std::endl;
        }
    }

    // Set texture IDs for Snake
    m_snake.headTextureIds = m_snakeHeadTextureIds;
    m_snake.bodyTextureIds = m_snakeBodyTextureIds;
    m_snake.tailTextureIds = m_snakeTailTextureIds;
}
// Hàm tiện ích để vẽ chữ (bạn có thể đặt ở đâu đó dùng chung hoặc trong Game.cpp)
void renderTextOnButtonHelper(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, const SDL_Rect& buttonRect, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect textRect;
    textRect.w = surface->w;
    textRect.h = surface->h;
    textRect.x = buttonRect.x + (buttonRect.w - textRect.w) / 2;
    textRect.y = buttonRect.y + (buttonRect.h - textRect.h) / 2;
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


void Game::renderPauseMenu() {
    // Màu sắc (ví dụ)
    SDL_Color menuBackgroundColor = {30, 30, 50, 210};  // Xám xanh đậm, hơi trong suốt
    SDL_Color buttonColor = {70, 80, 100, 255};      // Xanh nhạt cho nút
    SDL_Color buttonBorderColor = {120, 130, 150, 255}; // Viền nút
    SDL_Color textColor = {220, 220, 220, 255};      // Chữ màuเกือบ trắng

    // Kích thước và vị trí menu (ví dụ, căn giữa màn hình)
    int menuWidth = 350;
    int menuHeight = 300;
    SDL_Rect menuRect = { (SCREEN_WIDTH - menuWidth) / 2, (SCREEN_HEIGHT - menuHeight) / 2, menuWidth, menuHeight };

    // Vẽ nền menu
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND); // Cho phép alpha blending
    SDL_SetRenderDrawColor(m_renderer, menuBackgroundColor.r, menuBackgroundColor.g, menuBackgroundColor.b, menuBackgroundColor.a);
    SDL_RenderFillRect(m_renderer, &menuRect);

    // Vẽ viền menu (tùy chọn)
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, 255);
    SDL_RenderDrawRect(m_renderer, &menuRect);

    // Tiêu đề "PAUSED"
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(m_font, "PAUSED", textColor);
    if (surfaceTitle) {
        SDL_Texture* textureTitle = SDL_CreateTextureFromSurface(m_renderer, surfaceTitle);
        if (textureTitle) {
            SDL_Rect titleRect = { menuRect.x + (menuRect.w - surfaceTitle->w) / 2 , menuRect.y + 20, surfaceTitle->w, surfaceTitle->h };
            SDL_RenderCopy(m_renderer, textureTitle, NULL, &titleRect);
            SDL_DestroyTexture(textureTitle);
        }
        SDL_FreeSurface(surfaceTitle);
    }


    // Thông số nút
    int buttonWidth = menuWidth - 60;
    int buttonHeight = 50;
    int buttonTopPadding = 80; // Khoảng cách từ top menu đến nút đầu tiên
    int buttonSpacing = 15;   // Khoảng cách giữa các nút

    // Nút "Resume"
    m_resumeButtonMenuRect = {
        menuRect.x + (menuWidth - buttonWidth) / 2,
        menuRect.y + buttonTopPadding,
        buttonWidth,
        buttonHeight
    };
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_resumeButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_resumeButtonMenuRect);
    renderTextOnButtonHelper(m_renderer, m_font, "RESUME", m_resumeButtonMenuRect, textColor);

    // Nút "Replay"
    m_replayButtonMenuRect = {
        m_resumeButtonMenuRect.x,
        m_resumeButtonMenuRect.y + buttonHeight + buttonSpacing,
        buttonWidth,
        buttonHeight
    };
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_replayButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_replayButtonMenuRect);
    renderTextOnButtonHelper(m_renderer, m_font, "REPLAY", m_replayButtonMenuRect, textColor);

    // Nút "Options"
    m_optionsButtonMenuRect = {
        m_replayButtonMenuRect.x,
        m_replayButtonMenuRect.y + buttonHeight + buttonSpacing,
        buttonWidth,
        buttonHeight
    };
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_optionsButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_optionsButtonMenuRect);
    renderTextOnButtonHelper(m_renderer, m_font, "OPTIONS", m_optionsButtonMenuRect, textColor);
}
