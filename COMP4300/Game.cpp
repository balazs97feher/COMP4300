#include "Game.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

void Game::run()
{
}

void Game::init(const std::string& config)
{
    const fs::path configFile{ config };
    if (!fs::exists(configFile))
    {
        cerr << "Configuration file " << configFile.string() << " does not exit." << endl;
        exit(-1);
    }

    std::ifstream configuration{ configFile };
    if (!configuration.is_open())
    {
        cerr << "Failed to open configuration file." << endl;
        exit(-1);
    }

    for (size_t i = 0; i < 5; i++)
    {
        string configType;
        configuration >> configType;
        if (configType == "Window")
        {
            int wWidth, wHeight, frameLimit, fullScreenMode;
            configuration >> wWidth >> wHeight >> frameLimit >> fullScreenMode;
            const uint32_t wStyle = (fullScreenMode == 1) ? (sf::Style::Fullscreen | sf::Style::Default) : sf::Style::Default;
            mRenderWindow.create(sf::VideoMode(wWidth, wHeight), "Shooter", wStyle);
            mRenderWindow.setFramerateLimit(frameLimit);
        }
        else if (configType == "Font")
        {
            string font;
            int textSize, r, g, b;
            configuration >> font >> textSize >> r >> g >> b;
            mText.setCharacterSize(textSize);
            mText.setFillColor(sf::Color(r, g, b));
            if (!mFont.loadFromFile("./config/" + font))
            {
                cerr << "Failed to load font: " << font << endl;
                exit(-1);
            }
            mText.setFont(mFont);
        }
        else if (configType == "Player")
        {
            PlayerConfig pcfg;
            configuration >> pcfg.SR >> pcfg.CR >> pcfg.S >> pcfg.FR >> pcfg.FG
                >> pcfg.FB >> pcfg.OR >> pcfg.OG >> pcfg.OB >> pcfg.OT >> pcfg.V;
            mPlayerCfg = pcfg;
        }
        else if (configType == "Enemy")
        {
            EnemyConfig ecfg;
            configuration >> ecfg.SR >> ecfg.CR >> ecfg.SMIN >> ecfg.SMAX >> ecfg.OR
                >> ecfg.OG >> ecfg.OB >> ecfg.OT >> ecfg.VMIN >> ecfg.VMAX >> ecfg.L >> ecfg.SI;
            mEnemyCfg = ecfg;
        }
        else if (configType == "Bullet")
        {
            BulletConfig bcfg;
            configuration >> bcfg.SR >> bcfg.CR >> bcfg.S >> bcfg.FR >> bcfg.FG >> bcfg.FB
                >> bcfg.OR >> bcfg.OG >> bcfg.OB >> bcfg.OT >> bcfg.V >> bcfg.L;
            mBulletCfg = bcfg;
        }
    }

    spawnPlayer();
}

void Game::pause()
{
}

void Game::sMovement()
{
}

void Game::sUserInput()
{
}

void Game::sLifespan()
{
}

void Game::sEnemySpawner()
{
}

void Game::sCollision()
{
}

void Game::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);
    
    const auto wSize = mRenderWindow.getSize();
    mPlayer->cTransform = make_shared<CTransform>(sf::Vector2f(wSize.x / 2 - mPlayerCfg.SR, wSize.y / 2 - mPlayerCfg.SR),
        sf::Vector2f{ 0.0, mPlayerCfg.S }, 0.0f);

    const sf::Color fillColor(mPlayerCfg.FR, mPlayerCfg.FG, mPlayerCfg.FB);
    const sf::Color outlineColor(mPlayerCfg.OR, mPlayerCfg.OG, mPlayerCfg.OB);
    mPlayer->cShape = make_shared<CShape>(mPlayerCfg.CR, mPlayerCfg.V, fillColor,
        outlineColor, mPlayerCfg.OT);

    mPlayer->cCollision = make_shared<CCollision>(mPlayerCfg.CR);

    mPlayer->cScore = make_shared<CScore>(0);

    mPlayer->cInput = make_shared<CInput>();
}

void Game::spawnEnemy()
{
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemy)
{
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2i& mousePos)
{
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}
