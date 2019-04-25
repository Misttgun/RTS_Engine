#include "../../../include/ecs/systems/S_State.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/types/Directions.h"

namespace ragnarok
{
    S_State::S_State(SystemManager* t_systemMgr) : S_Base(System::State, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::State));
        m_requiredComponents.push_back(req);

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Switch_State, this);
    }

    void S_State::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for (auto &entity : m_entities)
        {
            const auto state = entityMgr->GetComponent<C_State>(entity, Component::State);
            if (state->GetState() == EntityState::Walking)
            {
                // Send a message that the entity is moving
                Message msg(static_cast<MessageType>(EntityMessage::Is_Moving));
                msg.m_receiver = entity;
                m_systemManager->GetMessageHandler()->Dispatch(msg);
            }
        }
    }

    void S_State::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {
        switch (t_event)
        {
            case EntityEvent::Became_Idle:
                ChangeState(t_entity, EntityState::Idle, false);
                break;

            default:;
        }
    }

    void S_State::Notify(const Message& t_message)
    {
        if (!HasEntity(t_message.m_receiver))
        {
            return;
        }

        const auto m = static_cast<EntityMessage>(t_message.m_type);

        switch (m)
        {
            case EntityMessage::Move:
            {
                const auto state = m_systemManager->GetEntityManager()->GetComponent<C_State>(t_message.m_receiver, Component::State);

                if (state->GetState() == EntityState::Dying)
                {
                    return;
                }

                EntityEvent e = {};
                const auto dir = static_cast<Direction>(t_message.m_int);

                if (dir == Direction::Up)
                {
                    e = EntityEvent::Moving_Up;
                }
                else if (dir == Direction::Down)
                {
                    e = EntityEvent::Moving_Down;
                }
                else if (dir == Direction::Left)
                {
                    e = EntityEvent::Moving_Left;
                }
                else if (dir == Direction::Right)
                {
                    e = EntityEvent::Moving_Right;
                }

                m_systemManager->AddEvent(t_message.m_receiver, static_cast<EventID>(e));
                ChangeState(t_message.m_receiver, EntityState::Walking, false);
            }
            break;

            case EntityMessage::Switch_State:
                ChangeState(t_message.m_receiver, static_cast<EntityState>(t_message.m_int), false);
                break;

            default:;
        }
    }

    void S_State::ChangeState(const EntityId& t_entity, const EntityState& t_state, bool t_force) const
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        auto state = entityMgr->GetComponent<C_State>(t_entity, Component::State);
        if (!t_force && state->GetState() == EntityState::Dying)
        {
            return;
        }

        state->SetState(t_state);
        Message msg(static_cast<MessageType>(EntityMessage::State_Changed));
        msg.m_receiver = t_entity;
        msg.m_int = static_cast<int>(t_state);
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
}
