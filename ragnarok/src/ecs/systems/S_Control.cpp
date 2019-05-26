#include "../../../include/ecs/systems/S_Control.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/ecs/components/C_Movable.h"
#include "../../../include/ecs/components/C_Attack.h"

namespace ragnarok
{
    S_Control::S_Control(SystemManager* t_systemMgr) : S_Base(System::Control, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::Movable));
        req.TurnOnBit(static_cast<unsigned int>(Component::Controller));
        m_requiredComponents.push_back(req);
        req.Clear();
    }

    void S_Control::Update(float t_dT)
    {}

    void S_Control::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {
        switch (t_event)
        {
            case EntityEvent::Moving_Left: MoveEntity(t_entity, Direction::Left); break;
            case EntityEvent::Moving_Right: MoveEntity(t_entity, Direction::Right); break;
            case EntityEvent::Moving_Up: MoveEntity(t_entity, Direction::Up); break;
            case EntityEvent::Moving_Down: MoveEntity(t_entity, Direction::Down); break;
            default:;
        }
    }

    void S_Control::Notify(const Message& t_message)
    {}

    void S_Control::MoveEntity(const EntityId& t_entity, const Direction& t_dir) const
    {
        auto mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(t_entity, Component::Movable);
        mov->Move(t_dir);

        auto attack = m_systemManager->GetEntityManager()->GetComponent<C_Attack>(t_entity, Component::Attack);
        if (attack != nullptr)
        {
            attack->SetTargetEntity(-1);
        }
    }
}
