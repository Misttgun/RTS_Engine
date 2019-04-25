#include "ragnarok/map/TileMap.h"
#include "ragnarok/map/MapHelpers.h"

namespace ragnarok
{
    TileMap::TileMap(const sf::Vector2u& t_mapSize, TileSet& t_tileSet) : m_tileSet(t_tileSet)
    {
        SetMapSize(t_mapSize);
    }

    Tile* TileMap::SetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer, TileType t_tileType)
    {
        auto tileInfo = m_tileSet.GetContainer().find(t_tileType);

        if(tileInfo == m_tileSet.GetContainer().end())
        {
            std::cout << "Tile info for " << t_tileType << " wasn't found." << std::endl; return nullptr;
        }

        auto tile = GetTile(t_x, t_y, t_layer);
        if(!tile)
        {
            tile = CreateTile(t_x, t_y, t_layer);
            if(!tile)
            {
                std::cout << "Couldn't create tile " << t_tileType << std::endl; return nullptr;
            }
        }

        tile->m_properties = tileInfo->second.get();

        return tile;
    }

    Tile* TileMap::SetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer, Tile* t_tileToCopy)
    {
        auto tile = GetTile(t_x, t_y, t_layer);
        if(!tile)
        {
            tile = CreateTile(t_x, t_y, t_layer);
            if(!tile)
            {
                std::cout << "Couldn't create tile..." << std::endl; return nullptr;
            }
        }

        *tile = *t_tileToCopy;

        return tile;
    }

    Tile* TileMap::GetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer)
    {
        if(!ValidCoords(t_x, t_y, t_layer, m_maxMapSize))
        {
            return nullptr;
        }

        auto itr = m_container.find(ConvertCoords(t_x, t_y, t_layer, m_maxMapSize));
        if(itr == m_container.end())
        {
            return nullptr;
        }

        return itr->second.get();
    }

    void TileMap::PlotTileMap(TileMap& t_map, const sf::Vector2i& t_position, unsigned int t_startLayer)
    {
        for(auto x = 0; x < t_map.GetMapSize().x; ++x)
        {
            for(auto y = 0; y < t_map.GetMapSize().y; ++y)
            {
                for(auto layer = 0; layer < Sheet::Num_Layers; ++layer)
                {
                    auto position = t_position + sf::Vector2i(x, y);
                    if(position.x >= m_maxMapSize.x || position.y >= m_maxMapSize.y)
                    {
                        continue;
                    }

                    auto tile = t_map.GetTile(x, y, layer);
                    if(!tile)
                    {
                        continue;
                    }
                    // Empty tile. Skip it, don't overwrite the main map!
                    auto newTile = SetTile(t_position.x + x, t_position.y + y, layer + t_startLayer, tile->m_properties->m_id); // use lower-level code to avoid having to re-fetch TileInfo struct.
                    if(!newTile)
                    {
                        std::cout << "Failed adding tile..." << std::endl; continue;
                    }

                    *newTile = *tile;
                }
            }
        }
    }

    void TileMap::RemoveTile(unsigned int t_x, unsigned int t_y, int t_layer)
    {
        if(!ValidCoords(t_x, t_y, t_layer, m_maxMapSize))
        {
            return;
        }

        for(auto layer = (t_layer == -1 ? 0 : t_layer); layer <= (t_layer == -1 ? Sheet::Num_Layers : t_layer); ++layer)
        {
            auto itr = m_container.find(ConvertCoords(t_x, t_y, layer, m_maxMapSize));
            if(itr == m_container.end())
            {
                continue;
            }

            m_container.erase(itr);
        }
    }

    void TileMap::RemoveTiles(const sf::Vector2u& t_x, const sf::Vector2u& t_y, const sf::Vector2u& t_layer)
    {
        if(!ValidCoords(t_x.x, t_y.x, t_layer.x, m_maxMapSize))
        {
            return;
        }

        if(!ValidCoords(t_x.y, t_y.y, t_layer.y, m_maxMapSize))
        {
            return;
        }

        for(auto x = t_x.x; x <= t_x.y; ++x)
        {
            for(auto y = t_y.x; y <= t_y.y; ++y)
            {
                for(auto layer = t_layer.x; layer <= t_layer.y; ++layer)
                {
                    auto itr = m_container.find(ConvertCoords(x, y, layer, m_maxMapSize));
                    if(itr == m_container.end())
                    {
                        continue;
                    }

                    m_container.erase(itr);
                }
            }
        }
    }

    sf::Vector2u TileMap::GetMapSize() const
    {
        return m_maxMapSize;
    }

    size_t TileMap::GetTileCount() const
    {
        return m_container.size();
    }

    TileSet& TileMap::GetTileSet() const
    {
        return m_tileSet;
    }

    unsigned int TileMap::GetLowestElevation() const
    {
        unsigned int x = 0, y = 0, layer = 0;
        unsigned int ret = 0;

        for(auto& tile : m_container)
        {
            ConvertCoords(tile.first, x, y, layer, m_maxMapSize);
            if(layer == 0)
            {
                return 0;
            }

            if(layer < ret)
            {
                ret = layer;
            }
        }

        return ret;
    }

    unsigned int TileMap::GetHighestElevation() const
    {
        unsigned int x = 0, y = 0, layer = 0;
        unsigned int ret = 0;

        for(auto& tile : m_container)
        {
            ConvertCoords(tile.first, x, y, layer, m_maxMapSize);
            if(layer == Sheet::Num_Layers - 1)
            {
                return Sheet::Num_Layers - 1;
            }

            if(layer > ret)
            {
                ret = layer;
            }
        }

        return ret;
    }

    void TileMap::SetMapSize(const sf::Vector2u& t_size)
    {
        // Perform re-allocations and re-calculations of tiles that are already inside the map, as the dimensions are changing.
        m_maxMapSize = t_size;
    }

    void TileMap::SaveToFile(std::ofstream& t_stream)
    {
        bool firstLine = true;

        for(auto& tile : m_container)
        {
            auto coordinateID = tile.first;
            unsigned int x = 0, y = 0, layer = 0;
            ConvertCoords(coordinateID, x, y, layer, m_maxMapSize);
            auto tileID = tile.second->m_properties->m_id;

            if(!firstLine)
            {
                t_stream << std::endl;
            }

            t_stream << "TILE " << tileID << " " << x << " " << y << " " << layer << " " << (tile.second->m_solid ? 1 : 0);
            
            if(firstLine)
            {
                firstLine = false;
            }
        }
    }

    void TileMap::ReadInTile(std::stringstream& t_stream)
    {
        int tileId = 0;
        t_stream >> tileId;

        if(tileId < 0)
        {
            std::cout << "! Bad tile id: " << tileId << std::endl;
            return;
        }

        auto itr = m_tileSet.GetContainer().find(tileId);

        if(itr == m_tileSet.GetContainer().end())
        {
            std::cout << "! Tile id(" << tileId << ") was not found in tileset." << std::endl;
            return;
        }

        sf::Vector2u tileCoords;
        unsigned int tileLayer = 0;
        unsigned int tileSolidity = 0;
        t_stream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
        auto mapSize = GetMapSize();
        
        if(tileCoords.x > mapSize.x || tileCoords.y > mapSize.y || tileLayer >= Sheet::Num_Layers)
        {
            std::cout << "! Tile is out of range: " << tileCoords.x << " " << tileCoords.y << std::endl;
            return;
        }

        auto tile = std::make_unique<Tile>();
        // Bind properties of a tile from a set.
        tile->m_properties = itr->second.get();
        tile->m_solid = (tileSolidity != 0);
        std::string warp;
        t_stream >> warp;
        tile->m_warp = false;
        
        if(warp == "WARP")
        {
            tile->m_warp = true;
        }

        if(!m_container.emplace(ConvertCoords(tileCoords.x, tileCoords.y, tileLayer, m_maxMapSize), std::move(tile)).second)
        {
            // Duplicate tile detected!
            std::cout << "! Duplicate tile! : " << tileCoords.x << " " << tileCoords.y << std::endl;
            return;
        }
    }

    void TileMap::Purge()
    {
        m_container.clear();
    }

    Tile* TileMap::CreateTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer)
    {
        auto tile = std::make_unique<Tile>();
        auto tileAddr = tile.get();
        
        if(!m_container.emplace(ConvertCoords(t_x, t_y, t_layer, m_maxMapSize), std::move(tile)).second)
        {
            return nullptr;
        }

        return tileAddr;
    }
}