#pragma once

#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

namespace ragnarok
{
    class LifespanUpdater : public BaseUpdater
    {
    public:
        void Update(const float t_dT, ParticleContainer* t_particles) override
        {
            auto& lifespans = t_particles->m_lifespan;
            auto& maxLifespans = t_particles->m_maxLifespan;
            for (size_t i = 0; i < t_particles->m_countAlive;)
            {
                lifespans[i] += t_dT;
                if (lifespans[i] < maxLifespans[i])
                {
                    ++i; continue;
                }

                t_particles->Disable(i);
            }
        }
    };
}