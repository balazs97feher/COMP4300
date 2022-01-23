#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace goldenhand
{
    class ConfigReader;
}

template<typename T>
goldenhand::ConfigReader& operator>>(goldenhand::ConfigReader& configReader, T& field);

namespace goldenhand
{
    class ConfigReader
    {
    public:
        ConfigReader(const std::string& path) : filePath{ path }
        {
            if (!std::filesystem::exists(filePath))
            {
                std::cerr << "Config file " << filePath.string() << " does not exit." << std::endl;
                exit(-1);
            }

            fileStream = std::ifstream{ filePath };
            if (!fileStream.is_open())
            {
                std::cerr << "Failed to open config file " << filePath.string() << "." << std::endl;
                exit(-1);
            }
        }

        bool eof() const
        {
            return fileStream.eof();
        }

    private:
        const std::filesystem::path filePath;
        std::ifstream fileStream;

        template<typename T> friend ConfigReader& ::operator>>(ConfigReader& configReader, T& field);
    };
}

template<typename T>
goldenhand::ConfigReader& operator>>(goldenhand::ConfigReader& configReader, T& field)
{
    configReader.fileStream >> field;
    return configReader;
}
