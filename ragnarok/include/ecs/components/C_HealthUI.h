#pragma once

#include "../core/C_Base.h"
#include <SFML/System/Vector2.hpp>

namespace ragnarok
{
    /**
    * \brief Health component
    */
    class C_HealthUI : public C_Base
    {
        public:
        C_HealthUI() : C_Base(Component::Health)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_offset.x >> m_offset.y;
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
        sf::Vector2f m_offset;
        bool m_showHealth;
    };
}