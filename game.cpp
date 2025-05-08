#include "Game.h"
#include "HighScoreManager.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>

Game::Game() :
    m_window(nullptr),
    m_renderer(nullptr),
    m_font(nullptr),
    m_gameState(MENU),
    m_score(0),
    m_running(true) {}

Game::~Game() {
    clean();
}

bool Game::init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    m_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!m_window) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if(TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        return false;
    }

    m_font = TTF_OpenFont("font.ttf", 24);
    if(!m_font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while(m_running) {
        handleEvents();

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        update(deltaTime);
        render();

        SDL_Delay(16); // Cap at ~60 FPS
    }
}

void Game::clean() {
    if(m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if(m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    if(m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }

    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            m_running = false;
        }

        switch(m_gameState) {
            case MENU:
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    m_gameState = PLAYING;
                }
                break;

            case PLAYING:
                m_snake.handleInput(event);
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                    m_gameState = PAUSED;
                }
                break;

            case PAUSED:
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                    m_gameState = PLAYING;
                }
                break;

            case GAME_OVER:
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                    resetGame();
                    m_gameState = PLAYING;
                }
                break;
        }
    }
}

void Game::update(float deltaTime) {
    if(m_gameState != PLAYING) return;

    m_snake.update();
    m_food.update();

    // Check food collision
    if(m_food.isActive() && m_snake.checkFoodCollision(m_food.getPosition())) {
        if(m_food.isSpeedFood()) {
            m_snake.increaseSpeed();
        }

        m_snake.grow();
        m_score += m_food.isSpeedFood() ? 20 : 10;

        // Generate new food with 20% chance for speed food
        if(rand() % 5 == 0) {
            m_food.generateSpeedFood();
        } else {
            m_food.generateNormalFood();
        }
    }

    // Check snake collision
    if(m_snake.checkCollision()) {
        m_gameState = GAME_OVER;
        HighScoreManager::Instance()->addScore(m_score);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    switch(m_gameState) {
        case MENU:
            renderMenu();
            break;

        case PLAYING:
            // Draw grid
            SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
            for(int x = 0; x < SCREEN_WIDTH; x += GRID_SIZE) {
                SDL_RenderDrawLine(m_renderer, x, 0, x, SCREEN_HEIGHT);
            }
            for(int y = 0; y < SCREEN_HEIGHT; y += GRID_SIZE) {
                SDL_RenderDrawLine(m_renderer, 0, y, SCREEN_WIDTH, y);
            }

            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderSpeedBoostBar();
            renderScore();
            break;

        case PAUSED:
            m_food.render(m_renderer);
            m_snake.render(m_renderer);
            renderPause();
            break;

        case GAME_OVER:
            renderGameOver();
            break;
    }

    SDL_RenderPresent(m_renderer);
}

void Game::renderMenu() {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, "SNAKE GAME", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 50, 200, 50};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);

    surface = TTF_RenderText_Solid(m_font, "Press ENTER to Start", white);
    texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    rect = {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 50, 300, 30};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderPause() {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, "PAUSED", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect rect = {SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 25, 100, 50};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderGameOver() {
    SDL_Color white = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(m_score);
    SDL_Surface* surface = TTF_RenderText_Solid(m_font, scoreText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 50, 200, 50};
    SDL_RenderCopy(m_renderer, texture, NULL, &rect);

    surface = TTF_RenderText_Solid(m_font, "Press R to Restart", white);
    texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 30};
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
    if(!m_snake.isSpeedBoosted()) return;

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
    m_food.generateNormalFood();
}
