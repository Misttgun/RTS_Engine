#pragma once

#include "../core/S_Base.h"
#include "../components/C_State.h"

namespace ragnarok
{
    /**
     * \brief State system
     */
    class S_State : public S_Base
    {
        public:
        S_State(SystemManager* t_systemMgr);
        ~S_State() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        private:
        /**
         * \brief Change an entity state
         * \param t_entity the entity
         * \param t_state the new state
         * \param t_force force the state even if the entity is dead
         */
        void ChangeState(const EntityId& t_entity, const EntityState& t_state, bool t_force) const;
    };
}