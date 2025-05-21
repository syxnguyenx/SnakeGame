#include "Game.h"
#include "Snake.h"           // <<<< Include Snake.h ở đây
#include "tail.h"         // <<<< Include Tails.hpp nếu Snake.h cần hoặc Game dùng đến
#include "Vector2D.h"      // <<<< Include Vector2D.hpp nếu Game dùng đến
#include "HighScoreManager.h"
#include "TextureManager.h"  // Đảm bảo đây là TextureManager.hpp hoặc .h đúng của bạn
#include "AudioManager.h"
// #include "Constants.h" // Đã được include qua Game.h

#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm> // Cho std::min, std::max
#include <vector>
#include <cstdlib>   // Cho rand()

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
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Loi SDL_CreateTextureFromSurface cho text \"" << text << "\": " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect textRect;
    TTF_SizeText(m_font, text.c_str(), &textRect.w, &textRect.h);

    textRect.x = buttonRect.x + (buttonRect.w - textRect.w) / 2;
    textRect.y = buttonRect.y + (buttonRect.h - textRect.h) / 2;

    SDL_RenderCopy(m_renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
}


Game::Game() :
    m_window(nullptr),
    m_renderer(nullptr),
    m_font(nullptr),
    m_food(GRID_WIDTH, GRID_HEIGHT),
    m_snake(nullptr), // <<<< KHỞI TẠO CON TRỎ LÀ NULLPTR
    m_gameState(MENU),
    m_score(0),
    m_currentMusicVolume(80), // Khởi tạo các biến mới
    m_currentSfxVolume(80),
    m_pauseIconID("pause_icon_id"), // Đặt ID cho icon Pause
    m_soundOnIconID("sound_on_icon_id"),
    m_soundOffIconID("sound_off_icon_id"),
    m_running(true) // Đặt m_running ở cuối hoặc theo thứ tự trong Game.h
{
    std::cout << "Game constructor called." << std::endl;
    // Không cần khởi tạo các vector path của rắn ở đây nữa nếu Snake tự quản lý
}

void Game::clean() {
    std::cout << "Game::clean() called." << std::endl;

    // Giải phóng con trỏ Snake nếu bạn dùng con trỏ
    if (m_snake) { // Kiểm tra xem m_snake có phải là con trỏ và đã được khởi tạo không
        delete m_snake;
        m_snake = nullptr;
        std::cout << "m_snake deleted." << std::endl;
    }

    // Dọn dẹp TextureManager (giải phóng tất cả các texture đã load)
    if (TextureManager::Instance()) { // Kiểm tra instance trước khi gọi
        TextureManager::Instance()->clearTextureMap();
        std::cout << "TextureManager cleared." << std::endl;
    }

    // Dọn dẹp AudioManager (nếu có hàm clean riêng hoặc giải phóng ở đây)
    // Ví dụ:
    // if (AudioManager::Instance()) {
    //     AudioManager::Instance()->clean(); // Hoặc tương tự
    //     std::cout << "AudioManager cleaned." << std::endl;
    // }
    // Mix_CloseAudio(); // Mix_Quit() sẽ tự động gọi hàm này

    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
        std::cout << "Font closed." << std::endl;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
        std::cout << "Renderer destroyed." << std::endl;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        std::cout << "Window destroyed." << std::endl;
    }

    // Gọi các hàm Quit của thư viện SDL
    IMG_Quit(); // Dọn dẹp SDL_image nếu bạn đã gọi IMG_Init()
    TTF_Quit();
    Mix_Quit(); // Dọn dẹp SDL_mixer
    SDL_Quit(); // Dọn dẹp SDL
    std::cout << "SDL subsystems quit." << std::endl;
}

Game::~Game() {
    std::cout << "Game destructor called." << std::endl;
    clean(); // Gọi clean để giải phóng tài nguyên, bao gồm cả m_snake
}

