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
        C_Attack() : C_Base(Component::Attack), m_currentCooldown(0), m_targetEntityID(-1), m_attackedEntityID(-1)
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
			int farm = 0;
			t_stream >> farm;
			m_canFarm = farm;
        }

        float GetCooldown() const
        {
            return m_cooldown;
        }

        void SetCooldown(float t_cooldown)
        {
            m_cooldown = t_cooldown;
        }

        int GetRange() const
        {
            return m_range;
        }

        int GetTargetEntity() const
        {
            return m_targetEntityID;
        }

        int GetAttackedEntity() const
        {
            return m_attackedEntityID;
        }

        bool IsDistant() const
        {
            return m_distant;
        }

		bool CanFarm() {
			return m_canFarm;
		}

        void ResetCooldown() {
            m_currentCooldown = m_cooldown;
        }

        void SetTargetEntity(unsigned int t_entity)
        {
            m_targetEntityID = t_entity;
        }

        void SetAttackedEntity(unsigned int t_entity)
        {
            m_attackedEntityID = t_entity;
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

        int GetAttackType() const
        {
            return m_attackType;
        }

        int GetDamage() const
        {
            return m_damage;
        }

    private:
        bool m_distant;
        int m_damage;
        float m_cooldown;
        int m_range;
        int m_attackType;
		bool m_canFarm;

        float m_currentCooldown;
        int m_targetEntityID;
        int m_attackedEntityID;
    };
}