#include "TextureManager.h"

//khởi tạo phương thức static instance ban đầu bằng null
TextureManager* TextureManager::s_pInstance = nullptr;

TextureManager* TextureManager::Instance() {
    //nếu instance chưa đc tạo
    if (s_pInstance == nullptr) {
           //tạo mới instance
        s_pInstance = new TextureManager();
    }
    return s_pInstance;
}
//load: tải texture từ file vào bộ nhớ
//filePath: đường dân đến file ảnh
//id: ID duy nhất để quản lí texture
bool TextureManager::load(const std::string& filePath, const std::string& id, SDL_Renderer* renderer) {
    //tải file ảnh vào SDL_Surface
    SDL_Surface* pSurface = IMG_Load(filePath.c_str());
    //kiểm tra lỗi khi tải ảnh
    if (pSurface == nullptr) {
        //in ra thông báo lỗi khi k tải đc ảnh
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return false;
    }

    //chuyển đổi SDL_Surface sang texture
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
    //giải phóng Surface sau khi đã chuyển đổi
    SDL_FreeSurface(pSurface);

    //kiểm tra tạo texture có thành công k
    if (pTexture != nullptr) {
            //lưu texture vào map với id tương ứng
        m_textureMap[id] = pTexture;
        return true;
    }

    return false;
}

//draw: vẽ toàn bộ texture lên màn hình
//id: ID của texture cần vẽ
//x,y :vị trí vẽ(tọa độ màn hình
//width, height: kích thước vẽ
//flip: hướng lật của texture (ngang/dọc)
void TextureManager::draw(const std::string& id, int x, int y, int width, int height,
                         SDL_Renderer* renderer, SDL_RendererFlip flip) {
        //thiết lập vùng cần vẽ từ texture
    SDL_Rect srcRect = {0, 0, width, height};
    //thiết lập vị trí và kích thước cần vẽ
    SDL_Rect destRect = {x, y, width, height};
    // m_textureMap[id]:  Texture cần vẽ (lấy từ map)
    //
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
