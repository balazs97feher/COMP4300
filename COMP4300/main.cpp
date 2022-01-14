#include "GameEngine.h"
#include "AssetManager.h"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    //GameEngine game;
    //game.run();

    AssetManager am{ "./config/" };
    am.loadAssets();

    return 0;
}