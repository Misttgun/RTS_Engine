#pragma once

#include "../core/S_Base.h"
#include "../components/C_Position.h"
#include "../components/C_Attack.h"

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
        bool EntityInAttackRange(C_Position *t_attackerPosition,
                                 const C_Position *t_targetPosition,
                                 bool t_distant, unsigned int t_range) const;

        void AttackEntity(unsigned int t_damage, EntityId t_entity) const;

        void BeginAttack(const EntityId &t_entity, const C_Position *t_attackerPosition, const C_Position *t_targetPosition) const;

        void SendAttackMessage(int t_sender, C_Attack *const t_attack) const;
		void SendFarmingMessage(int t_sender, C_Attack* const t_attack);
    };
}