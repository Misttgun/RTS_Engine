#include <cmath>

#include <cmath>
#include "../../../include/ecs/systems/S_Combat.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/map/Map.h"
#include "../../../include/ecs/components/C_SpriteSheet.h"
#include "../../../include/ecs/components/C_State.h"
#include "../../../include/ecs/components/C_Movable.h"
#include "../../../include/ecs/components/C_Health.h"
#include "../../../include/ecs/components/C_Ressource.h"

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

        for (auto &entity : m_entities)
        {
            const auto position = entityMgr->GetComponent<C_Position>(entity, Component::Position);
            const auto attack = entityMgr->GetComponent<C_Attack>(entity, Component::Attack);
            const auto spriteSheet = entityMgr->GetComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
            AnimBase *currentAnim = spriteSheet->GetSpriteSheet()->GetCurrentAnim();
            const int targetEntity = attack->GetTargetEntity();

            if (!attack->UpdateCooldown(t_dT))
            {
                continue;
            }

            if (targetEntity == -1)
            {
                continue;
            }

            if (currentAnim->GetName() != "Attack")
            {
                const C_Position *targetPosition =
                    entityMgr->GetComponent<C_Position>(targetEntity,
                                                        Component::Position);

                if (targetPosition == nullptr)
                {
                    attack->SetTargetEntity(-1);
                }
                else if (EntityInAttackRange(position, targetPosition, attack->IsDistant(), attack->GetRange()))
                {
                    attack->SetAttackedEntity(targetEntity);
                    BeginAttack(entity, position, targetPosition);
                }
            }
        }
    }

    void S_Combat::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {
        if (t_event == EntityEvent::Targeted_Entity)
        {
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

            const auto attack = eMgr->GetComponent<C_Attack>(t_message.m_sender, Component::Attack);
            if (attack->IsUnderCooldown())
            {
                return;
            }

            attack->ResetCooldown();

            AttackEntity(attack->GetDamage(), attack->GetAttackedEntity()); // TODO inflict damage
            //SendAttackMessage(t_message.m_sender, attack);
            if (attack->CanFarm())
            {
                //SendFarmingMessage(t_message.m_sender, attack);
                FarmRessource(attack->GetAttackedEntity());
            }
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
                                       bool t_distant, unsigned int t_range) const
    {
        int attackerX = t_attackerPosition->GetMapPosition().x;
        int attackerY = t_attackerPosition->GetMapPosition().y;
        int targetX = t_targetPosition->GetMapPosition().x;
        int targetY = t_targetPosition->GetMapPosition().y;

        int xDist = attackerX - targetX;
        int yDist = attackerY - targetY;
        int distance = std::abs(xDist) + std::abs(yDist);

        return distance <= t_range;
    }

    /**
     * Attack the passed entity
     * @param t_damage The damage dealt by the attacker
     * @param t_entity The ID of the entity
     */
    void S_Combat::AttackEntity(unsigned int t_damage, EntityId t_entity) const
    {
        EntityManager* eMgr = m_systemManager->GetEntityManager();
        auto hcomp = eMgr->GetComponent<C_Health>(t_entity, Component::Health);

        if (!hcomp)
            return;

        int newHealth = hcomp->GetHealth() - static_cast<int>(t_damage);
        if (newHealth < 0)
            newHealth = 0;
        hcomp->SetHealth(newHealth);

        if (newHealth == 0)
        {
            Message msg(static_cast<MessageType>(EntityMessage::Dead));
            msg.m_receiver = t_entity;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
    }

    /**
     * Farm the passed ressource
     * @param t_entity The ID of the ressource entity
     */
    void S_Combat::FarmRessource(EntityId t_entity) const
    {
        EntityManager* eMgr = m_systemManager->GetEntityManager();
        const auto rescomp = eMgr->GetComponent<C_Ressource>(t_entity, Component::Ressource);

        if(!rescomp)
            return;

        Message msg(static_cast<MessageType>(EntityMessage::Farming));
        msg.m_receiver = t_entity;
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }

    /**
     * Makes passed entity start attacking once
     * @param t_entity The entity that will start attacking
     */
    void S_Combat::BeginAttack(const EntityId &t_entity, const C_Position *t_attackerPosition, const C_Position *t_targetPosition) const
    {
        const sf::Vector2f delta = t_targetPosition->GetPosition() - t_attackerPosition->GetPosition();


        Message directionMessage(static_cast<MessageType>(EntityMessage::Direction_Changed));
        directionMessage.m_receiver = t_entity;
        if (static_cast<int>(delta.x) > 0)
        {
            directionMessage.m_int = static_cast<int>(ragnarok::Direction::Right);
        }
        else if (static_cast<int>(delta.x) < 0)
        {
            directionMessage.m_int = static_cast<int>(ragnarok::Direction::Left);
        }
        else if (static_cast<int>(delta.y) > 0)
        {
            directionMessage.m_int = static_cast<int>(ragnarok::Direction::Down);
        }
        else if (static_cast<int>(delta.y) < 0)
        {
            directionMessage.m_int = static_cast<int>(ragnarok::Direction::Up);
        }
        m_systemManager->GetMessageHandler()->Dispatch(directionMessage);

        Message stateMessage(static_cast<MessageType>(EntityMessage::Switch_State));
        stateMessage.m_receiver = t_entity;
        stateMessage.m_int = static_cast<int>(EntityState::Attacking);
        m_systemManager->GetMessageHandler()->Dispatch(stateMessage);
    }

    /**
     * Informs systems that an attack was just dealt
     * @param t_sender The attacker
     * @param t_attack The target of the attack
     */
    void S_Combat::SendAttackMessage(int t_sender, C_Attack *const t_attack) const
    {
        Message msg(static_cast<MessageType>(EntityMessage::Attack_Dealt));
        msg.m_sender = t_sender;
        msg.m_receiver = t_attack->GetTargetEntity();
        msg.m_int = static_cast<int>(t_attack->GetAttackType());
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }

    /**
     * Informs systems that a unit is farming
     * @param t_sender The attacker
     * @param t_attack The target of the attack
     */
    void S_Combat::SendFarmingMessage(int t_sender, C_Attack* const t_attack)
    {
        Message msg(static_cast<MessageType>(EntityMessage::Farming));
        msg.m_sender = t_sender;
        msg.m_receiver = t_attack->GetTargetEntity();
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
}
