#pragma once

#include "../core/S_Base.h"
#include "../components/C_Position.h"
#include "../components/C_Collidable.h"

namespace ragnarok
{
    struct TileInfo;
    class Map;

    /**
     * \brief Combat system
     */
    class S_Combat : public S_Base
    {
        public:
        explicit S_Combat(SystemManager* t_systemMgr);
        ~S_Combat() override = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        private:
        /**
         * \brief Determines whether target is in attack range
         * \param t_entity the entity
         * \param t_attackerPosition the attacker entity's position component
         * \param t_targetPosition the target entity's position component
         * \param t_distant whether the attack is distant
         */
        bool EntityInAttackRange(C_Position *t_attackerPosition,
                                 const C_Position *t_targetPosition,
                                 bool t_distant);

    };
}