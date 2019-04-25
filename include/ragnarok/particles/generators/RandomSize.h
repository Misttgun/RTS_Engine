#pragma once

#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    class RandomSize : public BaseGenerator
    {
    public:
        RandomSize() : m_from(0), m_to(0)
        {}

        RandomSize(const int t_from, const int t_to) : m_from(t_from), m_to(t_to)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& rand = *t_emitter->GetParticleSystem()->GetRand();
            auto& sizes = t_particles->m_currentSize;

            for (auto i = t_from; i <= t_to; ++i)
            {
                const float size = static_cast<float>(rand(m_from, m_to));
                sizes[i] = sf::Vector2f(size, size);
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_from >> m_to;
        }

    private:
        int m_from;
        int m_to;
    };
}