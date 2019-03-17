#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    enum class EntityState
    {
        Idle, Walking, Attacking, Hurt, Dying
    };

    /**
     * \brief State component
     */
    class C_State : public C_Base
    {
        public:
        C_State() : C_Base(Component::State)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            unsigned int state = 0;
            t_stream >> state;
            m_state = static_cast<EntityState>(state);
        }

        EntityState GetState() const
        {
            return m_state;
        }

        void SetState(const EntityState& t_state)
        {
            m_state = t_state;
        }

        private:
        EntityState m_state;
    };
}