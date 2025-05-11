#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "Snake.h"
#include "Food.h"
#include "Constants.h"
#include <string>
#include <vector>

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void clean();
    void resetGame();
    void renderScore();
    void loadTextures();

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void renderMenu();
    void renderPause();
    void renderGameOver();
    void renderSpeedBoostBar();

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;

    Snake m_snake;
    Food m_food;
    GameState m_gameState;

    int m_score;
    bool m_running;

    // Các ID và đường dẫn cho texture của rắn
    std::vector<std::string> m_snakeHeadTextureIds;
    std::vector<std::string> m_snakeBodyTextureIds;
    std::vector<std::string> m_snakeTailTextureIds; // Thêm

    std::vector<std::string> m_snakeHeadTexturePaths;
    std::vector<std::string> m_snakeBodyTexturePaths;
    std::vector<std::string> m_snakeTailTexturePaths; // Thêm
};

#endif
