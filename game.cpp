#include "Game.h"
#include "HighScoreManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "Constants.h"
#include "Food.h" // Thêm nếu Food.h định nghĩa SPEED_BOOST, BALL

#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm> // Cho std::min, std::max
#include <vector>

// --- HÀM HELPER ĐỂ VẼ CHỮ LÊN NÚT ---
// Đảm bảo hàm này được khai báo trong Game.h là private member:
// void renderTextOnButtonHelper(const std::string& text, const SDL_Rect& buttonRect, SDL_Color color);
// Và đây là định nghĩa của nó:
void Game::renderTextOnButtonHelper(const std::string& text, const SDL_Rect& buttonRect, SDL_Color color) {
    if (!m_font || !m_renderer) {
        std::cerr << "Loi: m_font hoac m_renderer chua duoc khoi tao trong renderTextOnButtonHelper!" << std::endl;
        return;
    }
    if (text.empty()) {
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(m_font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Loi TTF_RenderText_Solid cho text \"" << text << "\": " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface); // Giải phóng surface ngay sau khi tạo texture

    if (!texture) {
        std::cerr << "Loi SDL_CreateTextureFromSurface cho text \"" << text << "\": " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect textRect;
    textRect.w = surface->w; // Lấy width từ surface trước khi giải phóng (hoặc dùng TTF_SizeText)
    textRect.h = surface->h; // Lấy height từ surface trước khi giải phóng
    TTF_SizeText(m_font, text.c_str(), &textRect.w, &textRect.h); // Cách chính xác hơn để lấy kích thước

    textRect.x = buttonRect.x + (buttonRect.w - textRect.w) / 2;
    textRect.y = buttonRect.y + (buttonRect.h - textRect.h) / 2;

    SDL_RenderCopy(m_renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture); // Giải phóng texture sau khi vẽ
}


Game::Game() :
    m_window(nullptr),
    m_renderer(nullptr),
    m_font(nullptr),
    m_food(GRID_WIDTH, GRID_HEIGHT),
    m_gameState(MENU),
    m_score(0),
    m_running(true),        // Khởi tạo m_running ở đây
    m_currentMusicVolume(80),
    m_currentSfxVolume(80),
    m_pauseIconID("pause_icon") // Khởi tạo ID cho icon pause
{
    std::cout << "Game constructor called." << std::endl;
    m_snakeHeadTextureIds = {"head_up", "head_down", "head_left", "head_right"};
    m_snakeBodyTextureIds = {"body_horizontal", "body_vertical", "body_topleft", "body_topright", "body_bottomleft", "body_bottomright"};
    m_snakeTailTextureIds = {"tail_up", "tail_down", "tail_left", "tail_right"};

    m_snakeHeadTexturePaths = {"head_up.png", "head_down.png", "head_left.png", "head_right.png"};
    m_snakeBodyTexturePaths = {"body_horizontal.png", "body_vertical.png", "body_topleft.png", "body_topright.png", "body_bottomleft.png", "body_bottomright.png"};
    m_snakeTailTexturePaths = {"tail_up.png", "tail_down.png", "tail_left.png", "tail_right.png"};
}

Game::~Game() {
    std::cout << "Game destructor called." << std::endl;
    clean();
}

bool Game::init() {
    std::cout << "Game::init() started." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    m_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }
    m_font = TTF_OpenFont("font.ttf", 24);
    if (!m_font) {
        std::cerr << "Failed to load font! Path: font.ttf. Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    if (!AudioManager::Instance()) {
         std::cerr << "Failed to initialize AudioManager!" << std::endl;
         return false;
    }
    AudioManager::Instance()->setGlobalVolume(m_currentMusicVolume, m_currentSfxVolume);
    if (!TextureManager::Instance()->load("background.png", "background", m_renderer)) {
        std::cerr << "Failed to load background texture! Path: background.png" << std::endl;
    }
    if (!TextureManager::Instance()->load("normalfood.png", "ball", m_renderer)) {
        std::cerr << "Failed to load ball texture! Path: normalfood.png" << std::endl;
    }
    if (!TextureManager::Instance()->load("speedfood.png", "speed_food", m_renderer)) { // Tên file quả bóng đỏ của bạn
        std::cerr << "Failed to load speed_food texture! Path: speedfood.png" << std::endl;
    }

    // Load textures cho nút mute/unmute (BỎ "assets/")
    if (!TextureManager::Instance()->load("Mute Button unmuted1.png", "sound_on_icon", m_renderer)) {
        std::cerr << "Failed to load sound_on_icon texture! Path: Mute Button unmuted1.png" << std::endl;
    }
    if (!TextureManager::Instance()->load("Mute Button muted1.png", "sound_off_icon", m_renderer)) {
        std::cerr << "Failed to load sound_off_icon texture! Path: Mute Button muted1.png" << std::endl;
    }

    loadPauseButtonTexture();

    // Load âm thanh
    if(!AudioManager::Instance()->loadMusic("background_music.mp3", "bg_music")){
         std::cerr << "Failed to load music: background_music.mp3" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound("eat_food.wav", "eat_sound")){ // Nhớ dùng .wav hoặc .ogg
         std::cerr << "Failed to load sound: eat_food.wav" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound("game_over.wav", "gameover_sound")){ // Nhớ dùng .wav hoặc .ogg
         std::cerr << "Failed to load sound: game_over.wav" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound("click.wav", "click_sound")){
         std::cerr << "Failed to load sound: click.wav" << std::endl;
    }

    AudioManager::Instance()->playMusic("bg_music", -1);

    int buttonSize = 40;
    int padding = 10; // Khai báo padding MỘT LẦN ở đây
    m_muteButtonRect_MainMenu = {SCREEN_WIDTH - buttonSize - padding, padding, buttonSize, buttonSize};
    m_pauseButtonRect = {SCREEN_WIDTH - buttonSize - padding, padding, buttonSize, buttonSize};

    loadTextures();

    std::cout << "Game::init() finished successfully." << std::endl;
    return true;
}

void Game::loadPauseButtonTexture() {
    // Sử dụng m_pauseIconID đã được khai báo và khởi tạo trong Game.h/Game.cpp
    if (!TextureManager::Instance()->load("assets/pause_icon.png", m_pauseIconID, m_renderer)) {
        std::cerr << "Failed to load pause icon texture! Path: assets/pause_icon.png" << std::endl;
    }
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (m_running) {
        handleEvents();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        if (deltaTime <= 0.0f) { // Tránh deltaTime bằng 0 hoặc âm
            deltaTime = 0.016f; // Giá trị fallback nhỏ
        }
        lastTime = currentTime;
        update(deltaTime);
        render();
        SDL_Delay(16);
    }
}

void Game::clean() {
    std::cout << "Game::clean() called." << std::endl;
    TextureManager::Instance()->clearTextureMap();
    // AudioManager sẽ tự dọn dẹp trong destructor của nó nếu được thiết kế đúng (Singleton có destructor)
    // Hoặc bạn có thể thêm AudioManager::Instance()->clean();

    if (m_font) { TTF_CloseFont(m_font); m_font = nullptr; }
    if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
    if (m_window) { SDL_DestroyWindow(m_window); m_window = nullptr; }

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

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = {mouseX, mouseY};

            if (m_gameState == PLAYING) {
                if (SDL_PointInRect(&mousePoint, &m_pauseButtonRect)) {
                    m_gameState = PAUSED;
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == PAUSED) {
                if (SDL_PointInRect(&mousePoint, &m_resumeButtonMenuRect)) {
                    m_gameState = PLAYING;
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_replayButtonMenuRect)) {
                    resetGame();
                    // m_gameState = PLAYING; // resetGame đã làm việc này
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_optionsButtonMenuRect)) {
                    m_gameState = OPTIONS_MENU;
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == OPTIONS_MENU) {
                int volumeStep = 10;
                if (SDL_PointInRect(&mousePoint, &m_musicVolUpButtonRect)) {
                    m_currentMusicVolume = std::min(100, m_currentMusicVolume + volumeStep);
                    AudioManager::Instance()->setMusicVolume(m_currentMusicVolume * MIX_MAX_VOLUME / 100);
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_musicVolDownButtonRect)) {
                    m_currentMusicVolume = std::max(0, m_currentMusicVolume - volumeStep);
                    AudioManager::Instance()->setMusicVolume(m_currentMusicVolume * MIX_MAX_VOLUME / 100);
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_sfxVolUpButtonRect)) {
                    m_currentSfxVolume = std::min(100, m_currentSfxVolume + volumeStep);
                    AudioManager::Instance()->setSoundVolume(m_currentSfxVolume * MIX_MAX_VOLUME / 100);
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_sfxVolDownButtonRect)) {
                    m_currentSfxVolume = std::max(0, m_currentSfxVolume - volumeStep);
                    AudioManager::Instance()->setSoundVolume(m_currentSfxVolume * MIX_MAX_VOLUME / 100);
                    AudioManager::Instance()->playSound("click_sound", 0);
                } else if (SDL_PointInRect(&mousePoint, &m_optionsBackButtonRect)) {
                    m_gameState = PAUSED;
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == GAME_OVER) {
                if (SDL_PointInRect(&mousePoint, &m_gameOverRestartButtonRect)) {
                    resetGame();
                    // m_gameState = PLAYING; // resetGame đã làm việc này
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == MENU) {
                if (SDL_PointInRect(&mousePoint, &m_muteButtonRect_MainMenu)) {
                    AudioManager::Instance()->toggleMuteAll();
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
                // Thêm xử lý click cho nút Start trực quan (nếu có)
            }
        }

        if (event.type == SDL_KEYDOWN) {
            switch (m_gameState) {
                case MENU:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        m_gameState = PLAYING;
                        AudioManager::Instance()->playSound("start_game_sound",0);
                    }
                    break;
                case PLAYING:
                    if (event.key.keysym.sym == SDLK_p) {
                        m_gameState = PAUSED;
                        AudioManager::Instance()->playSound("pause_sound",0);
                    } else {
                        m_snake.handleInput(event);
                    }
                    break;
                case PAUSED:
                    if (event.key.keysym.sym == SDLK_p) {
                        m_gameState = PLAYING;
                        AudioManager::Instance()->playSound("resume_sound",0);
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                         m_gameState = OPTIONS_MENU;
                         AudioManager::Instance()->playSound("click_sound",0);
                    }
                    break;
                case GAME_OVER:
                    if (event.key.keysym.sym == SDLK_m) {
                        resetGame();
                        m_gameState = MENU; // Chuyển về MENU sau khi reset từ Game Over bằng phím M
                        AudioManager::Instance()->playSound("click_sound",0);
                    }
                    break;
                case OPTIONS_MENU:
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_BACKSPACE) {
                        m_gameState = PAUSED;
                        AudioManager::Instance()->playSound("click_sound",0);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (m_gameState == GAME_OVER) {
        return;
    }
    if (m_gameState != PLAYING) {
        return;
    }

    m_snake.update(); // Truyền deltaTime nếu bạn muốn Snake::update() nhận nó

    if (m_snake.checkCollision()) {
        m_gameState = GAME_OVER;
        AudioManager::Instance()->playSound("gameover_sound", 0);
        HighScoreManager::Instance()->addScore(m_score);
        return;
    }

    m_food.update();

    if (m_food.isActive() && m_snake.checkFoodCollision(m_food.getPosition())) {
        bool foodWasSpeedBoost = (m_food.getType() == SPEED_BOOST);
        // FoodType typeOfEatenFood = m_food.getType(); // Bị warning unused, có thể bỏ nếu không dùng để in log

        m_snake.grow();
        m_score += foodWasSpeedBoost ? 20 : 10;

        if (foodWasSpeedBoost) {
            m_snake.increaseSpeed();
        }
        AudioManager::Instance()->playSound("eat_sound", 0);

        if (rand() % 5 == 0) {
            m_food.generateSpeedFood();
        } else {
            m_food.generateFood();
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 144, 238, 144, 255);
    SDL_RenderClear(m_renderer);

    // TextureManager::Instance()->draw("background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, m_renderer); // Bỏ comment nếu muốn dùng ảnh nền

    switch (m_gameState) {
        case MENU:
            renderMenu();
            break;
        case PLAYING:
            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton();
            break;
        case PAUSED:
            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton();
            renderPauseMenu();
            break;
        case GAME_OVER:
            renderScore();
            renderGameOver();
            break;
        case OPTIONS_MENU: // <<<< ĐÃ THÊM CASE NÀY
            m_food.render(m_renderer); // Hoặc chỉ vẽ một nền mờ cho Options
            m_snake.render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton();
            renderOptionsMenu();
            break;
        default:
            break;
    }
    SDL_RenderPresent(m_renderer);
}

void Game::renderMenu() {
    SDL_Color black = {0,0,0,255};

    SDL_Surface* surface = TTF_RenderText_Solid(m_font, "SNAKE GAME", black);
    SDL_Texture* texture = nullptr;
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect rect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 4, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        if (texture) SDL_DestroyTexture(texture);
    }

    surface = TTF_RenderText_Solid(m_font, "Press ENTER to Start", black);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect rect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 2 + 50, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        if (texture) SDL_DestroyTexture(texture);
    }

    std::string iconToDrawID = AudioManager::Instance()->isMuted() ? "sound_off_icon" : "sound_on_icon";
    if (!TextureManager::Instance()->draw(iconToDrawID, m_muteButtonRect_MainMenu.x, m_muteButtonRect_MainMenu.y, m_muteButtonRect_MainMenu.w, m_muteButtonRect_MainMenu.h, m_renderer)) {
        // Vẽ hình chữ nhật thay thế nếu icon không load được
        SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(m_renderer, &m_muteButtonRect_MainMenu);
    }
}

