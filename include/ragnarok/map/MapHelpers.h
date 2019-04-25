#pragma once

#include <SFML/System/Vector2.hpp>

namespace ragnarok
{
    inline unsigned int ConvertCoords(unsigned int t_x, unsigned int t_y, unsigned int t_layer, const sf::Vector2u& t_mapSize)
    {
        return ((t_layer*t_mapSize.y + t_y) * t_mapSize.x + t_x);
    }

    inline void ConvertCoords(unsigned int t_id, unsigned int& t_x, unsigned int& t_y, unsigned int& t_layer, const sf::Vector2u& t_mapSize)
    {
        t_layer = (t_id / (t_mapSize.x * t_mapSize.y));
        t_y = (t_id - (t_layer * t_mapSize.x * t_mapSize.y)) / t_mapSize.x;
        t_x = t_id - (t_mapSize.x * (t_y + (t_mapSize.y * t_layer)));
    }

    inline bool ValidCoords(unsigned int t_x, unsigned int t_y, unsigned int t_layer, const sf::Vector2u& t_mapSize)
    {
        return !(t_x < 0 || t_y < 0 || t_x >= t_mapSize.x || t_y >= t_mapSize.y || t_layer < 0 || t_layer >= Sheet::Num_Layers);
    }
}