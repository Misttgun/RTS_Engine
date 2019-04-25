#pragma once

#include "../BaseGenerator.h"

namespace ragnarok
{
    class SizeRange : public BaseGenerator
    {
    public:
        SizeRange() : m_start(0), m_finish(0)
        {}

        SizeRange(const float t_start, const float t_finish) : m_start(t_start), m_finish(t_finish)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            if (m_start == m_finish)
            {
                auto& sizes = t_particles->m_currentSize;
                for (auto i = t_from; i <= t_to; ++i)
                {
                    sizes[i] = sf::Vector2f(m_start, m_start);
                }
            }
            else
            {
                auto& beginning = t_particles->m_startSize;
                auto& ending = t_particles->m_finalSize;

                for (auto i = t_from; i <= t_to; ++i)
                {
                    beginning[i] = sf::Vector2f(m_start, m_start);
                    ending[i] = sf::Vector2f(m_finish, m_finish);
                }
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_start >> m_finish;
        }

    private:
        float m_start;
        float m_finish;
    };
}