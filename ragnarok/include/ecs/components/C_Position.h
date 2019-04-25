#pragma once

#include <SFML/System/Vector2.hpp>
#include "../core/C_Base.h"

namespace ragnarok
{
    /**
     * \brief The position component
     */
    class C_Position : public C_Base
    {
        public:
        C_Position() : C_Base(Component::Position), m_elevation(0)
        {}

        ~C_Position() = default;

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_position.x >> m_position.y >> m_elevation;
        }

        sf::Vector2f GetPosition() const
        {
            return m_position;
        }

        sf::Vector2f GetOldPosition() const
        {
            return m_positionOld;
        }

        unsigned int GetElevation() const
        {
            return m_elevation;
        }

        void SetPosition(float t_x, float t_y)
        {
            m_positionOld = m_position;
            m_position = sf::Vector2f(t_x, t_y);
        }

        void SetPosition(const sf::Vector2f& t_vec)
        {
            m_positionOld = m_position;
            m_position = t_vec;
        }

        void SetElevation(unsigned int t_elevation)
        {
            m_elevation = t_elevation;
        }

        void MoveBy(float t_x)
        {
            MoveBy(sf::Vector2f(t_x, t_x));
        }

        void MoveBy(const sf::Vector2f& t_vec)
        {
            m_positionOld = m_position;
            m_position += t_vec;
        }

        private:
        sf::Vector2f m_position;
        sf::Vector2f m_positionOld;
        unsigned int m_elevation; // represent how high the entity is in relation to the map
    };
}