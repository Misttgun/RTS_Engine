#pragma once

#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    class RandomLifespan : public BaseGenerator
    {
    public:
        RandomLifespan() : m_from(0.f), m_to(0.f)
        {}

        RandomLifespan(const float t_from, const float t_to) : m_from(t_from), m_to(t_to)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& rand = *t_emitter->GetParticleSystem()->GetRand();
            auto& lifespans = t_particles->m_maxLifespan;

            for (auto i = t_from; i <= t_to; ++i)
            {
                lifespans[i] = rand(m_from, m_to);
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_from >> m_to;
        }

    private:
        float m_from;
        float m_to;
    };
}