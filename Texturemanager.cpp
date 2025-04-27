#include "TextureManager.h"
#include <SDL_mixer.h>
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

    if (pTexture != nullptr) {
        m_textureMap[id] = pTexture;
        return true;
    }

    return false;
}

void TextureManager::draw(const std::string& id, int x, int y, int width, int height,
                         SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect = {0, 0, width, height};
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, m_textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
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
