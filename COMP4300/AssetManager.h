#pragma once
#include "Animation.h"

#include <list>
#include <string>
#include <string_view>
#include <unordered_map>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>

namespace goldenhand
{
    class AssetManager
    {
    public:
        AssetManager(const std::string& pathToAssets);

        void loadAssets();

        TextureSheet& getTexture(const std::string& name);
        Animation& getAnimation(const std::string_view name);
        Animation& getAnimation(const std::string& name);
        sf::Sound& getSound(const std::string& name);
        sf::Sound& getSound(const std::string_view name);
        sf::Font& getFont(const std::string& name);
        sf::Font& getFont(const std::string_view name);

    private:
        const std::string mPathToAssets;

        void addTexture(const std::string& name, const std::string& path, const int frameCount);
        void addAnimation(const std::string& name, const std::string& textureName, const int startFrame, const int frameCount, const int speed);
        void addSound(const std::string& name, const std::string& path);
        void addFont(const std::string& name, const std::string& path);

        std::unordered_map<std::string, TextureSheet> mTextures;
        std::unordered_map<std::string_view, Animation> mAnimations;
        std::list<std::string> mAnimationNames;
        std::unordered_map<std::string, sf::Sound> mSounds;
        std::list<sf::SoundBuffer> mSoundBuffers;
        std::unordered_map<std::string, sf::Font> mFonts;
    };
}
