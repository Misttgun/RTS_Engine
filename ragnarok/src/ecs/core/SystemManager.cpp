#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/ecs/core/EntityManager.h"
#include "../../../include/ecs/systems/S_Renderer.h"
#include "../../../include/ecs/systems/S_EntityUI.h"

namespace ragnarok
{
    SystemManager::SystemManager() : m_entityManager(nullptr)
    {}

    SystemManager::~SystemManager()
    {
        PurgeSystems();
    }

    void SystemManager::SetEntityManager(EntityManager* t_entityMgr)
    {
        if(!m_entityManager)
        {
            m_entityManager = t_entityMgr;
        }
    }

    EntityManager* SystemManager::GetEntityManager() const
    {
        return m_entityManager;
    }

    MessageHandler* SystemManager::GetMessageHandler()
    {
        return &m_messages;
    }

    void SystemManager::AddEvent(const EntityId& t_entity, const EventID& t_event)
    {
        m_events[t_entity].AddEvent(t_event);
    }

    void SystemManager::Update(const float t_dT)
    {
        for(auto &itr : m_systems)
        {
            itr.second->Update(t_dT);
        }

        HandleEvents();
    }

    void SystemManager::HandleEvents()
    {
        for(auto &event : m_events)
        {
            EventID id = 0;
            while(event.second.ProcessEvents(id))
            {
                for(auto &system : m_systems)
                {
                    if(system.second->HasEntity(event.first))
                    {
                        system.second->HandleEvent(event.first, static_cast<EntityEvent>(id));
                    }
                }
            }
        }
    }

    void SystemManager::Draw(Window* t_wind, unsigned int t_elevation)
    {
        auto itr = m_systems.find(System::Renderer);
        if(itr != m_systems.end())
        {
            auto system = dynamic_cast<S_Renderer*>(itr->second.get());
            system->Render(t_wind, t_elevation);
        }

        itr = m_systems.find(System::EntityUI);
        if (itr != m_systems.end())
        {
            auto ui = dynamic_cast<S_EntityUI*>(itr->second.get());
            ui->Render(t_wind);
        }
    }

    void SystemManager::EntityModified(const EntityId& t_entity, const Bitmask& t_bits)
    {
        for(auto &s_itr : m_systems)
        {
            auto& system = s_itr.second;

            if(system->FitsRequirements(t_bits))
            {
                if(!system->HasEntity(t_entity))
                {
                    system->AddEntity(t_entity);
                }
            }
            else
            {
                if(system->HasEntity(t_entity))
                {
                    system->RemoveEntity(t_entity);
                }
            }
        }
    }

    void SystemManager::RemoveEntity(const EntityId& t_entity)
    {
        for(auto &system : m_systems)
        {
            system.second->RemoveEntity(t_entity);
        }
    }

    void SystemManager::PurgeEntities()
    {
        for(auto &system : m_systems)
        {
            system.second->Purge();
        }
    }

    void SystemManager::PurgeSystems()
    {
        m_systems.clear();
    }
}