bool Game::init() {
    std::cout << "Game::init() started." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo IMG để load PNG (và các định dạng khác nếu cần)
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    m_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window) { /* ... xử lý lỗi và return false ... */ }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) { /* ... xử lý lỗi và return false ... */ }

    if (TTF_Init() == -1) { /* ... xử lý lỗi và return false ... */ }

    m_font = TTF_OpenFont("font.ttf", 24); // Đảm bảo file font.ttf có trong thư mục project hoặc thư mục assets
    if (!m_font) {
        std::cerr << "Failed to load font! Path: font.ttf. Error: " << TTF_GetError() << std::endl;
        return false; // Thoát nếu không load được font
    }

    if (!AudioManager::Instance()) { // AudioManager tự gọi Mix_OpenAudio
         std::cerr << "Failed to initialize AudioManager!" << std::endl;
         return false;
    }
    AudioManager::Instance()->setGlobalVolume(m_currentMusicVolume, m_currentSfxVolume);

    // Load textures chung
    std::string assetPath = ""; // Để trống nếu file ngang cấp .exe, hoặc "assets/"
    if (!TextureManager::Instance()->load(assetPath + "background.png", "background", m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "background.png" << std::endl;
    }
    if (!TextureManager::Instance()->load(assetPath + "normalfood.png", "ball", m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "normalfood.png" << std::endl;
    }
    // Ảnh quả bóng đỏ bạn đã cung cấp, ví dụ tên là "red_ball_speed.png"
    if (!TextureManager::Instance()->load(assetPath + "speedfood.png", "speed_food", m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "speedfood.png" << std::endl;
    }
    if (!TextureManager::Instance()->load(assetPath + "Mute Button unmuted1.png", m_soundOnIconID, m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "Mute Button unmuted1.png" << std::endl;
    }
    if (!TextureManager::Instance()->load(assetPath + "Mute Button muted1.png", m_soundOffIconID, m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "Mute Button muted1.png" << std::endl;
    }
    if (!TextureManager::Instance()->load(assetPath + "pause_icon.png", m_pauseIconID, m_renderer)) {
        std::cerr << "Failed to load: " << assetPath + "pause_icon.png" << std::endl;
    }

    // Load âm thanh
    std::string soundDir = assetPath; // Hoặc assetPath + "sounds/" nếu có thư mục con
    if(!AudioManager::Instance()->loadMusic(soundDir + "background_music.mp3", "bg_music")){
         std::cerr << "Failed to load music: " << soundDir + "background_music.mp3" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound(soundDir + "eat_food.wav", "eat_sound")){
         std::cerr << "Failed to load sound: " << soundDir + "eat_food.wav" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound(soundDir + "game_over.wav", "gameover_sound")){
         std::cerr << "Failed to load sound: " << soundDir + "game_over.wav" << std::endl;
    }
    if(!AudioManager::Instance()->loadSound(soundDir + "click.wav", "click_sound")){
         std::cerr << "Failed to load sound: " << soundDir + "click.wav" << std::endl;
    }

    AudioManager::Instance()->playMusic("bg_music", -1);

    // Khởi tạo Snake
    m_snake = new Snake();
    if (!m_snake) {
        std::cerr << "Loi: Khong the cap phat bo nho cho Snake!" << std::endl;
        return false;
    }
    // Gọi Snake::Setup với các đường dẫn ảnh mới của bạn
    m_snake->Setup(m_renderer,
                   assetPath + "snake.png",          // path cho đầu rắn
                   assetPath + "body.png",           // path cho thân thẳng của Tails
                   assetPath + "last_tail.png",      // path cho đuôi cuối thẳng của Tails
                   assetPath + "curve.png",          // path cho ảnh cong của Tails
                   assetPath + "curve_tail.png"      // path cho đuôi cuối cong của Tails
                  );

    // Khởi tạo vị trí các nút UI
    int buttonSize = 40;
    int padding = 10;
    m_muteButtonRect_MainMenu = {SCREEN_WIDTH - buttonSize - padding, padding, buttonSize, buttonSize};
    m_pauseButtonRect = {SCREEN_WIDTH - buttonSize - padding, padding, buttonSize, buttonSize};

    // loadTextures(); // Hàm này có thể không cần nữa nếu Snake và Tails tự load texture trong Setup của chúng

    std::cout << "Game::init() finished successfully." << std::endl;
    return true;
}

// loadTextures() có thể không cần thiết nữa nếu Snake/Tails tự xử lý
void Game::loadTextures() {
    std::cout << "Game::loadTextures() called." << std::endl;
    // Nếu Snake và Tails tự load texture trong hàm Setup của chúng,
    // thì Game không cần phải load và gán các ID/path này cho Snake nữa.
    // Nếu Snake còn các thành viên headTextureIds... thì phải dùng m_snake->headTextureIds
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    const float targetFrameTime = 1.0f / 60.0f; // ~60 FPS

    while (m_running) {
        Uint32 frameStart = SDL_GetTicks();

        handleEvents();

        Uint32 currentTime = SDL_GetTicks(); // Lấy lại currentTime để tính deltaTime chính xác hơn
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        if (deltaTime <= 0.0f) {
            deltaTime = targetFrameTime; // Giá trị fallback nhỏ, hoặc thời gian khung hình mục tiêu
        }
        lastTime = currentTime;

        update(deltaTime); // Truyền deltaTime vào update
        render();

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < targetFrameTime * 1000.0f) {
            SDL_Delay(static_cast<Uint32>((targetFrameTime * 1000.0f) - frameTime));
        }
    }
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
                    resetGame(); // resetGame đã đặt m_gameState = PLAYING
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
                } else if (SDL_PointInRect(&mousePoint, &m_musicVolDownButtonRect)) {
                    m_currentMusicVolume = std::max(0, m_currentMusicVolume - volumeStep);
                    AudioManager::Instance()->setMusicVolume(m_currentMusicVolume * MIX_MAX_VOLUME / 100);
                } else if (SDL_PointInRect(&mousePoint, &m_sfxVolUpButtonRect)) {
                    m_currentSfxVolume = std::min(100, m_currentSfxVolume + volumeStep);
                    AudioManager::Instance()->setSoundVolume(m_currentSfxVolume * MIX_MAX_VOLUME / 100);
                } else if (SDL_PointInRect(&mousePoint, &m_sfxVolDownButtonRect)) {
                    m_currentSfxVolume = std::max(0, m_currentSfxVolume - volumeStep);
                    AudioManager::Instance()->setSoundVolume(m_currentSfxVolume * MIX_MAX_VOLUME / 100);
                } else if (SDL_PointInRect(&mousePoint, &m_optionsBackButtonRect)) {
                    m_gameState = PAUSED;
                }
                if (SDL_PointInRect(&mousePoint, &m_musicVolUpButtonRect) || SDL_PointInRect(&mousePoint, &m_musicVolDownButtonRect) ||
                    SDL_PointInRect(&mousePoint, &m_sfxVolUpButtonRect) || SDL_PointInRect(&mousePoint, &m_sfxVolDownButtonRect) ||
                    SDL_PointInRect(&mousePoint, &m_optionsBackButtonRect) ) {
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == GAME_OVER) {
                if (SDL_PointInRect(&mousePoint, &m_gameOverRestartButtonRect)) {
                    resetGame();
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
            } else if (m_gameState == MENU) {
                if (SDL_PointInRect(&mousePoint, &m_muteButtonRect_MainMenu)) {
                    AudioManager::Instance()->toggleMuteAll();
                    AudioManager::Instance()->playSound("click_sound", 0);
                }
                // Thêm xử lý click cho nút Start trực quan ở đây nếu có
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
                        if (m_snake) m_snake->handleInput(event); // Kiểm tra m_snake trước khi dùng
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
                        m_gameState = MENU;
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
    if (m_gameState == GAME_OVER) return;
    if (m_gameState != PLAYING) return;
    if (!m_snake) return;


    m_snake->update(); // Cập nhật vị trí rắn
    std::cout << "   Da goi m_snake->update(). Vi tri dau ran moi: ("
              << m_snake->getHeadPosition().x << "," << m_snake->getHeadPosition().y << ")" << std::endl;


    // ===== KIỂM TRA VA CHẠM NGAY LẬP TỨC SAU KHI RẮN DI CHUYỂN =====
    if (m_snake->checkCollisionWithSelf()) {
        std::cout << "   !!! VA CHAM NGAY SAU KHI UPDATE RAN !!!" << std::endl;
        m_gameState = GAME_OVER;
        AudioManager::Instance()->playSound("gameover_sound", 0);
        if(HighScoreManager::Instance()) HighScoreManager::Instance()->addScore(m_score);
        return; // <<<< Rất quan trọng: Thoát khỏi hàm update ngay khi thua
    }
    // ================================================================

    m_food.update(); // Cập nhật trạng thái mồi

    // Chỉ xử lý ăn mồi nếu không có tự va chạm ở trên
    SDL_Point foodPos = m_food.getPosition();
    if (m_food.isActive() && m_snake->checkFoodCollision(foodPos)) { // Đảm bảo checkFoodCollision nhận SDL_Point
        bool foodWasSpeedBoost = (m_food.getType() == SPEED_BOOST);

        m_snake->grow();
        m_score += foodWasSpeedBoost ? 20 : 10;

        if (foodWasSpeedBoost) {
            m_snake->increaseSpeed();
        }
        AudioManager::Instance()->playSound("eat_sound", 0);

        if (rand() % 7 == 0) {
            m_food.generateSpeedFood();
        } else {
            m_food.generateFood();
        }
    }
    std::cout << "Game::update() - Ket thuc frame cho PLAYING" << std::endl;
}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 144, 238, 144, 255);
    SDL_RenderClear(m_renderer);

    // TextureManager::Instance()->draw("background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, m_renderer);

    switch (m_gameState) {
        case MENU:
            renderMenu();
            break;
        case PLAYING:
            m_food.render(m_renderer);
            if (m_snake) m_snake->render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton();
            break;
        case PAUSED:
            m_food.render(m_renderer);
            if (m_snake) m_snake->render(m_renderer);
            renderScore();
            renderSpeedBoostBar();
            renderPauseButton();
            renderPauseMenu();
            break;
        case GAME_OVER:
            renderScore();
            renderGameOver();
            break;
        case OPTIONS_MENU:
            m_food.render(m_renderer);
            if (m_snake) m_snake->render(m_renderer);
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
        SDL_DestroyTexture(texture); // Giải phóng sau khi copy
        SDL_FreeSurface(surface); // Giải phóng sau khi tạo texture
    }

    surface = TTF_RenderText_Solid(m_font, "Press ENTER to Start", black);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect rect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 2 + 50, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    std::string iconToDrawID = AudioManager::Instance()->isMuted() ? m_soundOffIconID : m_soundOnIconID;
    if (!TextureManager::Instance()->draw(iconToDrawID, m_muteButtonRect_MainMenu.x, m_muteButtonRect_MainMenu.y, m_muteButtonRect_MainMenu.w, m_muteButtonRect_MainMenu.h, m_renderer)) {
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
        SDL_DestroyTexture(texture);
    }

    std::string scoreText = "Score: " + std::to_string(m_score);
    surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), black);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_Rect scoreRect = { (SCREEN_WIDTH - surface->w) / 2, SCREEN_HEIGHT / 2 - 30, surface->w, surface->h };
        if (texture) SDL_RenderCopy(m_renderer, texture, NULL, &scoreRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
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
    SDL_FreeSurface(surface);
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
    SDL_DestroyTexture(texture);
}

void Game::renderSpeedBoostBar() {
    if (!m_snake || !m_snake->isSpeedBoosted()) return; // Kiểm tra m_snake
    Uint32 currentTime = SDL_GetTicks();
    Uint32 boostStartTime = m_snake->getSpeedBoostTimer();
    float elapsedBoostTime = static_cast<float>(currentTime - boostStartTime);
    float boostDuration = 3000.0f;

    if (elapsedBoostTime >= boostDuration) return;

    float percent = 1.0f - (elapsedBoostTime / boostDuration);
    if (percent < 0.0f) percent = 0.0f;

    int barHeight = 20;
    int barWidth = 200;
    int padding = 10;
    SDL_Rect outline = {SCREEN_WIDTH - barWidth - padding, SCREEN_HEIGHT - barHeight - padding, barWidth, barHeight};
    SDL_SetRenderDrawColor(m_renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(m_renderer, &outline);

    SDL_Rect fill = {outline.x + 2, outline.y + 2, static_cast<int>((barWidth - 4) * percent), barHeight - 4};
    SDL_SetRenderDrawColor(m_renderer, 255, 100, 100, 255);
    SDL_RenderFillRect(m_renderer, &fill);
}

void Game::resetGame() {
    std::cout << "Game::resetGame() called." << std::endl;
    if (m_snake) m_snake->reset(); // Kiểm tra m_snake
    m_score = 0;
    m_food.generateFood();
    m_gameState = PLAYING;
    std::cout << "Game::resetGame() finished." << std::endl;
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
    if (surf) { // Kiểm tra surf trước khi dùng
    tex = SDL_CreateTextureFromSurface(m_renderer, surf);
    if (tex) {
        // Dòng 631 của bạn có thể là dòng này:
        SDL_Rect titleRect = {menuRect.x + (menuRect.w - surf->w) / 2, menuRect.y + 20, surf->w, surf->h }; // <<<< SỬA THÀNH surf->w và surf->h
        SDL_RenderCopy(m_renderer, tex, NULL, &titleRect);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

    int currentY = menuRect.y + 70;
    int labelX = menuRect.x + 30;
    int controlStartX = menuRect.x + 200;
    int valueTextWidth = 70;
    int plusMinusButtonSize = 35;
    int itemHeight = 40;
    int spacing = 10;

    // --- Music Volume ---
    std::string musicLabel = "Music Volume:";
    int labelTextW, labelTextH;
    TTF_SizeText(m_font, musicLabel.c_str(), &labelTextW, &labelTextH);
    renderTextOnButtonHelper(musicLabel, {labelX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, textColor);

    std::string musicVolStr = std::to_string(m_currentMusicVolume) + "%";
    TTF_SizeText(m_font, musicVolStr.c_str(), &labelTextW, &labelTextH);
    renderTextOnButtonHelper(musicVolStr, {controlStartX, currentY + (itemHeight - labelTextH)/2, labelTextW, labelTextH}, valueColor);

    m_musicVolDownButtonRect = {controlStartX + valueTextWidth + spacing, currentY + (itemHeight - plusMinusButtonSize)/2, plusMinusButtonSize, plusMinusButtonSize};
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_musicVolDownButtonRect);
    renderTextOnButtonHelper("-", m_musicVolDownButtonRect, textColor);

    m_musicVolUpButtonRect = {m_musicVolDownButtonRect.x + plusMinusButtonSize + spacing, currentY + (itemHeight - plusMinusButtonSize)/2, plusMinusButtonSize, plusMinusButtonSize};
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

    m_sfxVolDownButtonRect = {controlStartX + valueTextWidth + spacing, currentY + (itemHeight - plusMinusButtonSize)/2, plusMinusButtonSize, plusMinusButtonSize};
    SDL_SetRenderDrawColor(m_renderer, btnColor.r, btnColor.g, btnColor.b, 255);
    SDL_RenderFillRect(m_renderer, &m_sfxVolDownButtonRect);
    renderTextOnButtonHelper("-", m_sfxVolDownButtonRect, textColor);

    m_sfxVolUpButtonRect = {m_sfxVolDownButtonRect.x + plusMinusButtonSize + spacing, currentY + (itemHeight - plusMinusButtonSize)/2, plusMinusButtonSize, plusMinusButtonSize};
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