void Game::renderPauseButton() {
    if (!TextureManager::Instance()->draw(m_pauseIconID, m_pauseButtonRect.x, m_pauseButtonRect.y, m_pauseButtonRect.w, m_pauseButtonRect.h, m_renderer)) {
        SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(m_renderer, &m_pauseButtonRect);
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
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
    SDL_Color black = {0,0,0,255};
    SDL_Color buttonColor = {70, 80, 100, 255};
    SDL_Color buttonBorderColor = {120, 130, 150, 255};

    SDL_Surface* surface = TTF_RenderText_Solid(m_font, "GAME OVER", black);
    SDL_Texture* texture = nullptr;
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect titleRect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 3, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &titleRect);
        SDL_FreeSurface(surface);
        if (texture) SDL_DestroyTexture(texture);
    }

    std::string scoreText = "Score: " + std::to_string(m_score);
    surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), black);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect scoreRect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 2 - 30, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &scoreRect);
        SDL_FreeSurface(surface);
        if (texture) SDL_DestroyTexture(texture);
    }

    int buttonWidth = 200;
    int buttonHeight = 50;
    m_gameOverRestartButtonRect.x = (SCREEN_WIDTH - buttonWidth) / 2;
    m_gameOverRestartButtonRect.y = SCREEN_HEIGHT / 2 + 40;
    m_gameOverRestartButtonRect.w = buttonWidth;
    m_gameOverRestartButtonRect.h = buttonHeight;

    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_gameOverRestartButtonRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_gameOverRestartButtonRect);
    renderTextOnButtonHelper("CHOI LAI", m_gameOverRestartButtonRect, white);
}

