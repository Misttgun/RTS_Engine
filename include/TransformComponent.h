#pragma once
#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "BaseComponent.h"
#include <SFML/System.hpp>
#include <sstream>

namespace Engine 
{
	class TransformComponent: public BaseComponent
	{
	public:
		TransformComponent();
		~TransformComponent() = default;

		void readIn(std::stringstream& t_stream) override;

		void setPosition(const float t_x, const float t_y);
		void setPosition(const sf::Vector2f& t_pos);
		void setOrientation(const float t_up, const float t_right);
		void setOrientation(const sf::Vector2i& t_orientation);
		void setElevation(const float t_elevation);
		const sf::Vector2f& getPosition() const { return m_position; }
		const sf::Vector2i& getOrientation() const { return m_orientation; }
		unsigned int getElevation() const { return m_elevation; }

	private:
		sf::Vector2f m_position;
		sf::Vector2i m_orientation; //Orientation du sprite
		unsigned int m_elevation;
	};
}

#endif // !TRANSFORMCOMPONENT_H