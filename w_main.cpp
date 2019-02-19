#include "w_game.hpp"


int main(int arg, char *args[])
{
    // Keep the resolution Power-of-two
    W_Game game{"PrimS - Maze", 512, 512};
    game.w_gameloop();

    return 0;
}
