#pragma once

#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

namespace ragnarok
{
    class SpatialUpdater : public BaseUpdater
    {
    public:
        void Update(const float t_dT, ParticleContainer* t_particles) override
        {
            auto& velocities = t_particles->m_velocity;
            auto& accelerations = t_particles->m_acceleration;
            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                velocities[i] += accelerations[i] * t_dT;
            }

            auto& positions = t_particles->m_position;
            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                positions[i] += velocities[i] * t_dT;
            }
        }
    };
}