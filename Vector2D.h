#ifndef VECTOR2D_H_INCLUDED
#define VECTOR2D_H_INCLUDED

#include <iostream>
#include <string>

struct Vector2D {
    int x, y;

    Vector2D(int x_ = 0, int y_ = 0): x(x_), y(y_) {}

    inline Vector2D operator+(const Vector2D& v2) const {
        return Vector2D(x + v2.x, y + v2.y);
    }
    inline Vector2D operator-(const Vector2D& v2) const {
        return Vector2D(x - v2.x, y - v2.y);
    }
    // Nếu chỉ nhân với số nguyên cho game Snake, dùng int scalar sẽ an toàn hơn
    inline Vector2D operator*(const int scalar) const { // Đổi float thành int
        return Vector2D(x * scalar, y * scalar);
    }
    inline Vector2D& operator+=(const Vector2D& v2) {
        this->x += v2.x;
        this->y += v2.y;
        return *this;
    }
    inline Vector2D& operator-=(const Vector2D& v2) {
        this->x -= v2.x;
        this->y -= v2.y;
        return *this;
    }
    inline bool operator==(const Vector2D& v2) const {
        return (x == v2.x && y == v2.y);
    }
    // Các toán tử so sánh còn lại có thể không cần thiết cho logic Snake cơ bản
    // Nếu không dùng, có thể bỏ đi để struct gọn hơn
    // inline bool operator>=(const Vector2D& v2) const { return (x >= v2.x && y >= v2.y); }
    // inline bool operator<=(const Vector2D& v2) const { return (x <= v2.x && y <= v2.y); }
    // inline bool operator>(const Vector2D& v2) const { return (x > v2.x && y > v2.y); }
    // inline bool operator<(const Vector2D& v2) const { return (x < v2.x && y < v2.y); }

    void SetPos(const int &x_, const int &y_) {
        x = x_;
        y = y_;
    }

    void SetPos(const Vector2D& v2) {
        x = v2.x;
        y = v2.y; // <<<< ĐÃ SỬA LỖI LOGIC
    }

    void Log(std::string msg = "") {
        std::cout << msg << "(x y) = (" << x << " " << y << ")" << std::endl;
    }
};

#endif // VECTOR2D_H_INCLUDED
