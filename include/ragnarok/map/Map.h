#pragma once

#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include <memory>
#include <array>
#include <SFML/Graphics.hpp>
#include "MapDefinitions.h"
#include "TileInfo.h"
#include "TileMap.h"
#include "TileSet.h"
#include "MapLoadee.h"
#include "../resources/TextureManager.h"
#include "../threads/FileLoader.h"
#include "../utilities/Utilities.h"
#include "../window/Window.h"
#include "../ecs/core/EntityManager.h"

namespace ragnarok
{
    using MapLoadees = std::vector< MapLoadee* >;

    class StateManager;

    class Map : public FileLoader
    {
        public:
        Map(Window* t_window, EntityManager* t_entityManager, TextureManager* t_textureManager);
        ~Map();

        void SetStateManager(StateManager* t_stateManager);

        Tile* GetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer);
        TileInfo* GetDefaultTile();

        TileSet* GetTileSet();
        TileMap* GetTileMap();

        unsigned int GetTileSize()const;
        sf::Vector2u GetMapSize()const;
        sf::Vector2f GetPlayerStart()const;
        int GetPlayerId()const;

        void PurgeMap();

        void AddLoadee(MapLoadee* t_loadee);
        void RemoveLoadee(MapLoadee* t_loadee);

        void SaveToFile(const std::string& t_file);

        void Update(float t_dT);
        void ClearMapTexture(sf::Vector3i t_from = sf::Vector3i(0, 0, 0), sf::Vector3i t_to = sf::Vector3i(-1, -1, -1));
        void Redraw(sf::Vector3i t_from = sf::Vector3i(0, 0, 0), sf::Vector3i t_to = sf::Vector3i(-1, -1, -1));
        void Draw(unsigned int t_layer);

        protected:
        bool ProcessLine(std::stringstream& t_stream);

        Window* m_window;
        EntityManager* m_entityManager;
        TextureManager* m_textureManager;
        StateManager* m_stateManager;

        TileSet m_tileSet;
        TileMap m_tileMap;
        MapLoadees m_loadees;

        std::array<sf::RenderTexture, Sheet::Num_Layers> m_textures;
        sf::Sprite m_layerSprite;

        sf::Vector2u m_sheetSize;

        TileInfo m_defaultTile;

        sf::Vector2f m_playerStart;
        int m_playerId;

        float m_gameTime;
        float m_dayLength;
    };
}