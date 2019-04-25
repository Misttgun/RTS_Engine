#pragma once

#include <SFML/Graphics/Color.hpp>
#include "../BaseGenerator.h"

namespace ragnarok
{
    class ColorRange : public BaseGenerator
    {
    public:
        ColorRange() = default;

        ColorRange(const sf::Color& t_start, const sf::Color& t_finish) : m_start(t_start), m_finish(t_finish)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& beginning = t_particles->m_startColor;
            auto& current = t_particles->m_currentColor;
            auto& ending = t_particles->m_finalColor;

            for (auto i = t_from; i <= t_to; ++i)
            {
                beginning[i] = m_start;
                current[i] = m_start;
                ending[i] = m_finish;
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            int s_r = 0, s_g = 0, s_b = 0, s_a = 0;
            int f_r = 0, f_g = 0, f_b = 0, f_a = 0;
            t_stream >> s_r >> s_g >> s_b >> s_a;
            t_stream >> f_r >> f_g >> f_b >> f_a;

            m_start = { static_cast<sf::Uint8>(s_r), static_cast<sf::Uint8>(s_g), static_cast<sf::Uint8>(s_b), static_cast<sf::Uint8>(s_a) };
            m_finish = { static_cast<sf::Uint8>(f_r), static_cast<sf::Uint8>(f_g), static_cast<sf::Uint8>(f_b), static_cast<sf::Uint8>(f_a) };
        }

    private:
        sf::Color m_start;
        sf::Color m_finish;
    };
}