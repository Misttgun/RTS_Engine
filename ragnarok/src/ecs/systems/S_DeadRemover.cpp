#include <cmath>

#include <cmath>
#include "../../../include/ecs/systems/S_DeadRemover.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/map/Map.h"
#include "../../../include/ecs/components/C_SpriteSheet.h"
#include "../../../include/ecs/components/C_Attack.h"
#include "../../../include/ecs/components/C_State.h"

namespace ragnarok
{
    S_DeadRemover::S_DeadRemover(SystemManager* t_systemMgr) : S_Base(System::DeadRemover, t_systemMgr), m_msgSent(false)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::SpriteSheet));
        m_requiredComponents.push_back(req);
        req.Clear();

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Dead, this);
    }

    void S_DeadRemover::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();

        for(auto &entity : m_dead)
        {
            entityMgr->RemoveEntity(entity);
        }
    }

    void S_DeadRemover::HandleEvent(const EntityId& t_entity,
                               const EntityEvent& t_event)
    {
        if (t_event == EntityEvent::Despawned) {
            // TODO Maybe do something here?
        }
    }

    void S_DeadRemover::Notify(const Message& t_message)
    {
        EntityManager* eMgr = m_systemManager->GetEntityManager();

        const auto type = static_cast<EntityMessage>(t_message.m_type);
        if (type == EntityMessage::Dead)
        {
            m_dead.emplace_back(t_message.m_receiver);
        }
    }
}
