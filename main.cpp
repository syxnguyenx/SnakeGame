#include "Game.h"
#include <SDL_mixer.h>
int main(int argc, char* argv[]) {
    Game game;
    if(!game.init()) {
        return -1;
    }

    game.run();
    return 0;
}
