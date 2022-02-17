#pragma once

#include <string_view>

namespace Constants
{
    namespace Animation
    {
        constexpr inline std::string_view megaman_standing = "megaman_standing";
        constexpr inline std::string_view megaman_running = "megaman_running";
        constexpr inline std::string_view megaman_jumping = "megaman_jumping";
        constexpr inline std::string_view robot_running = "robot_running";
        constexpr inline std::string_view robot_shooting = "robot_shooting";
        constexpr inline std::string_view robot_dying = "robot_dying";
        constexpr inline std::string_view blade = "blade";
        constexpr inline std::string_view dirt_tile = "dirt_tile";
        constexpr inline std::string_view grass_tile = "grass_tile";
    }
    namespace Sound
    {
        constexpr inline std::string_view jump = "jump";
        constexpr inline std::string_view shoot = "shoot";
    }
    namespace Font
    {
        constexpr inline std::string_view ayar = "ayar";
    }
    namespace Scene
    {
        constexpr inline std::string_view menu = "menu";
        constexpr inline std::string_view platform = "platform";
    }
}