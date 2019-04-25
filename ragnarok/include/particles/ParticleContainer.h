#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "../resources/TextureManager.h"

namespace ragnarok
{

    class ParticleContainer
    {
    public:
        ParticleContainer(TextureManager* t_textureManager) : m_countAlive(0), m_textureManager(t_textureManager)
        {
            Reset();
        }

        ~ParticleContainer()
        {
            for (size_t i = 0; i < Max_Particles; ++i)
            {
                if (m_texture[i].empty())
                {
                    continue;
                }

                m_textureManager->ReleaseResource(m_texture[i]);
            }
        }


        /**
         * \brief Reset all of the allocated memory for the particles
         */
        void Reset()
        {
            for (size_t i = 0; i < Max_Particles; ++i)
            {
                ResetParticle(i);
            }

            m_countAlive = 0;
        }

        void Enable(size_t t_id)
        {
            if (m_countAlive >= Max_Particles)
            {
                return;
            }

            m_alive[t_id] = true;
            Swap(t_id, m_countAlive);
            ++m_countAlive;
        }

        void Disable(size_t t_id)
        {
            if (!m_countAlive)
            {
                return;
            }

            ResetParticle(t_id);
            Swap(t_id, m_countAlive - 1);
            --m_countAlive;
        }

        const static size_t Max_Particles = 3000;

        sf::Vector3f m_position[Max_Particles];
        sf::Vector3f m_velocity[Max_Particles];
        sf::Vector3f m_acceleration[Max_Particles];
        sf::Vector2f m_startSize[Max_Particles];
        sf::Vector2f m_currentSize[Max_Particles];
        sf::Vector2f m_finalSize[Max_Particles];
        sf::Color m_startColor[Max_Particles];
        sf::Color m_currentColor[Max_Particles];
        sf::Color m_finalColor[Max_Particles];
        std::string m_texture[Max_Particles];
        sf::RectangleShape m_drawable[Max_Particles];
        float m_startRotation[Max_Particles];
        float m_currentRotation[Max_Particles];
        float m_finalRotation[Max_Particles];
        float m_lifespan[Max_Particles];
        float m_maxLifespan[Max_Particles];
        bool m_gravity[Max_Particles];
        bool m_addBlend[Max_Particles];
        bool m_alive[Max_Particles];

        size_t m_countAlive; // Count of particules still alive
        TextureManager* m_textureManager;

    protected:
        void Swap(size_t t_first, size_t t_second)
        {
            std::swap(m_position[t_first], m_position[t_second]);
            std::swap(m_velocity[t_first], m_velocity[t_second]);
            std::swap(m_acceleration[t_first], m_acceleration[t_second]);
            std::swap(m_startSize[t_first], m_startSize[t_second]);
            std::swap(m_currentSize[t_first], m_currentSize[t_second]);
            std::swap(m_finalSize[t_first], m_finalSize[t_second]);
            std::swap(m_startColor[t_first], m_startColor[t_second]);
            std::swap(m_currentColor[t_first], m_currentColor[t_second]);
            std::swap(m_finalColor[t_first], m_finalColor[t_second]);
            std::swap(m_texture[t_first], m_texture[t_second]);
            std::swap(m_drawable[t_first], m_drawable[t_second]);
            std::swap(m_startRotation[t_first], m_startRotation[t_second]);
            std::swap(m_currentRotation[t_first], m_currentRotation[t_second]);
            std::swap(m_finalRotation[t_first], m_finalRotation[t_second]);
            std::swap(m_lifespan[t_first], m_lifespan[t_second]);
            std::swap(m_maxLifespan[t_first], m_maxLifespan[t_second]);
            std::swap(m_gravity[t_first], m_gravity[t_second]);
            std::swap(m_addBlend[t_first], m_addBlend[t_second]);
            std::swap(m_alive[t_first], m_alive[t_second]);
        }

        void ResetParticle(size_t t_id)
        {
            m_alive[t_id] = false;
            m_gravity[t_id] = false;
            m_addBlend[t_id] = false;
            m_lifespan[t_id] = 0.f;
            m_maxLifespan[t_id] = 0.f;
            m_position[t_id] = { 0.f, 0.f, 0.f };
            m_velocity[t_id] = { 0.f, 0.f, 0.f };
            m_acceleration[t_id] = { 0.f, 0.f, 0.f };
            m_startRotation[t_id] = 0.f;
            m_currentRotation[t_id] = 0.f;
            m_finalRotation[t_id] = 0.f;
            m_startSize[t_id] = { 0.f, 0.f };
            m_currentSize[t_id] = { 0.f, 0.f };
            m_finalSize[t_id] = { 0.f, 0.f };
            m_startColor[t_id] = { 0, 0, 0, 0 };
            m_currentColor[t_id] = { 0, 0, 0, 0 };
            m_finalColor[t_id] = { 0, 0, 0, 0 };

            if (!m_texture[t_id].empty())
            {
                m_textureManager->ReleaseResource(m_texture[t_id]);
                m_texture[t_id].clear();
                m_drawable[t_id].setTexture(nullptr);
            }
        }
    };
}