void Game::renderScore() {
    SDL_Color black = {0, 0, 0, 255};
    std::string scoreText = "Score: " + std::to_string(m_score);
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), black);
    if (!surface) { std::cerr << "Failed to render score text surface: " << TTF_GetError() << std::endl; return; }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface); // Giải phóng surface ngay sau khi tạo texture
    if (!texture) { std::cerr << "Failed to create score texture: " << SDL_GetError() << std::endl; return; }

    int textWidth = 0;
    int textHeight = 0;
    if (TTF_SizeText(m_font, scoreText.c_str(), &textWidth, &textHeight) == 0) {
        SDL_Rect rect = {10, 10, textWidth, textHeight};
        SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    } else {
        SDL_Rect rect = {10, 10, 100, 30};
        SDL_RenderCopy(m_renderer, texture, NULL, &rect);
    }
    SDL_DestroyTexture(texture); // Giải phóng texture sau khi vẽ
}

void Game::renderSpeedBoostBar() {
    if (!m_snake.isSpeedBoosted()) return;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 boostStartTime = m_snake.getSpeedBoostTimer();
    float elapsedBoostTime = static_cast<float>(currentTime - boostStartTime);
    float boostDuration = 3000.0f;

    if (elapsedBoostTime >= boostDuration) return;

    float percent = 1.0f - (elapsedBoostTime / boostDuration);
    if (percent < 0.0f) percent = 0.0f;
    // if (percent > 1.0f) percent = 1.0f; // Không cần thiết vì elapsedBoostTime >= 0

    int barHeight = 20;
    int barWidth = 200;
    int padding = 10; // Khai báo padding cục bộ nếu chưa có ở phạm vi class
    SDL_Rect outline = {SCREEN_WIDTH - barWidth - padding, SCREEN_HEIGHT - barHeight - padding, barWidth, barHeight};
    SDL_SetRenderDrawColor(m_renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(m_renderer, &outline);

    SDL_Rect fill = {outline.x + 2, outline.y + 2, static_cast<int>((barWidth - 4) * percent), barHeight - 4};
    SDL_SetRenderDrawColor(m_renderer, 255, 100, 100, 255);
    SDL_RenderFillRect(m_renderer, &fill);
}

void Game::resetGame() {
    std::cout << "Game::resetGame() called." << std::endl;
    m_snake.reset();
    m_score = 0;
    m_food.generateFood();
    m_gameState = PLAYING;
    std::cout << "Game::resetGame() finished." << std::endl;
}

void Game::loadTextures() {
    TextureManager* tm = TextureManager::Instance();

    for (size_t i = 0; i < m_snakeHeadTextureIds.size(); ++i) {
        // BỎ assetPath +
        if (!tm->load(m_snakeHeadTexturePaths[i], m_snakeHeadTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeHeadTexturePaths[i] << std::endl;
        }
    }
    for (size_t i = 0; i < m_snakeBodyTextureIds.size(); ++i) {
        // BỎ assetPath +
        if (!tm->load(m_snakeBodyTexturePaths[i], m_snakeBodyTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeBodyTexturePaths[i] << std::endl;
        }
    }
    for (size_t i = 0; i < m_snakeTailTextureIds.size(); ++i) {
        // BỎ assetPath +
        if (!tm->load(m_snakeTailTexturePaths[i], m_snakeTailTextureIds[i], m_renderer)) {
            std::cerr << "Failed to load: " << m_snakeTailTexturePaths[i] << std::endl;
        }
    }

    m_snake.headTextureIds = m_snakeHeadTextureIds;
    m_snake.bodyTextureIds = m_snakeBodyTextureIds;
    m_snake.tailTextureIds = m_snakeTailTextureIds;
}

void Game::renderPauseMenu() {
    SDL_Color menuBackgroundColor = {30, 30, 50, 210};
    SDL_Color buttonColor = {70, 80, 100, 255};
    SDL_Color buttonBorderColor = {120, 130, 150, 255};
    SDL_Color textColor = {220, 220, 220, 255};

    int menuWidth = 350;
    int menuHeight = 300;
    SDL_Rect menuRect = { (SCREEN_WIDTH - menuWidth) / 2, (SCREEN_HEIGHT - menuHeight) / 2, menuWidth, menuHeight };

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, menuBackgroundColor.r, menuBackgroundColor.g, menuBackgroundColor.b, menuBackgroundColor.a);
    SDL_RenderFillRect(m_renderer, &menuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, 255);
    SDL_RenderDrawRect(m_renderer, &menuRect);

    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(m_font, "PAUSED", textColor);
    SDL_Texture* textureTitle = nullptr;
    if (surfaceTitle) {
        textureTitle = SDL_CreateTextureFromSurface(m_renderer, surfaceTitle);
        if (textureTitle) {
            SDL_Rect titleRect = { menuRect.x + (menuRect.w - surfaceTitle->w) / 2 , menuRect.y + 20, surfaceTitle->w, surfaceTitle->h };
            SDL_RenderCopy(m_renderer, textureTitle, NULL, &titleRect);
            SDL_DestroyTexture(textureTitle);
        }
        SDL_FreeSurface(surfaceTitle);
    }

    int buttonWidth = menuWidth - 60;
    int buttonHeight = 50;
    int buttonTopPadding = 80;
    int buttonSpacing = 15;

    m_resumeButtonMenuRect.x = menuRect.x + (menuWidth - buttonWidth) / 2;
    m_resumeButtonMenuRect.y = menuRect.y + buttonTopPadding;
    m_resumeButtonMenuRect.w = buttonWidth;
    m_resumeButtonMenuRect.h = buttonHeight;
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_resumeButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_resumeButtonMenuRect);
    renderTextOnButtonHelper("RESUME", m_resumeButtonMenuRect, textColor);

    m_replayButtonMenuRect.x = m_resumeButtonMenuRect.x;
    m_replayButtonMenuRect.y = m_resumeButtonMenuRect.y + buttonHeight + buttonSpacing;
    m_replayButtonMenuRect.w = buttonWidth;
    m_replayButtonMenuRect.h = buttonHeight;
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_replayButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_replayButtonMenuRect);
    renderTextOnButtonHelper("REPLAY", m_replayButtonMenuRect, textColor);

    m_optionsButtonMenuRect.x = m_replayButtonMenuRect.x;
    m_optionsButtonMenuRect.y = m_replayButtonMenuRect.y + buttonHeight + buttonSpacing;
    m_optionsButtonMenuRect.w = buttonWidth;
    m_optionsButtonMenuRect.h = buttonHeight;
    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_optionsButtonMenuRect);
    SDL_SetRenderDrawColor(m_renderer, buttonBorderColor.r, buttonBorderColor.g, buttonBorderColor.b, buttonBorderColor.a);
    SDL_RenderDrawRect(m_renderer, &m_optionsButtonMenuRect);
    renderTextOnButtonHelper("OPTIONS", m_optionsButtonMenuRect, textColor);
}

