#include <cmath>

#include <cmath>
#include "../../../include/ecs/systems/S_Combat.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/map/Map.h"
#include "../../../include/ecs/components/C_SpriteSheet.h"
#include "../../../include/ecs/components/C_Attack.h"
#include "../../../include/ecs/components/C_State.h"

namespace ragnarok
{
    S_Combat::S_Combat(SystemManager* t_systemMgr) : S_Base(System::Combat, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::SpriteSheet));
        req.TurnOnBit(static_cast<unsigned int>(Component::Attack));
        m_requiredComponents.push_back(req);
        req.Clear();

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack_Action, this);
    }

    /**
     * Starts attacks for entities having a target in range
     *
     * For each entity, manages attack cooldown and if the entity has a target,
     * begins attacks when in range. Also unsets target when dead.
     * @param t_dT The delta time since last frame
     */
    void S_Combat::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for(auto &entity : m_entities)
        {
            const auto position = entityMgr->GetComponent<C_Position>(entity, Component::Position);
            const auto attack = entityMgr->GetComponent<C_Attack>(entity, Component::Attack);
            const auto spriteSheet = entityMgr->GetComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
            AnimBase *currentAnim = spriteSheet->GetSpriteSheet()->GetCurrentAnim();
            int targetEntity = attack->GetTargetEntity();

            if (targetEntity == -1)
            {
                continue;
            }

            if (currentAnim->GetName() != "Attack")
            {
                if (!attack->UpdateCooldown(t_dT))
                {
                    continue;
                }

                const C_Position *targetPosition =
                        entityMgr->GetComponent<C_Position>(targetEntity,
                                                            Component::Position);

                if (targetPosition == nullptr)
                {
                    attack->SetTargetEntity(-1);
                }
                else if (EntityInAttackRange(position, targetPosition,
                                             attack->IsDistant(),
                                             attack->GetRange()))
                {
                    BeginAttack(entity);
                }
            }
        }
    }

    void S_Combat::HandleEvent(const EntityId& t_entity,
                               const EntityEvent& t_event)
    {
        if (t_event == EntityEvent::Targeted_Entity) {
            // TODO Start chasing the target, but in a different system, not here
        }
    }

    /**
     * Handles Attack_Action messages in order to execute attacks
     * @param t_message The message to handle
     */
    void S_Combat::Notify(const Message& t_message)
    {
        EntityManager* eMgr = m_systemManager->GetEntityManager();

        const auto type = static_cast<EntityMessage>(t_message.m_type);
        if (type == EntityMessage::Attack_Action)
        {
            if (!HasEntity(t_message.m_sender))
            {
                return;
            }

            const auto attack = eMgr->GetComponent<C_Attack>(t_message.m_sender,
                                                             Component::Attack);
            if (attack->IsUnderCooldown())
            {
                return;
            }

            attack->ResetCooldown();
            KillEntity(attack->GetTargetEntity()); // TODO inflict damage
            SendAttackMessage(t_message.m_sender, attack);
        }
    }

    /**
     * Determines whether a target is in an attacker's attack range
     * @param t_attackerPosition The attacker's position component
     * @param t_targetPosition The target's position component
     * @param t_distant Whether the attack is distant
     * @return True if the target is in the attacker's attack range, else false
     */
    bool S_Combat::EntityInAttackRange(C_Position *const t_attackerPosition,
                                       const C_Position *t_targetPosition,
                                       bool t_distant, unsigned int t_range) {
        const unsigned int AABBWidth = 40; // TODO Temporary, for testing

        float xDist = t_attackerPosition->GetPosition().x - t_targetPosition->GetPosition().x;
        float yDist = t_attackerPosition->GetPosition().y - t_targetPosition->GetPosition().y;
        float distance = std::sqrt(xDist*xDist + yDist*yDist);

        return distance <= t_range + 2 * AABBWidth;
    }

    /**
     * Informs the systems of the death of passed entity
     * @param t_entity The ID of the dying entity
     */
    void S_Combat::KillEntity(int t_entity) {
        //Message msg(static_cast<MessageType>(EntityMessage::Switch_State));
        //msg.m_receiver = t_entity;
        //msg.m_int = static_cast<int>(EntityState::Dying);
        //m_systemManager->GetMessageHandler()->Dispatch(msg);

        Message msg(static_cast<MessageType>(EntityMessage::Dead));
        msg.m_receiver = t_entity;
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }

    /**
     * Makes passed entity start attacking once
     * @param t_entity The entity that will start attacking
     */
    void S_Combat::BeginAttack(const EntityId &t_entity) {
        Message msg(static_cast<MessageType>(EntityMessage::Switch_State));
        msg.m_receiver = t_entity;
        msg.m_int = static_cast<int>(EntityState::Attacking);
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }

    /**
     * Informs systems that an attack was just dealt
     * @param t_sender The attacker
     * @param t_attack The target of the attack
     */
    void S_Combat::SendAttackMessage(int t_sender,
                                     C_Attack *const t_attack) {
        Message msg(static_cast<MessageType>(EntityMessage::Attack_Dealt));
        msg.m_sender = t_sender;
        msg.m_receiver = t_attack->GetTargetEntity();
        msg.m_int = static_cast<int>(0); // TODO Attack type (fire, slash, ...)
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
}