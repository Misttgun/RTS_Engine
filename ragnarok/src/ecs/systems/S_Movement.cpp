#include "../../../include/ecs/systems/S_Movement.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/map/Map.h"

namespace ragnarok
{
    S_Movement::S_Movement(SystemManager* t_systemMgr) : S_Base(System::Movement, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::Movable));
        m_requiredComponents.push_back(req);
        req.Clear();

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Is_Moving, this);

        m_gameMap = nullptr;
    }

    void S_Movement::Update(float t_dT)
    {
        if (!m_gameMap)
        {
            return;
        }

        EntityManager* entities = m_systemManager->GetEntityManager();

        for (auto &entity : m_entities)
        {
            // Get the movable and position component
            auto position = entities->GetComponent<C_Position>(entity, Component::Position);
            const auto movable = entities->GetComponent<C_Movable>(entity, Component::Movable);

            // Move the entity
            position->MoveBy(movable->GetMovement() * t_dT);

            MovementStep(t_dT, movable, position);
        }
    }

    void S_Movement::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {
        switch (t_event)
        {
            case EntityEvent::Colliding_X: StopEntity(t_entity, Axis::x); break;
            case EntityEvent::Colliding_Y: StopEntity(t_entity, Axis::y); break;
            case EntityEvent::Moving_Left: SetDirection(t_entity, Direction::Left); break;
            case EntityEvent::Moving_Right: SetDirection(t_entity, Direction::Right); break;

            case EntityEvent::Moving_Up:
            {
                const auto mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(t_entity, Component::Movable);
                if (mov->GetMovement().x == 0)
                {
                    SetDirection(t_entity, Direction::Up);
                }
            }
            break;

            case EntityEvent::Moving_Down:
            {
                const auto mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(t_entity, Component::Movable);
                if (mov->GetMovement().x == 0)
                {
                    SetDirection(t_entity, Direction::Down);
                }
            }
            break;

            default:;
        }
    }

    void S_Movement::Notify(const Message& t_message)
    {
        EntityManager* eMgr = m_systemManager->GetEntityManager();

        const auto m = static_cast<EntityMessage>(t_message.m_type);
        switch (m)
        {
            case EntityMessage::Is_Moving:
            {
                if (!HasEntity(t_message.m_receiver))
                {
                    return;
                }

                const auto movable = eMgr->GetComponent<C_Movable>(t_message.m_receiver, Component::Movable);
                if (movable->GetMovement() != sf::Vector2f(0.0f, 0.0f))
                {
                    return;
                }

                // If entity is not moving send the event to change the state to idle
                m_systemManager->AddEvent(t_message.m_receiver, static_cast<EventID>(EntityEvent::Became_Idle));
            }
            break;

            default:;
        }
    }

    void S_Movement::MovementStep(float t_dT, C_Movable* t_movable, const C_Position* t_position) const
    {
        // Can be useful if we want to change speed based on tile friction
        /*const sf::Vector2f f_coefficient = GetTileFriction(t_position->GetElevation(),
                                                     static_cast<unsigned int>(floor(t_position->GetPosition().x / Sheet::Tile_Size)),
                                                     static_cast<unsigned int>(floor(t_position->GetPosition().y / Sheet::Tile_Size)));

        const sf::Vector2f friction(t_movable->GetVelocity() * f_coefficient.x, t_movable->GetVelocity() * f_coefficient.y);*/

        // Reset the entity movement velocity
        t_movable->SetMovement(sf::Vector2f(0, 0));
    }

    void S_Movement::SetMap(Map* t_gameMap)
    {
        m_gameMap = t_gameMap;
    }

    void S_Movement::StopEntity(const EntityId& t_entity, const Axis& t_axis) const
    {
        auto movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(t_entity, Component::Movable);

        if (t_axis == Axis::x)
        {
            movable->SetMovement(sf::Vector2f(0.f, movable->GetMovement().y));
        }
        else if (t_axis == Axis::y)
        {
            movable->SetMovement(sf::Vector2f(movable->GetMovement().x, 0.f));
        }
    }

    void S_Movement::SetDirection(const EntityId& t_entity, const Direction& t_dir) const
    {
        auto movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(t_entity, Component::Movable);
        movable->SetDirection(t_dir);

        Message msg(static_cast<MessageType>(EntityMessage::Direction_Changed));
        msg.m_receiver = t_entity;
        msg.m_int = static_cast<int>(t_dir);
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
}
