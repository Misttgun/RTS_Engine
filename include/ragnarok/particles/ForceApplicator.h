#pragma once

#include <SFML/System/Vector3.hpp>

namespace ragnarok
{
    struct ForceApplicator
    {
        ForceApplicator(const sf::Vector3f& t_center, const sf::Vector3f& t_force, float t_radius)
            : m_center(t_center), m_force(t_force), m_radius(t_radius)
        {}

        sf::Vector3f m_center;
        sf::Vector3f m_force;
        float m_radius;
    };
}