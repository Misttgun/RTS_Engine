#pragma once

#include "../core/C_Base.h"
#include <iostream>

namespace ragnarok
{
    /**
     * \brief Attack component
     */
    class C_Attack : public C_Base
    {
        public:
        C_Attack() : C_Base(Component::Attack), m_currentCooldown(0), m_targetEntityID(-1)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            int distant = 0;
            t_stream >> distant;
            m_distant = distant;
            t_stream >> m_damage;
            t_stream >> m_cooldown;
        }

        float GetCooldown() const
        {
            return m_cooldown;
        }

        void SetCooldown(float t_cooldown)
        {
            m_cooldown = t_cooldown;
        }

        int GetTargetEntity() {
            return m_targetEntityID;
        }

        bool IsDistant() {
            return m_distant;
        }

        void ResetCooldown() {
            m_currentCooldown = m_cooldown;
        }

        void SetTargetEntity(int t_entity) {
            m_targetEntityID = t_entity;
        }

        bool UpdateCooldown(float t_deltaTime) {
            if (m_currentCooldown > 0.0F)
            {
                m_currentCooldown -= t_deltaTime;
            }

            return m_currentCooldown <= 0.0F;
        }

    private:
        bool m_distant;
        unsigned int m_damage;
        float m_cooldown;
        float m_currentCooldown;
        int m_targetEntityID;
    };
}