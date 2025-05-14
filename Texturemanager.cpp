#include "TextureManager.h"
#include <SDL_mixer.h>
#include <iostream>
TextureManager* TextureManager::s_pInstance = nullptr;

TextureManager* TextureManager::Instance() {
    if (s_pInstance == nullptr) {
        s_pInstance = new TextureManager();
    }
    return s_pInstance;
}

bool TextureManager::load(const std::string& filePath, const std::string& id, SDL_Renderer* renderer) {
    SDL_Surface* pSurface = IMG_Load(filePath.c_str());
    if (pSurface == nullptr) {
        return false;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (pTexture == nullptr) {
        return false;
    }

    // Kiểm tra và giải phóng texture cũ nếu ID đã tồn tại
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end()) {
        if (it->second != nullptr) {
            SDL_DestroyTexture(it->second);
        }
        // Không cần m_textureMap.erase(it) nếu bạn sẽ gán đè ngay sau đây
    }

    m_textureMap[id] = pTexture;
    return true;
}

// Định nghĩa hàm draw (đã sửa để trả về bool)
bool TextureManager::draw(const std::string& id, int x, int y, int width, int height,
                          SDL_Renderer* renderer, SDL_RendererFlip flip) {
    auto it = m_textureMap.find(id);
    if (it == m_textureMap.end() || it->second == nullptr) {
        // std::cerr << "Warning: TextureManager::draw: Texture with ID '" << id << "' not found or is null." << std::endl;
        return false;
    }
    SDL_Texture* texture = it->second;
    SDL_Rect srcRect;
    if (SDL_QueryTexture(texture, NULL, NULL, &srcRect.w, &srcRect.h) != 0) {
        return false;
    }
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, nullptr, flip);
    return true;
}

void TextureManager::drawFrame(const std::string& id, int x, int y, int width, int height,
                              int currentRow, int currentFrame, SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect = {width * currentFrame, height * (currentRow - 1), width, height};
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, m_textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::clearTextureMap() {
    for (auto& pair : m_textureMap) {
        SDL_DestroyTexture(pair.second);
    }
    m_textureMap.clear();
}
