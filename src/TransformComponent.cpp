#include "TransformComponent.h"

Engine::TransformComponent::TransformComponent()
	:BaseComponent(Component::Position), m_elevation(0)
{}

void Engine::TransformComponent::ReadIn(std::stringstream & t_stream)
{
	t_stream >> m_position.x >> m_position.y >> m_orientation.x >> m_orientation.y >> m_elevation;
}

void Engine::TransformComponent::setPosition(const float t_x, const float t_y)
{
	m_position = sf::Vector2f(t_x, t_y);
}

void Engine::TransformComponent::setPosition(const sf::Vector2f & t_pos)
{
	m_position = t_pos;
}

void Engine::TransformComponent::setOrientation(const float t_up, const float t_right)
{
	m_orientation = sf::Vector2i(t_up, t_right);
}

void Engine::TransformComponent::setOrientation(const sf::Vector2i & t_orientation)
{
	m_orientation = t_orientation;
}

void Engine::TransformComponent::setElevation(const float t_elevation)
{
	m_elevation = t_elevation;
}
