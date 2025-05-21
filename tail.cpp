#include "tail.h"
#include "TextureManager.h"
#include "Constants.h"
#include "Vector2D.h"
#include "Utils.h"
#include <cmath>
#include <SDL.h>

Tails::Tails() :
     tailTexture(nullptr),         // Khởi tạo
    lastTailTexture(nullptr),     // Khởi tạo
    curveTexture(nullptr),        // Khởi tạo
    curveTailTexture(nullptr),    // Khởi tạo
    curve_pivot_pos(Vector2D(-1, -1)),      // Khởi tạo 'curve'
    current_total_segments(0),    // Khởi tạo các thành viên khác
    segment_size(GRID_SIZE)
{
    t_srcCurve = {0, 0, 0, 0};
}

Tails::~Tails() {
    FreeTexture();
}

void Tails::Setup(SDL_Renderer* renderer,
                  const std::string& bodyStraightPath, const std::string& bodyStraightID,
                  const std::string& lastTailPath,     const std::string& lastTailID,
                  const std::string& curvePath,        const std::string& curveID,
                  const std::string& curveTailPath,    const std::string& curveTailID,
                  int initialSegments,
                  int segSize) {
    // FreeTexture(); // Nếu có

    m_bodyStraightTextureID = bodyStraightID;
    if (!TextureManager::Instance()->load(bodyStraightPath, m_bodyStraightTextureID, renderer)) { /* log */ }

    m_lastTailTextureID = lastTailID;
    if (!TextureManager::Instance()->load(lastTailPath, m_lastTailTextureID, renderer)) { /* log */ }

    m_curveTextureSheetID = curveID; // Đổi tên biến thành viên cho rõ nếu đây là curve chung
    if (!TextureManager::Instance()->load(curvePath, m_curveTextureSheetID, renderer)) { /* log */ }

    m_curveTailTextureSheetID = curveTailID; // Đổi tên biến thành viên
    if (!TextureManager::Instance()->load(curveTailPath, m_curveTailTextureSheetID, renderer)) { /* log */ }

    // ... (phần còn lại của Setup như khởi tạo segment_size, current_total_segments, ...)
    this->segment_size = segSize;
    this->current_total_segments = initialSegments > 0 ? initialSegments : 1;
    // this->t_srcCurve không cần nữa nếu curve.png không phải spritesheet mà là 1 ảnh cua cơ bản
    this->curve_pivot_pos.x = -1; this->curve_pivot_pos.y = -1;
    this->tail_segments.assign(this->current_total_segments, Vector2D(0, 0));
    this->segment_angles.assign(this->current_total_segments, 0);
}

// tail.cpp - trong Tails::MoveBody
void Tails::MoveBody(const Vector2D& oldHeadPosition, int oldHeadAngle) {
    if (tail_segments.empty()) return; // Nên kiểm tra current_total_segments > 0

    // Vòng lặp từ đốt gần đuôi nhất (current_total_segments - 1) ngược về đốt thứ hai (chỉ số 1)
    for (int i = current_total_segments - 1; i > 0; i--) {
        // Đảm bảo i và i-1 là các chỉ số hợp lệ
        if (static_cast<size_t>(i) < tail_segments.size() && static_cast<size_t>(i-1) < tail_segments.size()) {
            tail_segments[i] = tail_segments[i-1];
        }
        if (static_cast<size_t>(i) < segment_angles.size() && static_cast<size_t>(i-1) < segment_angles.size()) {
            segment_angles[i] = segment_angles[i-1];
        }
    }

    // Đốt đầu tiên của Tails (chỉ số 0)
    if (!tail_segments.empty()) { // Hoặc current_total_segments > 0
         tail_segments[0] = oldHeadPosition;
    }
    if(!segment_angles.empty()){
        segment_angles[0] = oldHeadAngle;
    }
}

void Tails::Grow() {
    if (tail_segments.empty() && segment_angles.empty()) { // Kiểm tra nếu vector rỗng thì không làm gì cả
         std::cerr << "Warning: Tails::Grow() called on empty segments. Initializing with one segment." << std::endl;
         // Có thể bạn muốn khởi tạo 1 đốt ở đây nếu nó rỗng hoàn toàn
         // current_total_segments = 1;
         // tail_segments.assign(1, Vector2D(0,0)); // Ví dụ
         // segment_angles.assign(1, 0);
         // return;
         // Hoặc đơn giản là return nếu không mong muốn hành vi này
         return;
    }
    if (tail_segments.empty() != segment_angles.empty()) { // Kiểm tra sự không đồng nhất (bất thường)
         std::cerr << "Error: Tails::Grow() tail_segments and segment_angles are inconsistent in size." << std::endl;
         return;
    }


    // Thêm một đốt mới bằng cách nhân bản đốt đuôi cuối cùng
    tail_segments.push_back(tail_segments.back());
    segment_angles.push_back(segment_angles.back());
    current_total_segments++; // Tăng tổng số đốt
    // std::cout << "Tails::Grow() called. New total segments: " << current_total_segments << std::endl;
}

