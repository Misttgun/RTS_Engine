#pragma once

#include "../core/C_Base.h"

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
            t_stream >> m_range;
            t_stream >> m_attackType;
        }

        float GetCooldown() const
        {
            return m_cooldown;
        }

        void SetCooldown(float t_cooldown)
        {
            m_cooldown = t_cooldown;
        }

        unsigned int GetRange() const
        {
            return m_range;
        }

        unsigned int GetTargetEntity() const
        {
            return m_targetEntityID;
        }

        bool IsDistant() const
        {
            return m_distant;
        }

        void ResetCooldown()
        {
            m_currentCooldown = m_cooldown;
        }

        void SetTargetEntity(unsigned int t_entity)
        {
            m_targetEntityID = t_entity;
        }

        bool UpdateCooldown(float t_deltaTime)
        {
            if (m_currentCooldown > 0.0F)
            {
                m_currentCooldown -= t_deltaTime;
            }

            return m_currentCooldown <= 0.0F;
        }

        bool IsUnderCooldown() const
        {
            return m_currentCooldown > 0.0F;
        }

        unsigned int GetAttackType() const
        {
            return m_attackType;
        }

        unsigned int GetDamage() const
        {
            return m_damage;
        }

    private:
        bool m_distant;
        unsigned int m_damage;
        float m_cooldown;
        unsigned int m_range;
        unsigned int m_attackType;

        float m_currentCooldown;
        unsigned int m_targetEntityID;
    };
}