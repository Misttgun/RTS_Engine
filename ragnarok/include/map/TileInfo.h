#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "MapDefinitions.h"
#include "../resources/TextureManager.h"

namespace ragnarok
{
    struct TileInfo
    {
        TileInfo(sf::Texture* t_texture, const std::string& t_name, const sf::Vector2i& t_pos, TileID t_id = 0) 
        : m_texture(t_texture), m_id(t_id), m_name(t_name), m_position(t_pos)
        {
            if (!m_texture)
            {
                return;
            }

            m_sprite.setTexture(*m_texture);
            const sf::IntRect tileBoundaries(m_position.x, m_position.y, Sheet::Tile_Size, Sheet::Tile_Size);
            m_sprite.setTextureRect(tileBoundaries);
        }

        sf::Texture* m_texture;
        sf::Sprite m_sprite;

        TileID m_id;
        std::string m_name;
        sf::Vector2i m_position;
    };

    struct Tile
    {
        Tile& operator= (const Tile& t_rhs)
        {
            if (&t_rhs != this)
            {
                m_properties = t_rhs.m_properties;
                m_warp = t_rhs.m_warp;
                m_solid = t_rhs.m_solid;
            }

            return *this;
        }

        TileInfo* m_properties;
        bool m_warp; // Is the tile a warp.
        bool m_solid; // Is the tile a solid.
    };
}