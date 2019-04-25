#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"
#include "../../map/Map.h"

namespace ragnarok
{
    class CollisionUpdater : public BaseUpdater
    {
    public:
        void Update(const float t_dT, ParticleContainer* t_particles) override
        {
            auto& positions = t_particles->m_position;
            auto& velocities = t_particles->m_velocity;

            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (positions[i].z > 0.f)
                {
                    continue;
                }

                positions[i].z = 0.f; velocities[i].z = 0.f;
            }

            if (!m_map)
            {
                return;
            }

            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (positions[i].z > 0.f)
                {
                    continue;
                }

                ApplyFriction(t_dT, positions[i], velocities[i]);
            }
        }

        void SetMap(Map* t_map)
        {
            m_map = t_map;
        }

    private:
        void ApplyFriction(const float t_dT, sf::Vector3f& t_position, sf::Vector3f& t_velocity) const
        {
            const sf::Vector2i tileCoords = sf::Vector2i(
                static_cast<int>(floor(t_position.x / Sheet::Tile_Size)),
                static_cast<int>(floor(t_position.y / Sheet::Tile_Size)));

            const auto tile = m_map->GetTile(tileCoords.x, tileCoords.y, 0);
            sf::Vector2f friction;
            if (!tile)
            {
                friction = m_map->GetDefaultTile()->m_friction;
            }
            else
            {
                friction = tile->m_properties->m_friction;
            }

            friction.x *= std::abs(t_velocity.x);
            friction.y *= std::abs(t_velocity.y);
            friction *= t_dT;

            if (t_velocity.x != 0.f && friction.x != 0.f)
            {
                if (std::abs(t_velocity.x) - std::abs(friction.x) < 0.f)
                {
                    t_velocity.x = 0.f;
                }
                else
                {
                    t_velocity.x += (t_velocity.x > 0.f ? friction.x * -1.f : friction.x);
                }
            }

            if (t_velocity.y != 0.f && friction.y != 0.f)
            {
                if (std::abs(t_velocity.y) - std::abs(friction.y) < 0.f)
                {
                    t_velocity.y = 0.f;
                }
                else
                {
                    t_velocity.y += (t_velocity.y > 0.f ? friction.y * -1.f : friction.y);
                }
            }
        }

        Map* m_map;
    };
}