Vector2D Tails::getSegmentPosition(int index) const {
    if (index >= 0 && index < current_total_segments && static_cast<size_t>(index) < tail_segments.size()) {
        return tail_segments[index];
    }
    // Trả về một giá trị không hợp lệ hoặc throw lỗi nếu index sai
    std::cerr << "Error: Tails::getSegmentPosition() index out of bounds: " << index << std::endl;
    return Vector2D(-1000, -1000); // Giá trị không hợp lệ rõ ràng để dễ debug
}

int ConvertDirectionToAngleForRendering(Direction dir) {
    switch (dir) {
        case UP:    return 270; // Hoặc -90, tùy thuộc vào TextureManager::draw và ảnh của bạn
        case DOWN:  return 90;
        case LEFT:  return 180;
        case RIGHT: return 0;
        case UNKNOWN_DIRECTION: // Xử lý trường hợp không xác định
        default:
            // std::cout << "ConvertDirectionToAngleForRendering: Huong khong xac dinh, tra ve goc 0" << std::endl;
            return 0;
    }
}

// tail.cpp (hoặc Tails.cpp)

// Đảm bảo hàm này đã được định nghĩa và hoạt động chính xác ở đầu file hoặc trong Utils.hpp
Direction GetDirectionFromTo(const Vector2D& from, const Vector2D& to, int segmentSize);
int ConvertDirectionToAngleForRendering(Direction dir); // Hàm này cũng cần được định nghĩa

