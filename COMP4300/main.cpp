#include "Game.h"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    Game game;
    game.init("config.txt");
    game.run();

    return 0;
}