#include "ragnarok/ecs/systems/S_Sound.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/ecs/components/C_Position.h"
#include "ragnarok/ecs/components/C_State.h"
#include "ragnarok/types/Directions.h"
#include "ragnarok/map/Map.h"

namespace ragnarok
{
    S_Sound::S_Sound(SystemManager* t_systemMgr) : S_Base(System::Sound, t_systemMgr), m_audioManager(nullptr), m_soundManager(nullptr)
    {
        // Requirements an entity must meet to belong to this system. In this case we have two possible versions.
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::SoundEmitter));
        m_requiredComponents.push_back(req);

        req.ClearBit(static_cast<unsigned int>(Component::SoundEmitter));
        req.TurnOnBit(static_cast<unsigned int>(Component::SoundListener));
        m_requiredComponents.push_back(req);

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Frame_Change, this);
    }


    void S_Sound::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for (auto &entity : m_entities)
        {
            // Get and store entity position and elevation
            const auto c_pos = entityMgr->GetComponent<C_Position>(entity, Component::Position);
            auto position = c_pos->GetPosition();
            const auto elevation = c_pos->GetElevation();

            // If the entity is a sound listener, set the listener position
            const auto IsListener = entityMgr->HasComponent(entity, Component::SoundListener);
            if (IsListener)
            {
                sf::Listener::setPosition(MakeSoundPosition(position, elevation));
            }

            // Entity is not a sound emitter
            if (!entityMgr->HasComponent(entity, Component::SoundEmitter))
            {
                continue;
            }

            // The sound is currently being played (ID != -1)
            auto c_snd = entityMgr->GetComponent<C_SoundEmitter>(entity, Component::SoundEmitter);
            if (c_snd->GetSoundID() == -1)
            {
                continue;
            }

            // If entity is not a sound listener, update sound position
            if (!IsListener)
            {
                if (!m_soundManager->SetPosition(c_snd->GetSoundID(), MakeSoundPosition(position, elevation)))
                {
                    c_snd->SetSoundID(-1);
                }
            }
            // If entity is a sound listener, check if the sound is still being played
            else
            {
                if (!m_soundManager->IsPlaying(c_snd->GetSoundID()))
                {
                    c_snd->SetSoundID(-1);
                }
            }
        }
    }

    void S_Sound::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {}

    void S_Sound::Notify(const Message& t_message)
    {
        if (!HasEntity(t_message.m_receiver))
        {
            return;
        }

        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        const auto IsListener = entityMgr->HasComponent(t_message.m_receiver, Component::SoundListener);

        const auto m = static_cast<EntityMessage>(t_message.m_type);
        switch (m)
        {
            case EntityMessage::Direction_Changed:
            {
                if (!IsListener)
                {
                    return;
                }

                // If sound listener, change sound direction
                const auto dir = static_cast<Direction>(t_message.m_int);
                switch (dir)
                {
                    case Direction::Up: sf::Listener::setDirection(0, 0, -1); break;
                    case Direction::Down: sf::Listener::setDirection(0, 0, 1); break;
                    case Direction::Left: sf::Listener::setDirection(-1, 0, 0); break;
                    case Direction::Right: sf::Listener::setDirection(1, 0, 0); break;
                }
            }
            break;
            case EntityMessage::Frame_Change:
                if (!entityMgr->HasComponent(t_message.m_receiver, Component::SoundEmitter))
                {
                    return;
                }

                const auto state = entityMgr->GetComponent<C_State>(t_message.m_receiver, Component::State)->GetState();
                auto sound = EntitySound::None;

                if (state == EntityState::Walking)
                {
                    sound = EntitySound::Footstep;
                }
                else if (state == EntityState::Attacking)
                {
                    sound = EntitySound::Attack;
                }
                else if (state == EntityState::Hurt)
                {
                    sound = EntitySound::Hurt;
                }
                else if (state == EntityState::Dying)
                {
                    sound = EntitySound::Death;
                }

                if (sound == EntitySound::None)
                {
                    return;
                }

                EmitSound(t_message.m_receiver, sound, false, IsListener, t_message.m_int);
                break;
        }
    }

    void S_Sound::SetUp(AudioManager* t_audioManager, SoundManager* t_soundManager)
    {
        m_audioManager = t_audioManager;
        m_soundManager = t_soundManager;
    }

    sf::Vector3f S_Sound::MakeSoundPosition(const sf::Vector2f& t_entityPos, unsigned int t_elevation) const
    {
        return { t_entityPos.x, static_cast<float>(t_elevation * Sheet::Tile_Size), t_entityPos.y };
    }

    void S_Sound::EmitSound(const EntityId& t_entity, const EntitySound& t_sound, bool t_useId, bool t_relative, int t_checkFrame) const
    {
        if (!HasEntity(t_entity))
        {
            return;
        }

        if (!m_systemManager->GetEntityManager()->HasComponent(t_entity, Component::SoundEmitter))
        {
            return;
        }

        // Is a sound emitter.
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        auto c_snd = entityMgr->GetComponent<C_SoundEmitter>(t_entity, Component::SoundEmitter);
        if (c_snd->GetSoundID() != -1 && t_useId)
        {
            return;
        }

        // If sound is free or use of ID isn't required.
        if (t_checkFrame != -1 && !c_snd->IsSoundFrame(t_sound, t_checkFrame))
        {
            return;
        }

        // Frame is irrelevant or correct.
        const auto c_pos = entityMgr->GetComponent<C_Position>(t_entity, Component::Position);
        const auto pos = (t_relative ? sf::Vector3f(0.f, 0.f, 0.f) : MakeSoundPosition(c_pos->GetPosition(), c_pos->GetElevation()));
        
        if (t_useId)
        {
            c_snd->SetSoundID(m_soundManager->Play(c_snd->GetSound(t_sound), pos));
        }
        else
        {
            m_soundManager->Play(c_snd->GetSound(t_sound), pos, false, t_relative);
        }
    }
}