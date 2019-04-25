#pragma once

#include "../BaseGenerator.h"
#include "../Emitter.h"

namespace ragnarok
{
    class PointPosition : public BaseGenerator
    {
    public:
        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, size_t t_from, size_t t_to) override
        {
            auto& positions = t_particles->m_position;
            const auto center = t_emitter->GetPosition();

            for (auto i = t_from; i <= t_to; ++i)
            {
                positions[i] = center;
            }
        }
    };
}