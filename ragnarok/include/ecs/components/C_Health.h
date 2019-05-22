#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    /**
    * \brief Health component
    */
    class C_Health : public C_Base
    {
        public:
        C_Health() : C_Base(Component::Health), m_showHealth(true)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_maxHealth >> m_offset.x >> m_offset.y;
            m_health = m_maxHealth;
        }

        int GetHealth() const
        {
            return m_health;
        }

        int GetMaxHealth() const
        {
            return m_maxHealth;
        }

        void SetHealth(const int t_health)
        {
            m_health = t_health;
        }

        void SetMaxHealth(const int t_maxHealth)
        {
            m_maxHealth = t_maxHealth;
        }

        const sf::Vector2f& GetOffset() const
        {
            return m_offset;
        }

        void SetOffset(const sf::Vector2f& t_offset)
        {
            m_offset = t_offset;
        }

        void SetShowHealth(bool t_show)
        {
            m_showHealth = t_show;
        }

        bool ShowHealth() const
        {
            return m_showHealth;
        }

        private:
        int m_health;
        int m_maxHealth;
        sf::Vector2f m_offset;
        bool m_showHealth;
    };
}