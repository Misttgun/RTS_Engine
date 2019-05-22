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

        unsigned int GetRange() const
        {
            return m_range;
        }

        int GetTargetEntity() {
            return m_targetEntityID;
        }

        bool IsDistant() {
            return m_distant;
        }

		bool CanFarm() {
			return m_canFarm;
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

        bool IsUnderCooldown() {
            return m_currentCooldown > 0.0F;
        }

        unsigned int GetAttackType() {
            return m_attackType;
        }

    private:
        bool m_distant;
        unsigned int m_damage;
        float m_cooldown;
        unsigned int m_range;
        unsigned int m_attackType;
		bool m_canFarm;

        float m_currentCooldown;
        int m_targetEntityID;
    };
}