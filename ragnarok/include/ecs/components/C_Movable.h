#pragma once

#include <SFML/System/Vector2.hpp>
#include "../core/C_Base.h"
#include "../../types/Directions.h"

namespace ragnarok
{
    /**
     * \brief Movement component
     */
    class C_Movable : public C_Base
    {
    public:
        C_Movable() : C_Base(Component::Movable), m_velocity(0.f), m_direction(static_cast<Direction>(0))
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_velocity;

            unsigned int dir = 0;
            t_stream >> dir;

            m_direction = static_cast<Direction>(dir);
        }

        float GetVelocity() const
        {
            return m_velocity;
        }

        Direction GetDirection() const
        {
            return m_direction;
        }

        sf::Vector2f GetMovement() const
        {
            return m_movement;
        }

        void SetVelocity(float t_vel)
        {
            m_velocity = t_vel;
        }

        void SetDirection(const Direction& t_dir)
        {
            m_direction = t_dir;
        }

        void SetMovement(const sf::Vector2f& t_vec)
        {
            m_movement = t_vec;
        }

        void Move(const Direction& t_dir)
        {
            if (t_dir == Direction::Up)
            {
                m_movement.y = -m_velocity;
            }
            else if (t_dir == Direction::Down)
            {
                m_movement.y = m_velocity;
            }
            else if (t_dir == Direction::Left)
            {
                m_movement.x = -m_velocity;
            }
            else if (t_dir == Direction::Right)
            {
                m_movement.x = m_velocity;
            }
        }

    private:
        float m_velocity;
        sf::Vector2f m_movement;
        Direction m_direction;
    };
}