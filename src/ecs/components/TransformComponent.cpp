#include "TransformComponent.h"

namespace SExE
{
    TransformComponent::TransformComponent() :BaseComponent(Component::Position), m_elevation(0)
    {}

    void TransformComponent::readIn(std::stringstream & t_stream)
    {
        t_stream >> m_position.x >> m_position.y >> m_orientation.x >> m_orientation.y >> m_elevation;
    }

    void TransformComponent::setPosition(const float t_x, const float t_y)
    {
        m_position = sf::Vector2f(t_x, t_y);
    }

    void TransformComponent::setPosition(const sf::Vector2f & t_pos)
    {
        m_position = t_pos;
    }

    void TransformComponent::setOrientation(const float t_up, const float t_right)
    {
        m_orientation = sf::Vector2i(t_up, t_right);
    }

    void TransformComponent::setOrientation(const sf::Vector2i & t_orientation)
    {
        m_orientation = t_orientation;
    }

    void TransformComponent::setElevation(const float t_elevation)
    {
        m_elevation = t_elevation;
    }
}