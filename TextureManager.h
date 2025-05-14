#ifndef TEXTUREMANAGER_H_INCLUDED
#define TEXTUREMANAGER_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include <iostream> // Cho std::cerr

class TextureManager {
public:
    static TextureManager* Instance();

    // >>> KHAI BÁO HÀM LOAD PHẢI CHÍNH XÁC NHƯ SAU: <<<
    bool load(const std::string& filePath, const std::string& id, SDL_Renderer* renderer);

    // Khai báo hàm draw (đã sửa để trả về bool ở các gợi ý trước)
    bool draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void drawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void clearTextureMap();
    // SDL_Texture* getTexture(const std::string& id); // Nếu bạn đã thêm hàm này

private:
    TextureManager() {}
    ~TextureManager();
    static TextureManager* s_pInstance;
    std::map<std::string, SDL_Texture*> m_textureMap;
};

#endif // TEXTUREMANAGER_H_INCLUDED
