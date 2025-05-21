#ifndef TEXTUREMANAGER_H_INCLUDED
#define TEXTUREMANAGER_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include <iostream>

class TextureManager {
public:
    static TextureManager* Instance();

    bool load(const std::string& filePath, const std::string& id, SDL_Renderer* renderer);

    // Khai báo hàm draw đã sửa (linh hoạt hơn)
    bool draw(const std::string& id, int x, int y, int width, int height,
              SDL_Renderer* renderer,
              const SDL_Rect* srcRect = nullptr,
              double angle = 0.0,
              SDL_Point* center = nullptr,
              SDL_RendererFlip flip = SDL_FLIP_NONE);

    // <<<< THÊM KHAI BÁO CHO drawFrame NẾU BẠN CẦN VÀ ĐÃ ĐỊNH NGHĨA NÓ >>>>
    void drawFrame(const std::string& id, int x, int y, int width, int height,
                   int currentRow, int currentFrame,
                   SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // <<<< THÊM KHAI BÁO CHO clearTextureMap NẾU BẠN CẦN VÀ ĐÃ ĐỊNH NGHĨA NÓ >>>>
    void clearTextureMap();

    // SDL_Texture* getTexture(const std::string& id); // Nếu bạn dùng

private:
    TextureManager() {}
    ~TextureManager(); // Nên có destructor để gọi clearTextureMap
    static TextureManager* s_pInstance;
    std::map<std::string, SDL_Texture*> m_textureMap;
};

#endif // TEXTUREMANAGER_H_INCLUDED
