#pragma once

#include <string>
#include <unordered_map>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

struct TextureSheet
{
    sf::Texture texture;
    uint8_t frameCount;
};

class AssetManager
{
public:
    AssetManager(const std::string& pathToAssets);

    void loadAssets();

    TextureSheet& getTexture(const std::string& name);
    sf::Sound& getSound(const std::string& name);
    sf::Font& getFont(const std::string& name);

private:
    const std::string mPathToAssets;

    void addTexture(const std::string& name, const std::string& path, const int frameCount);
    void addSound(const std::string& name, const std::string& path);
    void addFont(const std::string& name, const std::string& path);

    std::unordered_map<std::string, TextureSheet> mTextures;
    std::unordered_map<std::string, sf::Sound> mSounds;
    std::vector<sf::SoundBuffer> mSoundBuffers;
    std::unordered_map<std::string, sf::Font> mFonts;
};

