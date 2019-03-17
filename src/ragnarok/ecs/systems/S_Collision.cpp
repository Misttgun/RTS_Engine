#include "ragnarok/ecs/systems/S_Collision.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/map/Map.h"

namespace ragnarok
{
    S_Collision::S_Collision(SystemManager* t_systemMgr) : S_Base(System::Collision, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::Collidable));
        m_requiredComponents.push_back(req);
        req.Clear();

        m_gameMap = nullptr;
    }

    void S_Collision::SetMap(Map* t_map)
    {
        m_gameMap = t_map;
    }

    void S_Collision::Update(float t_dT)
    {
        if(!m_gameMap)
        {
            return;
        }

        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for(auto &entity : m_entities)
        {
            const auto position = entityMgr->GetComponent<C_Position>(entity, Component::Position);
            auto collidable = entityMgr->GetComponent<C_Collidable>(entity, Component::Collidable);

            CheckOutOfBounds(position);
            collidable->SetPosition(position->GetPosition());
            collidable->ResetCollisionFlags();
            MapCollisions(entity, position, collidable);
        }

        //EntityCollisions(); Not useful for now
    }

    void S_Collision::EntityCollisions()
    {
        EntityManager* entities = m_systemManager->GetEntityManager();

        for(auto itr = m_entities.begin(); itr != m_entities.end(); ++itr)
        {
            for(auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr2)
            {
                const auto collidable1 = entities->GetComponent<C_Collidable>(*itr, Component::Collidable);
                const auto collidable2 = entities->GetComponent<C_Collidable>(*itr2, Component::Collidable);
                
                if(collidable1->GetCollidable().intersects(collidable2->GetCollidable()))
                {
                    // Entity-on-entity collision!
                }
            }
        }
    }

    void S_Collision::CheckOutOfBounds(C_Position* t_pos) const
    {
        const unsigned int TileSize = m_gameMap->GetTileSize();

        if(t_pos->GetPosition().x < 0)
        {
            t_pos->SetPosition(0.0f, t_pos->GetPosition().y);
        }
        else if(t_pos->GetPosition().x > m_gameMap->GetMapSize().x * TileSize)
        {
            t_pos->SetPosition(static_cast<float>(m_gameMap->GetMapSize().x * TileSize), t_pos->GetPosition().y);
        }

        if(t_pos->GetPosition().y < 0)
        {
            t_pos->SetPosition(t_pos->GetPosition().x, 0.0f);
        }
        else if(t_pos->GetPosition().y > m_gameMap->GetMapSize().y * TileSize)
        {
            t_pos->SetPosition(t_pos->GetPosition().x, static_cast<float>(m_gameMap->GetMapSize().y * TileSize));
        }
    }

    void S_Collision::CheckCollisions(C_Position* t_pos, C_Collidable* t_col, Collisions& t_collisions) const
    {
        // Establish a range of tiles that are intersecting with the entity collision box
        const unsigned int TileSize = m_gameMap->GetTileSize();
        sf::FloatRect EntityAABB = t_col->GetCollidable();
        const int FromX = static_cast<int>(floor(EntityAABB.left / TileSize));
        const int ToX = static_cast<int>(floor((EntityAABB.left + EntityAABB.width) / TileSize));
        const int FromY = static_cast<int>(floor(EntityAABB.top / TileSize));
        const int ToY = static_cast<int>(floor((EntityAABB.top + EntityAABB.height) / TileSize));

        for(int x = FromX; x <= ToX; ++x)
        {
            for(int y = FromY; y <= ToY; ++y)
            {
                for(size_t l = t_pos->GetElevation(); l < t_pos->GetElevation() + 1; ++l)
                {
                    auto t = m_gameMap->GetTile(x, y, l);

                    // The tile does not exist
                    if(!t)
                    {
                        continue;
                    }

                    // The tile is not solid
                    if(!t->m_solid)
                    {
                        continue;
                    }

                    // Construct the tile AABB box and the collision information to t_collisions
                    sf::FloatRect TileAABB = static_cast<sf::FloatRect>(sf::IntRect(x*TileSize, y*TileSize, TileSize, TileSize));
                    sf::FloatRect Intersection;
                    EntityAABB.intersects(TileAABB, Intersection);
                    float S = Intersection.width * Intersection.height;
                    t_collisions.emplace_back(S, t->m_properties, TileAABB);

                    break;
                }
            }
        }
    }

    void S_Collision::HandleCollisions(const EntityId& t_entity, C_Position* t_pos, C_Collidable* t_col, Collisions& t_collisions) const
    {
        sf::FloatRect EntityAABB = t_col->GetCollidable();
        const unsigned int TileSize = m_gameMap->GetTileSize();

        if(t_collisions.empty())
        {
            return;
        }

        // Sort collision information in a descending order. This ensures that collision with the largest area of intersection comes first
        std::sort(t_collisions.begin(), t_collisions.end(), [](CollisionElement& t_1, CollisionElement& t_2) {return t_1.m_area > t_2.m_area;});

        for(auto &col : t_collisions)
        {
            EntityAABB = t_col->GetCollidable();

            // Is the entity still colliding with the tile
            if(!EntityAABB.intersects(col.m_tileBounds))
            {
                continue;
            }

            // Penetration information on x and y axis
            const float xDiff = (EntityAABB.left + (EntityAABB.width / 2)) - (col.m_tileBounds.left + (col.m_tileBounds.width / 2));
            const float yDiff = (EntityAABB.top + (EntityAABB.height / 2)) - (col.m_tileBounds.top + (col.m_tileBounds.height / 2));
            
            // Exact distance by which the entity is going to be pushed to resolve the collision
            float resolve = 0;

            // Decide which axis to resolve the collision on
            if(std::abs(xDiff) > std::abs(yDiff))
            {
                if(xDiff > 0)
                {
                    resolve = (col.m_tileBounds.left + TileSize) - EntityAABB.left;
                }
                else
                {
                    resolve = -((EntityAABB.left + EntityAABB.width) - col.m_tileBounds.left);
                }

                t_pos->MoveBy(sf::Vector2f(resolve, 0.f));
                t_col->SetPosition(t_pos->GetPosition());
                m_systemManager->AddEvent(t_entity, static_cast<EventID>(EntityEvent::Colliding_X));
                t_col->CollideOnX();
            }
            else
            {
                if(yDiff > 0)
                {
                    resolve = (col.m_tileBounds.top + TileSize) - EntityAABB.top;
                }
                else
                {
                    resolve = -((EntityAABB.top + EntityAABB.height) - col.m_tileBounds.top);
                }

                t_pos->MoveBy(sf::Vector2f(0.f, resolve));
                t_col->SetPosition(t_pos->GetPosition());
                m_systemManager->AddEvent(t_entity, static_cast<EventID>(EntityEvent::Colliding_Y));
                t_col->CollideOnY();
            }
        }
    }

    void S_Collision::MapCollisions(const EntityId& t_entity, C_Position* t_pos, C_Collidable* t_col) const
    {
        Collisions c;
        CheckCollisions(t_pos, t_col, c);
        HandleCollisions(t_entity, t_pos, t_col, c);
    }

    void S_Collision::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {}

    void S_Collision::Notify(const Message& t_message)
    {}
}