// tail.cpp
void Tails::Render(SDL_Renderer* renderer) {
    if (current_total_segments == 0 || !renderer) return;

    SDL_Rect destRect;
    double angleToDraw;
    SDL_RendererFlip flipToUse;
    const SDL_Rect* srcRectToUse; // Sẽ trỏ đến t_srcCurve khi cần
    std::string textureIdToUse;
    SDL_Rect currentSrcCurve_local_for_body; // Dùng để tính toán srcRect cho đốt thân cong hiện tại

    // --- 1. VẼ CÁC ĐỐT THÂN (TỪ ĐẦU TIÊN CỦA TAILS ĐẾN ĐỐT KẾ CUỐI) ---
    // Vòng lặp này sẽ không vẽ đốt đuôi cuối cùng.
    // last_segment_index là chỉ số của đốt đuôi cuối cùng.
    int last_segment_idx = current_total_segments - 1;

    for (int i = 0; i < last_segment_idx; ++i) { // Lặp từ 0 đến current_total_segments - 2
        Vector2D p_curr = tail_segments.at(i);
        destRect = {(int)p_curr.x, (int)p_curr.y, segment_size, segment_size};

        textureIdToUse = "";
        angleToDraw = 0.0;
        srcRectToUse = nullptr;
        flipToUse = SDL_FLIP_NONE;

        Direction dir_in = UNKNOWN_DIRECTION;
        if (i == 0) { // Đốt đầu tiên của Tails (nối với đầu rắn)
            int headAngle = segment_angles.at(0);
            if(headAngle == 0) dir_in = RIGHT;
            else if(headAngle == 90) dir_in = DOWN;
            else if(headAngle == 180) dir_in = LEFT;
            else if(headAngle == 270 || headAngle == -90) dir_in = UP;
        } else {
            dir_in = GetDirectionFromTo(tail_segments.at(i - 1), p_curr, segment_size);
        }

        // Đốt sau đốt hiện tại (p_curr) là tail_segments.at(i + 1)
        // Vì i tối đa là last_segment_idx - 1 (tức current_total_segments - 2),
        // nên i + 1 tối đa sẽ là current_total_segments - 1 (đốt đuôi cuối cùng), chỉ số này hợp lệ.
        Direction dir_out = GetDirectionFromTo(p_curr, tail_segments.at(i + 1), segment_size);

        if (dir_in != UNKNOWN_DIRECTION && dir_out != UNKNOWN_DIRECTION) {
            if (dir_in == dir_out) { // Thân thẳng
                textureIdToUse = m_bodyStraightTextureID; // ID của "body.png"
                angleToDraw = static_cast<double>(ConvertDirectionToAngleForRendering(dir_in));
            } else { // Thân cong
                textureIdToUse = m_curveTextureSheetID; // ID của "curve.png"
                angleToDraw = 0; // Spritesheet thường không cần xoay nếu các hình đã đúng hướng
                bool curve_type_determined = true;

                // LOGIC CHỌN SPRITE CONG (currentSrcCurve_local_for_body) TỪ SPRITESHEET "curve.png"
                // Giả sử curve.png chứa 4 hình cua 40x40 nằm ngang, bắt đầu từ x=0:
                // ⌜ (0,0), ⌝ (40,0), ⌞ (80,0), ⌟ (120,0)
                // Bạn cần thay thế các X_OFFSET này bằng giá trị thực tế của file curve.png
                if ((dir_in == RIGHT && dir_out == UP) || (dir_in == DOWN && dir_out == LEFT)) { // Mong muốn ⌜
                    currentSrcCurve_local_for_body = {0, 0, segment_size, segment_size};
                } else if ((dir_in == LEFT && dir_out == UP) || (dir_in == DOWN && dir_out == RIGHT)) { // Mong muốn ⌝
                    currentSrcCurve_local_for_body = {segment_size * 1, 0, segment_size, segment_size};
                } else if ((dir_in == RIGHT && dir_out == DOWN) || (dir_in == UP && dir_out == LEFT)) { // Mong muốn ⌞
                    currentSrcCurve_local_for_body = {segment_size * 2, 0, segment_size, segment_size};
                } else if ((dir_in == LEFT && dir_out == DOWN) || (dir_in == UP && dir_out == RIGHT)) { // Mong muốn ⌟
                    currentSrcCurve_local_for_body = {segment_size * 3, 0, segment_size, segment_size};
                } else {
                    // std::cout << "Warning: Khuc cua khong xac dinh cho dot than i=" << i << " dir_in=" << dir_in << " dir_out=" << dir_out << std::endl;
                    textureIdToUse = m_bodyStraightTextureID;
                    angleToDraw = static_cast<double>(ConvertDirectionToAngleForRendering(dir_in));
                    curve_type_determined = false;
                }

                if(curve_type_determined) {
                    srcRectToUse = &currentSrcCurve_local_for_body;
                    // Nếu đốt cong này là đốt ngay trước đốt đuôi cuối, cập nhật curve_pivot_pos và t_srcCurve của class
                    if (i == current_total_segments - 2) {  // current_total_segments - 2 là chỉ số của đốt kế cuối
                        curve_pivot_pos = p_curr;      // Lưu vị trí của đốt cong này
                        t_srcCurve = currentSrcCurve_local_for_body; // Lưu srcRect của nó để vẽ đuôi cuối cong
                    }
                }
            }
        } else { // Không xác định được hướng vào hoặc ra, vẽ tạm thân thẳng dựa trên góc đã lưu
            textureIdToUse = m_bodyStraightTextureID;
            angleToDraw = static_cast<double>(ConvertDirectionToAngleForRendering(dir_in));
        }

        // Vẽ đốt thân hiện tại (i)
        if (!textureIdToUse.empty()) {
            if (!TextureManager::Instance()->draw(textureIdToUse, destRect.x, destRect.y, destRect.w, destRect.h,
                                                 renderer, srcRectToUse, angleToDraw, nullptr, flipToUse)) {
                // std::cerr << "Khong ve duoc texture: " << textureIdToUse << " cho dot " << i << std::endl;
            }
        }
    } // Kết thúc vòng lặp vẽ thân

    // --- 2. Vẽ ĐỐT ĐUÔI CUỐI CÙNG (nếu có ít nhất 1 đốt) ---
    if (current_total_segments > 0) {
        // last_segment_idx đã được tính ở trên
        Vector2D last_pos = tail_segments.at(last_segment_idx);
        destRect = {(int)last_pos.x, (int)last_pos.y, segment_size, segment_size};
        angleToDraw = 0.0;
        flipToUse = SDL_FLIP_NONE;
        srcRectToUse = nullptr;
        textureIdToUse = m_lastTailTextureID; // Mặc định là đuôi thẳng "last_tail.png"

        Direction dir_to_last_segment = UNKNOWN_DIRECTION;
        if (current_total_segments == 1) { // Rắn chỉ có 1 đốt do Tails quản lý
            int currentAngleVal = segment_angles.at(last_segment_idx);
            if(currentAngleVal == 0) dir_to_last_segment = RIGHT;
            else if(currentAngleVal == 90) dir_to_last_segment = DOWN;
            else if(currentAngleVal == 180) dir_to_last_segment = LEFT;
            else if(currentAngleVal == 270 || currentAngleVal == -90) dir_to_last_segment = UP;
        } else { // Có ít nhất 2 đốt trong Tails, last_segment_idx > 0
            dir_to_last_segment = GetDirectionFromTo(tail_segments.at(last_segment_idx - 1), last_pos, segment_size);
        }
        angleToDraw = static_cast<double>(ConvertDirectionToAngleForRendering(dir_to_last_segment));

        // Kiểm tra xem đốt TRƯỚC đốt đuôi cuối (tail_segments.at(last_segment_index - 1))
        // có phải là vị trí curve_pivot_pos đã lưu không (nghĩa là đốt trước đuôi là một khúc cua)
        if (current_total_segments > 1 && tail_segments.at(last_segment_idx - 1) == curve_pivot_pos && curve_pivot_pos.x != -1) {
            textureIdToUse = m_curveTailTextureSheetID; // ID của "curve_tail.png"
            // t_srcCurve (biến thành viên của class Tails) lúc này đang giữ srcRect của khúc cua thân trước đó.
            // Bạn cần đảm bảo t_srcCurve này phù hợp với spritesheet m_curveTailTextureSheetID
            // Nếu m_curveTailTextureSheetID là một ảnh đơn thì srcRectToUse = nullptr; và bạn xoay nó.
            // Nếu nó LÀ spritesheet và dùng chung cấu trúc srcCurve với curveTextureSheetID thì dùng t_srcCurve:
            srcRectToUse = &t_srcCurve;
            angleToDraw = 0; // Nếu sprite đuôi cong đã đúng hướng trong spritesheet
        }

        if (!textureIdToUse.empty()) {
            TextureManager::Instance()->draw(textureIdToUse, destRect.x, destRect.y, destRect.w, destRect.h,
                                             renderer, srcRectToUse, angleToDraw, nullptr, flipToUse);
        }
    }
}

