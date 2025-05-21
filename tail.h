#ifndef TAIL_H_INCLUDED // Hoặc tên include guard phù hợp
#define TAIL_H_INCLUDED

#include <vector>
#include <string>
#include <SDL.h>       // Cho SDL_Texture, SDL_Rect

#include "Vector2D.h"  // Đảm bảo đây là file định nghĩa Vector2D của bạn
#include "Constants.h" // Giả sử TAILS_SIZE (hoặc GRID_SIZE) được định nghĩa ở đây

// class Game; // Forward declaration nếu Tails cần biết về Game (hiện tại có vẻ không)
// class TextureManager; // Forward declaration nếu Tails cần biết về TextureManager mà không muốn include đầy đủ

class Tails {
private:
    SDL_Texture* tailTexture;      // Dùng để vẽ đốt thân thẳng
    SDL_Texture* lastTailTexture;  // Dùng để vẽ đốt đuôi cuối cùng (khi thẳng)
    SDL_Texture* curveTexture;     // Dùng để vẽ các khúc cua (có thể là spritesheet)
    SDL_Texture* curveTailTexture;

    std::string m_bodyStraightTextureID;     // ID cho "body.png"
    std::string m_lastTailTextureID;       // ID cho "last_tail.png"
    std::string m_curveTextureSheetID;     // ID cho "curve.png" (spritesheet)
    std::string m_curveTailTextureSheetID;

    Vector2D curve_pivot_pos; // <<<< KHAI BÁO BIẾN NÀY (hoặc tên bạn đã chọn)
    SDL_Rect t_srcCurve;

    std::vector<Vector2D> tail_segments;
    int current_total_segments;
    std::vector<int> segment_angles;
    int segment_size;

public:
    Tails();
    ~Tails();

    void Setup(SDL_Renderer* renderer,
               const std::string& bodyStraightPath, const std::string& bodyStraightID,
               const std::string& lastTailPath,     const std::string& lastTailID,
               const std::string& curveSheetPath,   const std::string& curveSheetID,
               const std::string& curveTailPath,    const std::string& curveTailID,
               int initialSegments,
               int segSize);

    void Grow();
    void MoveBody(const Vector2D& newLeaderPosition, int newLeaderAngle);
    void Render(SDL_Renderer* renderer);
    int getTotalSegments() const;
    Vector2D getSegmentPosition(int index) const; // Thêm hàm này
void InitializeBody(const Vector2D& leaderPos, int leaderAngle, int numSegments, int segSizeVal);
    void FreeTexture();
};

#endif // TAIL_H_INCLUDED
