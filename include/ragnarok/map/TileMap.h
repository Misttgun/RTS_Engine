#pragma once

#include <unordered_map>
#include "TileInfo.h"
#include "TileSet.h"

namespace ragnarok
{
    using TileMapContainer = std::unordered_map<TileID, std::unique_ptr<Tile>>;
    using TileType = int;

    class TileMap
    {
        public:
        TileMap(const sf::Vector2u& t_mapSize, TileSet& t_tileSet);

        Tile* SetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer, TileType t_tileType);
        Tile* SetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer, Tile* t_tileToCopy);
        Tile* GetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer);

        void PlotTileMap(TileMap& t_map, const sf::Vector2i& t_position, unsigned int t_startLayer);
        void RemoveTile(unsigned int t_x, unsigned int t_y, int t_layer = -1);
        void RemoveTiles(const sf::Vector2u& t_x, const sf::Vector2u& t_y, const sf::Vector2u& t_layer);

        sf::Vector2u GetMapSize() const;
        size_t GetTileCount() const;
        TileSet& GetTileSet() const;

        unsigned int GetLowestElevation() const;
        unsigned int GetHighestElevation() const;
        void SetMapSize(const sf::Vector2u& t_size);

        void SaveToFile(std::ofstream& t_stream);
        void ReadInTile(std::stringstream& t_stream);

        void Purge();

        private:
        Tile * CreateTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer);
        TileMapContainer m_container;
        TileSet& m_tileSet;
        sf::Vector2u m_maxMapSize;
    };
}