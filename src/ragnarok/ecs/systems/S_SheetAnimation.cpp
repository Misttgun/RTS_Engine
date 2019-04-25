#include "ragnarok/ecs/systems/S_SheetAnimation.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/ecs/components/C_State.h"
#include "ragnarok/ecs/components/C_SpriteSheet.h"

namespace ragnarok
{
    S_SheetAnimation::S_SheetAnimation(SystemManager* t_systemMgr) : S_Base(System::SheetAnimation, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::SpriteSheet));
        req.TurnOnBit(static_cast<unsigned int>(Component::State));
        m_requiredComponents.push_back(req);

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::State_Changed, this);
    }

    void S_SheetAnimation::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for (auto &entity : m_entities)
        {
            const auto sheet = entityMgr->GetComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
            auto state = entityMgr->GetComponent<C_State>(entity, Component::State);

            // Update the spritesheet
            sheet->GetSpriteSheet()->Update(t_dT);

            // Get the current animation name
            const std::string& animName = sheet->GetSpriteSheet()->GetCurrentAnim()->GetName();
            if (animName == "Attack")
            {
                // If "Attack" animation is no longer playing, switch to idle
                if (!sheet->GetSpriteSheet()->GetCurrentAnim()->IsPlaying())
                {
                    Message msg(static_cast<MessageType>(EntityMessage::Switch_State));
                    msg.m_receiver = entity;
                    msg.m_int = static_cast<int>(EntityState::Idle);
                    m_systemManager->GetMessageHandler()->Dispatch(msg);
                }
                // "Attack" animation is still playing
                else if (sheet->GetSpriteSheet()->GetCurrentAnim()->IsInAction())
                {
                    Message msg(static_cast<MessageType>(EntityMessage::Attack_Action));
                    msg.m_sender = entity;
                    m_systemManager->GetMessageHandler()->Dispatch(msg);
                }
            }
            // If "Death" animation is playing, send a message that the entity is dead
            else if (animName == "Death" && !sheet->GetSpriteSheet()->GetCurrentAnim()->IsPlaying())
            {
                Message msg(static_cast<MessageType>(EntityMessage::Dead));
                msg.m_receiver = entity;
                m_systemManager->GetMessageHandler()->Dispatch(msg);
            }

            // Notify interested system that the current animation frame has changed
            if (sheet->GetSpriteSheet()->GetCurrentAnim()->CheckMoved())
            {
                const int frame = sheet->GetSpriteSheet()->GetCurrentAnim()->GetFrame();
                Message msg(static_cast<MessageType>(EntityMessage::Frame_Change));
                msg.m_receiver = entity;
                msg.m_int = frame;
                m_systemManager->GetMessageHandler()->Dispatch(msg);
            }
        }
    }

    void S_SheetAnimation::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {}

    void S_SheetAnimation::Notify(const Message& t_message)
    {
        if (!HasEntity(t_message.m_receiver))
        {
            return;
        }

        const auto m = static_cast<EntityMessage>(t_message.m_type);
        switch (m)
        {
            case EntityMessage::State_Changed:
            {
                const auto s = static_cast<EntityState>(t_message.m_int);
                switch (s)
                {
                    case EntityState::Idle:
                        ChangeAnimation(t_message.m_receiver, "Idle", true, true);
                        break;
                    case EntityState::Walking:
                        ChangeAnimation(t_message.m_receiver, "Walk", true, true);
                        break;
                    case EntityState::Attacking:
                        ChangeAnimation(t_message.m_receiver, "Attack", true, false);
                        break;
                    case EntityState::Hurt: break;
                    case EntityState::Dying:
                        ChangeAnimation(t_message.m_receiver, "Death", true, false);
                        break;
                }
            }
            break;

            default:;
        }
    }

    void S_SheetAnimation::ChangeAnimation(const EntityId& t_entity, const std::string& t_anim, bool t_play, bool t_loop) const
    {
        const auto sheet = m_systemManager->GetEntityManager()->GetComponent<C_SpriteSheet>(t_entity, Component::SpriteSheet);
        sheet->GetSpriteSheet()->SetAnimation(t_anim, t_play, t_loop);
    }
}