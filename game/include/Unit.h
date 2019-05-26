#pragma once

class Unit{
	public:
		Unit(float t_x, float t_y) {
			m_destination = sf::Vector2f(t_x, t_y);
		};
		~Unit() = default;

		sf::Vector2f getDestination()
		{
			return m_destination;
		};

		void setDestination(float t_x, float t_y)
		{
			m_destination = sf::Vector2f(t_x, t_y);
		};

		void setDestination(const sf::Vector2f t_direction)
		{
			m_destination = sf::Vector2f(t_direction.x, t_direction.y);
		};

	private:
		sf::Vector2f m_destination;
};