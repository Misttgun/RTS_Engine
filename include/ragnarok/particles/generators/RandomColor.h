#pragma once

#include <SFML/Graphics/Color.hpp>
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    class RandomColor : public BaseGenerator
    {
    public:
        RandomColor() = default;

        RandomColor(const sf::Vector3i& t_from, const sf::Vector3i& t_to) : m_from(t_from), m_to(t_to)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& rand = *t_emitter->GetParticleSystem()->GetRand();
            auto& colors = t_particles->m_currentColor;

            for (auto i = t_from; i <= t_to; ++i)
            {
                const sf::Color target{
                    static_cast<sf::Uint8>(rand(m_from.x, m_to.x)),
                    static_cast<sf::Uint8>(rand(m_from.y, m_to.y)),
                    static_cast<sf::Uint8>(rand(m_from.z, m_to.z)),
                    255
                };

                colors[i] = target;
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_from.x >> m_to.x >> m_from.y >> m_to.y >> m_from.z >> m_to.z;
        }

    private:
        sf::Vector3i m_from;
        sf::Vector3i m_to;
    };
}