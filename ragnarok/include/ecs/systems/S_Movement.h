#pragma once

#include "../core/S_Base.h"
#include "../components/C_Position.h"
#include "../components/C_Movable.h"

namespace ragnarok
{
    enum class Axis
    {
        x, y
    };

    class Map;

    /**
     * \brief The movement system
     */
    class S_Movement : public S_Base
    {
        public:
        S_Movement(SystemManager* t_systemMgr);
        ~S_Movement() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        void SetMap(Map* t_gameMap);

        private:
        void StopEntity(const EntityId& t_entity, const Axis& t_axis) const;
        void SetDirection(const EntityId& t_entity, const Direction& t_dir) const;

        /**
         * \brief Get the current tile friction
         * \param t_elevation tile elevation
         * \param t_x tile x position
         * \param t_y tile y position
         * \return return the tile friction
         */
        sf::Vector2f GetTileFriction(unsigned int t_elevation, unsigned int t_x, unsigned int t_y) const;
        void MovementStep(float t_dT, C_Movable* t_movable, const C_Position* t_position) const;

        Map* m_gameMap;
    };
}