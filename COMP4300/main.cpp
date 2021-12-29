#include "Game.h"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    Game game;
    game.init("./config/config_assignment02.txt");
    game.run();

    return 0;
}