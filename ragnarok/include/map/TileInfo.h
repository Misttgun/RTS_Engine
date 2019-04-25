#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "MapDefinitions.h"
#include "../resources/TextureManager.h"

namespace ragnarok
{
    struct TileInfo
    {
        TileInfo(sf::Texture* t_texture, TileID t_id = 0) : m_texture(t_texture), m_id(t_id), m_deadly(false)
        {
            if(!m_texture)
            {
                return;
            }

            m_sprite.setTexture(*m_texture);
            m_sheetSize = m_texture->getSize();
            sf::IntRect tileBoundaries(m_id % (m_sheetSize.x / Sheet::Tile_Size) * Sheet::Tile_Size,  
                                       m_id / (m_sheetSize.y / Sheet::Tile_Size) * Sheet::Tile_Size, 
                                       Sheet::Tile_Size, Sheet::Tile_Size);
            m_sprite.setTextureRect(tileBoundaries);
        }

        sf::Vector2u m_sheetSize;

        sf::Texture* m_texture;
        sf::Sprite m_sprite;

        TileID m_id;
        std::string m_name;
        sf::Vector2f m_friction;
        bool m_deadly;
    };

    struct Tile
    {
        Tile& operator= (const Tile& t_rhs)
        {
            if(&t_rhs != this)
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