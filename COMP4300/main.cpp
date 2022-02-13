#include "GameEngine.h"

int main()
{
    goldenhand::GameEngine game;
    game.initialize("./config/settings.txt");
    game.run();

    return 0;
}