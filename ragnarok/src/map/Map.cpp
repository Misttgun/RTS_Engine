#include "../../include/map/Map.h"
#include "../../include/states/StateManager.h"
#include "../../include/ecs/core/EntityManager.h"
#include "../../include/ecs/components/C_Position.h"
#include "../../include/window/Window.h"
#include "../../include/states/StateLoading.h"

namespace ragnarok
{
	Map::Map(Window* t_window, EntityManager* t_entityManager, TextureManager* t_textureManager)
		: m_window(t_window), m_entityManager(t_entityManager), m_textureManager(t_textureManager), m_tileSet(t_textureManager)
		, m_tileMap({ 0, 0 }, m_tileSet), m_defaultTile(nullptr, "DTile", sf::Vector2i(0, 0)), m_playerId(-1), m_gameTime(0.f), m_dayLength(30.f), m_discreteMap(GetMapSize())
    {
        m_layerSprite.setPosition({ 0,0 });
    }

    Map::~Map()
    {
        PurgeMap();
        m_tileSet.Purge();
		m_discreteMap.Purge();
    }

    void Map::SetStateManager(StateManager* t_stateManager)
    {
        m_stateManager = t_stateManager;
    }

    Tile* Map::GetTile(unsigned int t_x, unsigned int t_y, unsigned int t_layer)
    {
        return m_tileMap.GetTile(t_x, t_y, t_layer);
    }

    TileInfo* Map::GetDefaultTile()
    {
        return &m_defaultTile;
    }

    TileSet* Map::GetTileSet()
    {
        return &m_tileSet;
    }

    TileMap* Map::GetTileMap()
    {
        return &m_tileMap;
    }

	DiscreteMap* Map::GetDiscreteMap()
	{
		return &m_discreteMap;
	}

    unsigned int Map::GetTileSize()const
    {
        return Sheet::Tile_Size;
    }

    sf::Vector2u Map::GetMapSize()const
    {
        return m_tileMap.GetMapSize();
    }

    sf::Vector2f Map::GetPlayerStart()const
    {
        return m_playerStart;
    }

    int Map::GetPlayerId()const
    {
        return m_playerId;
    }

    void Map::SaveToFile(const std::string& t_file)
    {
        std::ofstream file(t_file, std::ios::out);
        if (!file.is_open())
        {
            std::cout << "Failed to save map to location: " << t_file << std::endl; return;
        }

        const auto mapSize = m_tileMap.GetMapSize();
        file << "SIZE " << mapSize.x << " " << mapSize.y << std::endl;
        file << "SHEET " << m_tileSet.GetName() << std::endl;

        for (auto& loadee : m_loadees)
        {
            loadee->SaveMap(file);
        }

        m_tileMap.SaveToFile(file);
        file.close();
    }

    void Map::Redraw(sf::Vector3i t_from, sf::Vector3i t_to)
    {
        const auto mapSize = m_tileMap.GetMapSize();
        if (mapSize.x == 0 || mapSize.y == 0)
        {
            return;
        }

        if (t_from.x < 0 || t_from.y < 0 || t_from.z < 0)
        {
            return;
        }

        if (t_from.x >= mapSize.x || t_from.y >= mapSize.y)
        {
            return;
        }

        const sf::Vector3i originalTo = t_to;
        if (t_to.x < 0 || t_to.x >= mapSize.x)
        {
            t_to.x = mapSize.x - 1;
        }

        if (t_to.y < 0 || t_to.y >= mapSize.y)
        {
            t_to.y = mapSize.y - 1;
        }

        if (t_to.z < 0 || t_to.z >= Sheet::Num_Layers)
        {
            t_to.z = Sheet::Num_Layers - 1;
        }

        const auto realMapSize = mapSize * static_cast<unsigned int>(Sheet::Tile_Size);

        for (auto layer = t_from.z; layer <= t_to.z; ++layer)
        {
            if (m_textures[layer].getSize() == realMapSize)
            {
                continue;
            }

            if (!m_textures[layer].create(realMapSize.x, realMapSize.y))
            {
                std::cout << "Failed creating tile layer texture!" << std::endl;
            }
        }

        ClearMapTexture(t_from, originalTo);

        for (auto x = t_from.x; x <= t_to.x; ++x)
        {
            for (auto y = t_from.y; y <= t_to.y; ++y)
            {
                for (auto layer = t_from.z; layer <= t_to.z; ++layer)
                {
                    auto tile = m_tileMap.GetTile(x, y, layer);

                    if (!tile)
                    {
                        continue;
                    }

                    auto& sprite = tile->m_properties->m_sprite;
                    sprite.setPosition(static_cast<float>(x * Sheet::Tile_Size), static_cast<float>(y * Sheet::Tile_Size));
                    m_textures[layer].draw(sprite);
                }
            }
        }

        for (auto layer = t_from.z; layer <= t_to.z; ++layer)
        {
            m_textures[layer].display();
        }
    }