void Game::renderOptionsMenu() {
    SDL_Color menuBgColor = {40, 40, 60, 230};
    SDL_Color btnColor = {80, 90, 110, 255};
    SDL_Color btnBorderColor = {130, 140, 160, 255};
    SDL_Color textColor = {230, 230, 230, 255};
    SDL_Color valueColor = {255, 220, 100, 255};

    int menuWidth = 400;
    int menuHeight = 300;
    SDL_Rect menuRect = {(SCREEN_WIDTH - menuWidth) / 2, (SCREEN_HEIGHT - menuHeight) / 2, menuWidth, menuHeight};

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, menuBgColor.r, menuBgColor.g, menuBgColor.b, menuBgColor.a);
    SDL_RenderFillRect(m_renderer, &menuRect);
    SDL_SetRenderDrawColor(m_renderer, btnBorderColor.r, btnBorderColor.g, btnBorderColor.b, 255);
    SDL_RenderDrawRect(m_renderer, &menuRect);

    SDL_Surface* surf = TTF_RenderText_Solid(m_font, "OPTIONS", textColor);
    SDL_Texture* tex = nullptr;
    if (surf) {
        tex = SDL_CreateTextureFromSurface(m_renderer, surf);
        if (tex) {
            SDL_Rect titleRect = {menuRect.x + (menuRect.w - surf->w) / 2, menuRect.y + 20, surf->w, surf->h};
            SDL_RenderCopy(m_renderer, tex, NULL, &titleRect);
            SDL_DestroyTexture(tex);
        }
        SDL_FreeSurface(surf);
    }

    int currentY = menuRect.y + 70;
    int labelX = menuRect.x + 30; // Tăng padding cho label
    int controlStartX = menuRect.x + 180; // Vị trí bắt đầu của giá trị và nút +/-
    int valueTextWidth = 70; // Chiều rộng ước lượng cho text giá trị âm lượng
    int plusMinusButtonSize = 35;
    int itemHeight = 40;
    int spacing = 10;

    // --- Music Volume ---
    std::string musicLabel = "Music Volume:";
    int labelTextW, labelTextH;
    TTF_SizeText(m_font, musicLabel.c_str(), &labelTextW, &labelTextH);
    // Vẽ label "Music Volume:"
    renderTextOnButtonHelper(musicLabel, {labelX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, textColor);

    std::string musicVolStr = std::to_string(m_currentMusicVolume) + "%";
    TTF_SizeText(m_font, musicVolStr.c_str(), &labelTextW, &labelTextH); // Lấy lại kích thước cho text giá trị
    // Vẽ giá trị âm lượng nhạc
    renderTextOnButtonHelper(musicVolStr, {controlStartX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, valueColor);

    // Nút giảm âm lượng nhạc "-"
    m_musicVolDownButtonRect.x = controlStartX + valueTextWidth + spacing;
    m_musicVolDownButtonRect.y = currentY + (itemHeight - plusMinusButtonSize)/2;
    m_musicVolDownButtonRect.w = plusMinusButtonSize;
    m_musicVolDownButtonRect.h = plusMinusButtonSize;
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_musicVolDownButtonRect);
    renderTextOnButtonHelper("-", m_musicVolDownButtonRect, textColor);

    // Nút tăng âm lượng nhạc "+"
    m_musicVolUpButtonRect.x = m_musicVolDownButtonRect.x + plusMinusButtonSize + spacing;
    m_musicVolUpButtonRect.y = currentY + (itemHeight - plusMinusButtonSize)/2;
    m_musicVolUpButtonRect.w = plusMinusButtonSize;
    m_musicVolUpButtonRect.h = plusMinusButtonSize;
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_musicVolUpButtonRect);
    renderTextOnButtonHelper("+", m_musicVolUpButtonRect, textColor);

    currentY += itemHeight + spacing;

    // --- SFX Volume ---
    std::string sfxLabel = "SFX Volume:";
    TTF_SizeText(m_font, sfxLabel.c_str(), &labelTextW, &labelTextH);
    renderTextOnButtonHelper(sfxLabel, {labelX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, textColor);

    std::string sfxVolStr = std::to_string(m_currentSfxVolume) + "%";
    TTF_SizeText(m_font, sfxVolStr.c_str(), &labelTextW, &labelTextH);
    renderTextOnButtonHelper(sfxVolStr, {controlStartX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, valueColor);

    m_sfxVolDownButtonRect.x = controlStartX + valueTextWidth + spacing;
    m_sfxVolDownButtonRect.y = currentY + (itemHeight - plusMinusButtonSize)/2;
    m_sfxVolDownButtonRect.w = plusMinusButtonSize;
    m_sfxVolDownButtonRect.h = plusMinusButtonSize;
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_sfxVolDownButtonRect);
    renderTextOnButtonHelper("-", m_sfxVolDownButtonRect, textColor);

    m_sfxVolUpButtonRect.x = m_sfxVolDownButtonRect.x + plusMinusButtonSize + spacing;
    m_sfxVolUpButtonRect.y = currentY + (itemHeight - plusMinusButtonSize)/2;
    m_sfxVolUpButtonRect.w = plusMinusButtonSize;
    m_sfxVolUpButtonRect.h = plusMinusButtonSize;
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_sfxVolUpButtonRect);
    renderTextOnButtonHelper("+", m_sfxVolUpButtonRect, textColor);

    currentY += itemHeight + spacing * 3;

    // --- Nút Back ---
    int backButtonWidth = 120;
    int backButtonHeight = 40;
    m_optionsBackButtonRect.x = menuRect.x + (menuWidth - backButtonWidth) / 2;
    m_optionsBackButtonRect.y = currentY;
    m_optionsBackButtonRect.w = backButtonWidth;
    m_optionsBackButtonRect.h = backButtonHeight;
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_optionsBackButtonRect);
    renderTextOnButtonHelper("Back", m_optionsBackButtonRect, textColor);
}
