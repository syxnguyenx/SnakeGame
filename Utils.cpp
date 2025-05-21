// Utils.cpp
#include "Utils.h"   // Include file header tương ứng
#include <cmath>       // Cho std::abs
#include <iostream>    // Cho std::cerr (nếu bạn muốn dùng để debug)

Direction GetDirectionFromTo(const Vector2D& from, const Vector2D& to, int segmentSize) {
    // ... (Nội dung hàm GetDirectionFromTo của bạn, đảm bảo có return UNKNOWN_DIRECTION ở cuối) ...
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    bool significantDx = (std::abs(dx) >= segmentSize / 2);
    bool significantDy = (std::abs(dy) >= segmentSize / 2);
    bool minorDx = (std::abs(dx) < segmentSize / 2);
    bool minorDy = (std::abs(dy) < segmentSize / 2);

    if (significantDx && minorDy) {
        if (dx > 0) return RIGHT;
        if (dx < 0) return LEFT;
    } else if (significantDy && minorDx) {
        if (dy > 0) return DOWN;
        if (dy < 0) return UP;
    }
    return UNKNOWN_DIRECTION;
}
