#ifndef TEXTUREMANEGER_H_INCLUDED
#define TEXTUREMANEGER_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>

class TextureManager {
public:
    //phương thức static để lấy Instnace duy nhất
    static TextureManager* Instance();
    /*texture được lưu vào id
    filepath: đường dẫn đến file ảnh
    id: tên định danh để truy xuất file ảnh */
    bool load(const std::string& filePath, const std::string& id, SDL_Renderer* renderer);
    /*x,y: vị trí vẽ lên màn hình
    width, height: kích thước hình vẽ
    flip: hướng lật mặt ảnh (mặc định không lật)*/
    void draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    /*currentRow: hàng hiện tại trong sprite sheet
    currentFrame: frame hiện tại trong hàng*/
    void drawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer,SDL_RendererFlip flip = SDL_FLIP_NONE);
    //xóa tất cả texture đã load và giải phóng bộ nhớ
    void clearTextureMap();
private:
    //constructor private để ngăn việc tạo instance từ bên ngoài
    TextureManager() {}
    //con trỏ đến instance duy nhất
    static TextureManager* s_pInstance;
    //map lưu trữ các texture với key là string id
    // - Key: chuỗi định danh texture
    // - Value: con trỏ SDL_Texture
    std::map<std::string, SDL_Texture*> m_textureMap
};

#endif // TEXTUREMANEGER_H_INCLUDED