// tail.cpp - trong Tails::InitializeBody
void Tails::InitializeBody(const Vector2D& leaderPos, int leaderAngle, int numSegments, int segSizeVal) {
    this->segment_size = segSizeVal;
    this->current_total_segments = numSegments > 0 ? numSegments : 1;

    this->tail_segments.assign(this->current_total_segments, Vector2D(0, 0));
    this->segment_angles.assign(this->current_total_segments, 0);

    if (tail_segments.empty()) return;

    Vector2D prevPos = leaderPos; // Vị trí của đầu rắn

    for (int i = 0; i < current_total_segments; ++i) {
        // Đốt thân thứ i của Tails sẽ nằm ở vị trí của đốt (i-1) của Tails ở frame trước,
        // hoặc ở vị trí của đầu rắn ở frame trước (cho đốt thân đầu tiên).
        // Khi reset, chúng ta đặt chúng thẳng hàng lùi về sau đầu rắn.
        Vector2D currentSegmentPos = prevPos; // Gán trước
        if (leaderAngle == 0) { // RIGHT
            currentSegmentPos.x = leaderPos.x - ((i + 1) * segment_size); // Đốt đầu tiên của Tails cách đầu 1 đoạn
            currentSegmentPos.y = leaderPos.y;
        } else if (leaderAngle == 180) { // LEFT
            currentSegmentPos.x = leaderPos.x + ((i + 1) * segment_size);
            currentSegmentPos.y = leaderPos.y;
        } else if (leaderAngle == -90 || leaderAngle == 270) { // UP
            currentSegmentPos.y = leaderPos.y + ((i + 1) * segment_size);
            currentSegmentPos.x = leaderPos.x;
        } else if (leaderAngle == 90) { // DOWN
            currentSegmentPos.y = leaderPos.y - ((i + 1) * segment_size);
            currentSegmentPos.x = leaderPos.x;
        }
        // Gán vị trí và góc
        if (static_cast<size_t>(i) < tail_segments.size()) tail_segments[i] = currentSegmentPos;
        if (static_cast<size_t>(i) < segment_angles.size()) segment_angles[i] = leaderAngle;
    }
}

int Tails::getTotalSegments() const {
    return current_total_segments; // Giả sử current_total_segments là biến lưu tổng số đốt
}

void Tails::FreeTexture() {
     if (tailTexture) { SDL_DestroyTexture(tailTexture); tailTexture = nullptr; }
    if (lastTailTexture) { SDL_DestroyTexture(lastTailTexture); lastTailTexture = nullptr; }
    if (curveTexture) { SDL_DestroyTexture(curveTexture); curveTexture = nullptr; }
    if (curveTailTexture) { SDL_DestroyTexture(curveTailTexture); curveTailTexture = nullptr; }
}
