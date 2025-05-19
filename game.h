// Game.h
#ifndef GAME_H
#define GAME_H

#include <SDL.h>       // Cho SDL_Rect, SDL_Window, SDL_Renderer, SDL_Color etc.
#include <SDL_ttf.h>   // Cho TTF_Font
#include <string>
#include <vector>
#include "Constants.h" // Cho GameState, SCREEN_WIDTH, SCREEN_HEIGHT etc.
#include "Snake.h"
#include "Food.h"
// #include "AudioManager.h" // Nếu bạn cần truy cập trực tiếp

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void renderPauseMenu();   // Khai báo các hàm render menu
    void renderOptionsMenu();
    void renderScore();
    // void renderTextOnButtonHelper(const std::string& text, const SDL_Rect& buttonRect, SDL_Color color); // Nếu là private member

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void renderMenu();
    void renderGameOver();
    void renderSpeedBoostBar();
    void renderPauseButton();
    void loadPauseButtonTexture(); // Nếu bạn dùng
    void loadTextures();
    void resetGame();
    void clean(); // Bạn nên có hàm clean để giải phóng tài nguyên

    // Các thành viên hiện có của bạn
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;

    Snake m_snake;
    Food m_food;
    GameState m_gameState;

    int m_score;
    bool m_running;

    std::vector<std::string> m_snakeHeadTextureIds;
    std::vector<std::string> m_snakeBodyTextureIds;
    std::vector<std::string> m_snakeTailTextureIds;

    std::vector<std::string> m_snakeHeadTexturePaths;
    std::vector<std::string> m_snakeBodyTexturePaths;
    std::vector<std::string> m_snakeTailTexturePaths;

    std::string m_pauseIconID;

    // Cho nút Pause trên màn hình game
    SDL_Rect m_pauseButtonRect;
    // std::string m_pauseIconID; // Hoặc SDL_Texture* m_pauseIconTexture;

    // Cho các nút trong Pause Menu
    SDL_Rect m_resumeButtonMenuRect;
    SDL_Rect m_replayButtonMenuRect;
    SDL_Rect m_optionsButtonMenuRect; // Nút "Options" để mở Options Menu

    // ====> CÁC KHAI BÁO QUAN TRỌNG CHO OPTIONS MENU (ĐẢM BẢO BẠN ĐÃ THÊM NHỮNG DÒNG NÀY) <====
    int m_currentMusicVolume;        // Lưu trữ mức âm lượng nhạc hiện tại (0-100)
    int m_currentSfxVolume;          // Lưu trữ mức âm lượng SFX hiện tại (0-100)
    SDL_Rect m_musicVolUpButtonRect;    // Nút tăng âm lượng nhạc
    SDL_Rect m_musicVolDownButtonRect;  // Nút giảm âm lượng nhạc
    SDL_Rect m_sfxVolUpButtonRect;      // Nút tăng âm lượng SFX
    SDL_Rect m_sfxVolDownButtonRect;    // Nút giảm âm lượng SFX
    SDL_Rect m_optionsBackButtonRect;   // Nút "Back" trong Options Menu

    // Cho nút Restart ở Game Over Menu
    SDL_Rect m_gameOverRestartButtonRect;

    // Cho nút Mute/Unmute ở Main Menu
    SDL_Rect m_muteButtonRect_MainMenu;

    SDL_Texture* m_soundOnTexture;  // <<<< THÊM
    SDL_Texture* m_soundOffTexture; // <<<< THÊM
    // Nếu bạn muốn renderTextOnButtonHelper là một phương thức private của Game:
    void renderTextOnButtonHelper(const std::string& text, const SDL_Rect& buttonRect, SDL_Color color);
};

#endif // GAME_H
