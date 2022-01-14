#include "AssetManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

AssetManager::AssetManager(const std::string& pathToAssets) : mPathToAssets{pathToAssets}
{
}

void AssetManager::loadAssets()
{
    const fs::path assetFile{ mPathToAssets + "assets.txt"};
    if (!fs::exists(assetFile))
    {
        cerr << "Asset file " << assetFile.string() << " does not exist." << endl;
        exit(-1);
    }

    std::ifstream assets{ assetFile };
    if (!assets.is_open()) exit(-1);

    string assetType, assetName, assetPath;
    int frameCount;
    assets >> assetType;
    while (!assets.eof())
    {
        if (assetType == "Texture")
        {
            assets >> assetName >> assetPath >> frameCount;
            addTexture(assetName, assetPath, frameCount);
        }
        else if (assetType == "Sound")
        {
            assets >> assetName >> assetPath;
            addSound(assetName, assetPath);
        }
        else if (assetType == "Font")
        {
            assets >> assetName >> assetPath;
            addFont(assetName, assetPath);
        }

        assets >> assetType;
    }
}

void AssetManager::addTexture(const std::string& name, const std::string& path, const int frameCount)
{
    mTextures[name] = TextureSheet{};
    if (!mTextures[name].texture.loadFromFile(mPathToAssets + path)) exit(-1);
    mTextures[name].frameCount = frameCount;
}

void AssetManager::addSound(const std::string& name, const std::string& path)
{
    sf::SoundBuffer soundBuffer;
    if (!soundBuffer.loadFromFile(mPathToAssets + path)) exit(-1);

    mSoundBuffers.push_back(soundBuffer);

    mSounds[name] = sf::Sound{ soundBuffer };
}

void AssetManager::addFont(const std::string& name, const std::string& path)
{
    mFonts[name] = sf::Font{};
    if (!mFonts[name].loadFromFile(mPathToAssets + path)) exit(-1);
}

TextureSheet& AssetManager::getTexture(const std::string& name)
{
    try
    {
        return mTextures.at(name);
    }
    catch (const std::out_of_range&)
    {
        cerr << "Texture " << name << " does not exist." << endl;
        exit(-1);
    }
}

sf::Sound& AssetManager::getSound(const std::string& name)
{
    try
    {
        return mSounds.at(name);
    }
    catch (const std::out_of_range&)
    {
        cerr << "Sound " << name << " does not exist." << endl;
        exit(-1);
    }
}

sf::Font& AssetManager::getFont(const std::string& name)
{
    try
    {
        return mFonts.at(name);
    }
    catch (const std::out_of_range&)
    {
        cerr << "Font " << name << " does not exist." << endl;
        exit(-1);
    }
}
