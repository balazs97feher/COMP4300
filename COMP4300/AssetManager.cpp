#include "AssetManager.h"
#include "ConfigReader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

namespace goldenhand
{
    AssetManager::AssetManager(const std::string& pathToAssets) : mPathToAssets{pathToAssets}
    {
    }

    void AssetManager::loadAssets()
    {
        ConfigReader assets{ mPathToAssets + "assets.txt" };

        string assetType, assetName, assetPath, textureName;
        int startFrame, frameCount, speed;
        assets >> assetType;
        while (!assets.eof())
        {
            if (assetType == "Texture")
            {
                assets >> assetName >> assetPath >> frameCount;
                addTexture(assetName, assetPath, frameCount);
            }
            if (assetType == "Animation")
            {
                assets >> assetName >> textureName >> startFrame >> frameCount >> speed;
                addAnimation(assetName, textureName, startFrame, frameCount, speed);
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

    void AssetManager::addAnimation(const std::string& name, const std::string& textureName, const int startFrame, const int frameCount, const int speed)
    {
        mAnimations.emplace(name, Animation(getTexture(textureName), startFrame, frameCount, speed));
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

    Animation& AssetManager::getAnimation(const std::string& name)
    {
        try
        {
            return mAnimations.at(name);
        }
        catch (const std::out_of_range&)
        {
            cerr << "Animation " << name << " does not exist." << endl;
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
} // namespace goldenhand
