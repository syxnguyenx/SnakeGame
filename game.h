#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "Constants.h" // Cho GameState, SCREEN_WIDTH, SCREEN_HEIGHT, GRID_SIZE, Direction, UNKNOWN_DIRECTION
#include "Food.h"

// Forward declaration để tránh include vòng nếu Snake.h có thể cần Game.h
class Snake;
class AudioManager;     // Forward declare nếu không include đầy đủ
class TextureManager; // Forward declare nếu không include đầy đủ

class Game {
public:
    Game();
    ~Game(); // Quan trọng: Sẽ giải phóng m_snake

    bool init();
    void run();
    void clean(); // Sẽ giải phóng m_snake

    // Các hàm này có thể vẫn hữu ích
    void resetGame();
    void renderScore();
    // void loadTextures(); // Hàm này có thể không cần nữa nếu Snake và Tails tự load texture trong Setup

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void loadTextures();
    // Các hàm render giao diện
    void renderMenu();
    void renderPauseMenu();
    void renderOptionsMenu();
    void renderGameOver();
    void renderSpeedBoostBar();
    void renderPauseButton(); // Nút pause trên màn hình game

    // Hàm tiện ích để vẽ chữ (nên là private member)
    void renderTextOnButtonHelper(const std::string& text, const SDL_Rect& buttonRect, SDL_Color color);

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;

    Food m_food;
    Snake* m_snake; // <<<< SỬ DỤNG CON TRỎ

    GameState m_gameState;
    int m_score;
    bool m_running;

    // ID cho icon nút Pause (nếu bạn dùng cách lưu ID)
    std::string m_pauseIconID;

    // Biến cho các nút trong Pause Menu
    SDL_Rect m_pauseButtonRect;       // Nút Pause trên màn hình chơi
    SDL_Rect m_resumeButtonMenuRect;
    SDL_Rect m_replayButtonMenuRect;
    SDL_Rect m_optionsButtonMenuRect;

    // Biến cho Options Menu
    int m_currentMusicVolume;
    int m_currentSfxVolume;
    SDL_Rect m_musicVolUpButtonRect;
    SDL_Rect m_musicVolDownButtonRect;
    SDL_Rect m_sfxVolUpButtonRect;
    SDL_Rect m_sfxVolDownButtonRect;
    SDL_Rect m_optionsBackButtonRect;

    // Biến cho Game Over Menu
    SDL_Rect m_gameOverRestartButtonRect;

    // Biến cho Main Menu
    SDL_Rect m_muteButtonRect_MainMenu;
    std::string m_soundOnIconID;  // ID cho icon sound on
    std::string m_soundOffIconID; // ID cho icon sound off
};

#endif // GAME_H
