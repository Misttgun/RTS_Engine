#pragma once

#include <SFML/Graphics.hpp>
#include "TileMap.h"

namespace ragnarok
{
	class DiscreteMap
	{
	public:
		DiscreteMap(sf::Vector2u t_mapSize)
		{
			m_squareSize = 32;

			m_mapSize.x = t_mapSize.x;
			m_mapSize.y = t_mapSize.y;
			m_mapArray = new int* [m_mapSize.y];
			for (unsigned int j = 0; j < m_mapSize.y; ++j)
			{
				m_mapArray[j] = new int[m_mapSize.x];
				for (unsigned int i = 0; i < m_mapSize.x; ++i)
					m_mapArray[j][i] = -1;
			}
		}

		~DiscreteMap(){}

		int isEmpty(int t_x, int t_y)
		{
			if (t_x >= m_mapSize.x * m_squareSize || t_y >= m_mapSize.y * m_squareSize || t_x < 0 || t_y < 0)
				return -2;
			return m_mapArray[t_y / m_squareSize][t_x / m_squareSize];
		}

		sf::Vector2i getSquareCenter(sf::Vector2i t_pos)
		{
			int t_x = t_pos.x;
			int t_y = t_pos.y;
			if (t_x >= m_mapSize.x * m_squareSize || t_y >= m_mapSize.y * m_squareSize || t_x < 0 || t_y < 0)
				return sf::Vector2i(-1,-1);
			int t_squaredX = t_x / m_squareSize;
			int t_squaredY = t_y / m_squareSize;
			return sf::Vector2i(t_squaredX * m_squareSize + m_squareSize / 2, t_squaredY * m_squareSize + m_squareSize / 2);
		}

		sf::Vector2i setSquare(int t_x, int t_y, int t_value)
		{
			if (t_x >= m_mapSize.x * m_squareSize || t_y >= m_mapSize.y * m_squareSize || t_x < 0 || t_y < 0)
				return sf::Vector2i(-1,-1);
			int t_squaredX = t_x / m_squareSize;
			int t_squaredY = t_y / m_squareSize;
			m_mapArray[t_squaredY][t_squaredX] = t_value;
			return sf::Vector2i(t_squaredX * m_squareSize + m_squareSize / 2, t_squaredY * m_squareSize + m_squareSize / 2);
		}

		void updateMapSize(sf::Vector2u t_mapSize)
		{
			if (t_mapSize.x != m_mapSize.x || t_mapSize.y != m_mapSize.y)
			{
				Purge();

				m_mapSize.x = t_mapSize.x;
				m_mapSize.y = t_mapSize.y;
				m_mapArray = new int* [m_mapSize.y];
				for (unsigned int j = 0; j < m_mapSize.y; ++j)
				{
					m_mapArray[j] = new int[m_mapSize.x];
					for (unsigned int i = 0; i < m_mapSize.x; ++i)
						m_mapArray[j][i] = -1;
				}
			}
		}

		void Purge()
		{
			for (unsigned int j = 0; j < m_mapSize.y; ++j)
				delete m_mapArray[j];
			delete m_mapArray;
		}

	protected:
		sf::Vector2u m_mapSize;
		int** m_mapArray;
		int m_squareSize;
	};
}