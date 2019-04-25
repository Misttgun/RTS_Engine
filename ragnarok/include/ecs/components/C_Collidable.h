#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "../core/C_Base.h"

namespace ragnarok
{
    enum class Origin
    {
        Top_Left, Abs_Centre, Mid_Bottom
    };

    /**
     * \brief Collision component
     */
    class C_Collidable : public C_Base
    {
        public:
        C_Collidable() : C_Base(Component::Collidable), m_origin(Origin::Mid_Bottom), m_collidingOnX(false), m_collidingOnY(false)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            unsigned int origin = 0;
            t_stream >> m_AABB.width >> m_AABB.height >> m_offset.x >> m_offset.y >> origin;
            m_origin = static_cast<Origin>(origin);
        }

        const sf::FloatRect& GetCollidable() const
        {
            return m_AABB;
        }

        bool IsCollidingOnX() const
        {
            return m_collidingOnX;
        }

        bool IsCollidingOnY() const
        {
            return m_collidingOnY;
        }

        void CollideOnX()
        {
            m_collidingOnX = true;
        }

        void CollideOnY()
        {
            m_collidingOnY = true;
        }

        void ResetCollisionFlags()
        {
            m_collidingOnX = false;
            m_collidingOnY = false;
        }

        void SetCollidable(const sf::FloatRect& t_rect)
        {
            m_AABB = t_rect;
        }

        void SetOrigin(const Origin& t_origin)
        {
            m_origin = t_origin;
        }

        void SetSize(const sf::Vector2f& t_vec)
        {
            m_AABB.width = t_vec.x;
            m_AABB.height = t_vec.y;
        }

        /**
         * \brief Sets the position of the collision box
         * \param t_vec position
         */
        void SetPosition(const sf::Vector2f& t_vec)
        {
            switch(m_origin)
            {
                case(Origin::Top_Left):
                    m_AABB.left = t_vec.x + m_offset.x;
                    m_AABB.top = t_vec.y + m_offset.y;
                    break;
                case(Origin::Abs_Centre):
                    m_AABB.left = t_vec.x - (m_AABB.width / 2) + m_offset.x;
                    m_AABB.top = t_vec.y - (m_AABB.height / 2) + m_offset.y;
                    break;
                case(Origin::Mid_Bottom):
                    m_AABB.left = t_vec.x - (m_AABB.width / 2) + m_offset.x;
                    m_AABB.top = t_vec.y - m_AABB.height + m_offset.y;
                    break;
            }
        }

        private:
        sf::FloatRect m_AABB;
        sf::Vector2f m_offset;
        Origin m_origin;

        bool m_collidingOnX;
        bool m_collidingOnY;
    };
}