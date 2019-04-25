#pragma once

#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

namespace ragnarok
{
    class ParticleSystem;

    class ForceUpdater : public BaseUpdater
    {
        friend class ParticleSystem;

    public:
        ForceUpdater() : m_applicators(nullptr)
        {}

        void Update(const float t_dT, ParticleContainer* t_particles) override
        {
            auto& velocities = t_particles->m_velocity;
            auto& gravity = t_particles->m_gravity;

            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (!gravity[i])
                {
                    continue;
                }

                velocities[i].z -= Gravity * t_dT;
            }

            if (!m_applicators)
            {
                return;
            }

            auto& positions = t_particles->m_position;
            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                for (auto& force : *m_applicators)
                {
                    Force(force.m_center, force.m_force * t_dT, force.m_radius, positions[i], velocities[i]);
                }
            }
        }

        void SetApplicators(ForceApplicatorList* t_list)
        {
            m_applicators = t_list;
        }

        static const float Gravity;

    private:
        void Force(const sf::Vector3f& t_center, const sf::Vector3f& t_force, const float t_radius, sf::Vector3f& t_position, sf::Vector3f& t_velocity) const
        {
            const sf::Vector3f from(t_center.x - t_radius, t_center.y - t_radius, t_center.z - t_radius);
            const sf::Vector3f to(t_center.x + t_radius, t_center.y + t_radius, t_center.z + t_radius);

            if (t_position.x < from.x)
            {
                return;
            }

            if (t_position.y < from.y)
            {
                return;
            }

            if (t_position.z < from.z)
            {
                return;
            }

            if (t_position.x > to.x)
            {
                return;
            }

            if (t_position.y > to.y)
            {
                return;
            }

            if (t_position.z > to.z)
            {
                return;
            }

            const sf::Vector3f distance = t_center - t_position;
            const sf::Vector3f a_distance = sf::Vector3f(std::abs(distance.x), std::abs(distance.y), std::abs(distance.z));
            const float magnitude = std::sqrt(std::pow(a_distance.x, 2) + std::pow(a_distance.y, 2) + std::pow(a_distance.z, 2));
            const sf::Vector3f normal = sf::Vector3f(a_distance.x / magnitude, a_distance.y / magnitude, a_distance.z / magnitude);
            const sf::Vector3f loss = sf::Vector3f(std::abs(t_force.x) / (t_radius / a_distance.x),
                                             std::abs(t_force.y) / (t_radius / a_distance.y),
                                             std::abs(t_force.z) / (t_radius / a_distance.z));
            sf::Vector3f applied = sf::Vector3f((t_force.x > 0 ? t_force.x - loss.x : t_force.x + loss.x),
                                                (t_force.y > 0 ? t_force.y - loss.y : t_force.y + loss.y),
                                                (t_force.z > 0 ? t_force.z - loss.z : t_force.z + loss.z));
            applied.x *= normal.x;
            applied.y *= normal.y;
            applied.z *= normal.z;

            if (distance.x < 0)
            {
                applied.x = -applied.x;
            }

            if (distance.y < 0)
            {
                applied.y = -applied.y;
            }

            if (distance.z < 0)
            {
                applied.z = -applied.z;
            }

            t_velocity += applied;
        }

        ForceApplicatorList* m_applicators;
    };

    const float ForceUpdater::Gravity = 128.f;
}