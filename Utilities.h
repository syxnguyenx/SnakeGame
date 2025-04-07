#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>

SLD_Texture* createTextTexture(SDL_Renderer* rederer, TTF_Font* font, const std::string& text, SDL_Color color);
void drawSlider(SDL_Renderer, int x, int y, int w, int h, float value, SDL_Color color);
void redererScore(SDL_Renderer* renderer, TTF_Font* font, const GameState& state);

#endif // UTILITIES_H_INCLUDED
