#pragma once

#include "../BaseUpdater.h"
#include "../ParticleContainer.h"
#include "../../Utilities/Utilities.h"

namespace ragnarok
{
    class Interpolator : public BaseUpdater
    {
    public:
        void Update(float t_dT, ParticleContainer* t_particles) override
        {
            auto& startColors = t_particles->m_startColor;
            auto& currentColors = t_particles->m_currentColor;
            auto& finalColors = t_particles->m_finalColor;
            auto& lifespans = t_particles->m_lifespan;
            auto& maxLifespans = t_particles->m_maxLifespan;
            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (startColors[i] == finalColors[i])
                {
                    continue;
                }

                currentColors[i].r = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].r, finalColors[i].r, lifespans[i]);
                currentColors[i].g = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].g, finalColors[i].g, lifespans[i]);
                currentColors[i].b = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].b, finalColors[i].b, lifespans[i]);
                currentColors[i].a = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].a, finalColors[i].a, lifespans[i]);
            }

            auto& startRotations = t_particles->m_startRotation;
            auto& currentRotations = t_particles->m_currentRotation;
            auto& finalRotations = t_particles->m_finalRotation;

            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (startRotations[i] == finalRotations[i])
                {
                    continue;
                }

                currentRotations[i] = Utils::Interpolate<float>(0.f, maxLifespans[i], startRotations[i], finalRotations[i], lifespans[i]);
            }

            auto& startSizes = t_particles->m_startSize;
            auto& currentSizes = t_particles->m_currentSize;
            auto& finalSizes = t_particles->m_finalSize;

            for (size_t i = 0; i < t_particles->m_countAlive; ++i)
            {
                if (startSizes[i] == finalSizes[i])
                {
                    continue;
                }

                currentSizes[i] = sf::Vector2f(
                    Utils::Interpolate<float>(0.f, maxLifespans[i], startSizes[i].x, finalSizes[i].x, lifespans[i]),
                    Utils::Interpolate<float>(0.f, maxLifespans[i], startSizes[i].y, finalSizes[i].y, lifespans[i]));
            }
        }
    };
}