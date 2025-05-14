#ifndef FOOD_H
#define FOOD_H

#include <SDL.h>

enum FoodType {
    BALL,
    SPEED_BOOST
};

class Food {
public:
    Food(int gridWidth, int gridHeight);
    ~Food();

    void update();
    void render(SDL_Renderer* renderer);

    SDL_Point getPosition() const { return m_position; }
    bool isActive() const { return m_isActive; }
    bool isSpeedFood() const { return m_isSpeedFood; }
    FoodType getType() const { return m_type; }

    void generateFood();
    void generateSpeedFood();

private:
    SDL_Point m_position;
    bool m_isActive;
    bool m_isSpeedFood;
    int m_gridWidth;
    int m_gridHeight;
    FoodType m_type;
};

#endif // FOOD_H