    void Map::ClearMapTexture(sf::Vector3i t_from, sf::Vector3i t_to)
    {
        const auto mapSize = m_tileMap.GetMapSize();
        if (t_from.x < 0 || t_from.y < 0 || t_from.z < 0)
        {
            return;
        }

        if (t_from.x >= mapSize.x || t_from.y >= mapSize.y)
        {
            return;
        }

        const auto toLayer = (t_to.z < 0 || t_to.z >= Sheet::Num_Layers ? Sheet::Num_Layers - 1 : t_to.z);
        if (t_to.x == -1 && t_to.y == -1)
        {
            // The entire map needs to be cleared, so just invoke .clear() on desired layers.
            for (auto layer = t_from.z; layer <= toLayer; ++layer)
            {
                m_textures[layer].clear({ 0,0,0,0 });
            }
            return;
        }

        // Portion of the map needs clearing.
        const auto position = sf::Vector2i(t_from.x, t_from.y) * static_cast<int>(Sheet::Tile_Size);
        const auto size = sf::Vector2i(
            ((t_to.x < 0 ? mapSize.x - 1 : t_to.x) - t_from.x) + 1,
            ((t_to.y < 0 ? mapSize.y - 1 : t_to.y) - t_from.y) + 1)
            * static_cast<int>(Sheet::Tile_Size);

        sf::RectangleShape shape;
        shape.setPosition(sf::Vector2f(position));
        shape.setSize(sf::Vector2f(size));
        shape.setFillColor(sf::Color(0, 0, 0, -255));

        for (auto layer = t_from.z; layer <= toLayer; ++layer)
        {
            m_textures[layer].draw(shape, sf::BlendMultiply);
            m_textures[layer].display();
        }
    }

    bool Map::ProcessLine(std::stringstream& t_stream)
    {
        std::string type;
        if (!(t_stream >> type))
        {
            return false;
        }

        if (type == "TILE")
        {
            m_tileMap.ReadInTile(t_stream);
        }
        else if (type == "SIZE")
        {
            sf::Vector2u mapSize;
            t_stream >> mapSize.x >> mapSize.y;
            m_tileMap.SetMapSize(mapSize);
        }
        else if (type == "ENTITY")
        {
            // Set up entity here.
            std::string name;
            t_stream >> name;
            if (name == "Player" && m_playerId != -1)
            {
                return true;
            }

            const int entityId = m_entityManager->AddEntity(name);

            if (entityId < 0)
            {
                return true;
            }

            if (name == "Player")
            {
                m_playerId = entityId;
            }

            const auto position = m_entityManager->GetComponent<C_Position>(entityId, Component::Position);
            if (position)
            {
                t_stream >> *position;
            }
        }
        else if (type == "SHEET")
        {
            std::string sheetName;
            t_stream >> sheetName;
            m_tileSet.ResetWorker();
            m_tileSet.AddFile(Utils::GetWorkingDirectory() + "res/Tilesheets/" + sheetName);
            m_tileSet.SetName(sheetName);
            auto loading = m_stateManager->GetState<StateLoading>(StateType::Loading);
            loading->AddLoader(&m_tileSet);

            while (!m_tileSet.IsDone())
            {
                std::cout << "Waiting for tile set to load..." << std::endl; sf::sleep(sf::seconds(0.5f));
            }
        }
        else
        {
            // Something else.
            std::cout << "! Passing type \"" << type << "\" to map loadees." << std::endl;

            for (auto& loadee : m_loadees)
            {
                loadee->ReadMapLine(type, t_stream);
            }
        }
        return true;
    }

    void Map::AddLoadee(MapLoadee* t_loadee)
    {
        m_loadees.emplace_back(t_loadee);
    }

    void Map::RemoveLoadee(MapLoadee* t_loadee)
    {
        m_loadees.erase(std::find_if(m_loadees.begin(), m_loadees.end(), [t_loadee](MapLoadee* t_arg)
        {
            return t_arg == t_loadee;
        }));
    }

    void Map::Update(float t_dT)
    {
        m_gameTime += t_dT;
        if (m_gameTime > m_dayLength * 2)
        {
            m_gameTime = 0.f;
        }

        float timeNormal = m_gameTime / m_dayLength;

        if (timeNormal > 1.f)
        {
            timeNormal = 2.f - timeNormal;
        }

        auto shader = m_window->GetRenderer()->GetShader("default");
        if (!shader)
        {
            return;
        }

        shader->setUniform("timeNormal", timeNormal);
    }

    void Map::Draw(unsigned int t_layer)
    {
        if (t_layer >= Sheet::Num_Layers)
        {
            return;
        }

        m_layerSprite.setTexture(m_textures[t_layer].getTexture());
        m_layerSprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(m_textures[t_layer].getSize())));
        m_window->GetRenderer()->UseShader("default");
        m_window->GetRenderer()->Draw(m_layerSprite);
    }

    void Map::PurgeMap()
    {
        m_tileMap.Purge();
        m_entityManager->Purge();
        m_playerId = -1;
        m_tileSet.Purge();
        Redraw();
    }
}
