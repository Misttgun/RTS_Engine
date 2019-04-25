#pragma once

#include "../core/S_Base.h"
#include "../components/C_Position.h"
#include "../components/C_Collidable.h"

namespace ragnarok
{
    struct TileInfo;
    class Map;

    /**
     * \brief Represent a collision information for tiles
     */
    struct CollisionElement
    {
        CollisionElement(float t_area, TileInfo* t_info, const sf::FloatRect& t_bounds) :m_area(t_area), m_tile(t_info), m_tileBounds(t_bounds)
        {}

        float m_area; // Area of collision
        TileInfo* m_tile;
        sf::FloatRect m_tileBounds;
    };

    using Collisions = std::vector<CollisionElement>;

    /**
     * \brief Collision system
     */
    class S_Collision : public S_Base
    {
        public:
        S_Collision(SystemManager* t_systemMgr);
        ~S_Collision() = default;

        void SetMap(Map* t_map);

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        private:
        /**
         * \brief Handles entity on entity collision
         */
        void EntityCollisions();
        
        /**
         * \brief Processes the actual map collisions
         * \param t_entity the entity
         * \param t_pos pointer to the entity position component
         * \param t_col pointer to the entity collidable component
         */
        void MapCollisions(const EntityId& t_entity, C_Position* t_pos, C_Collidable* t_col) const;
        
        /**
         * \brief Check for collision between the entity and the map and fill the t_colisions
         * \param t_pos pointer to the entity position component
         * \param t_col pointer to the entity collidable component
         * \param t_collisions collision information on the map
         */
        void CheckCollisions(C_Position* t_pos, C_Collidable* t_col, Collisions& t_collisions) const;
        
        /**
         * \brief Handles all the collision information that got collected in t_collisons
         * \param t_entity the entity
         * \param t_pos pointer to the entity position component
         * \param t_col pointer to the entity collidable component
         * \param t_collisions collision information on the map
         */
        void HandleCollisions(const EntityId& t_entity, C_Position* t_pos, C_Collidable* t_col, Collisions& t_collisions) const;
        
        /**
         * \brief Check if the entity is out of the boundaries of the map
         * \param t_pos the entity position
         */
        void CheckOutOfBounds(C_Position* t_pos) const;

        Map* m_gameMap;
    };
}