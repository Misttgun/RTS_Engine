#include "SystemManager.h"
#include "EntityManager.h"
#include "ecs/systems/RendererSystem.h"

namespace SExE
{
    SystemManager::SystemManager() : m_entityManager(nullptr)
    {}

    SystemManager::~SystemManager()
    {
        purgeSystems();
    }

    void SystemManager::setEntityManager(EntityManager* t_entityMgr)
    {
        if(!m_entityManager)
        {
            m_entityManager = t_entityMgr;
        }
    }

    EntityManager* SystemManager::getEntityManager()
    {
        return m_entityManager;
    }

    MessageHandler* SystemManager::getMessageHandler()
    {
        return &m_messages;
    }

    void SystemManager::addEvent(const EntityId& t_entity, const EventID& t_event)
    {
        m_events[t_entity].addEvent(t_event);
    }

    void SystemManager::update(float t_dT)
    {
        for(auto &itr : m_systems)
        {
            itr.second->update(t_dT);
        }

        handleEvents();
    }

    void SystemManager::handleEvents()
    {
        for(auto &event : m_events)
        {
            EventID id = 0;
            while(event.second.processEvents(id))
            {
                for(auto &system : m_systems)
                {
                    if(system.second->hasEntity(event.first))
                    {
                        system.second->handleEvent(event.first, (EntityEvent)id);
                    }
                }
            }
        }
    }

    void SystemManager::draw(Window* t_wind, unsigned int t_elevation)
    {
        auto itr = m_systems.find(System::Renderer);

        if(itr == m_systems.end())
        {
            return;
        }

        auto system = dynamic_cast<RendererSysytem*>(itr->second.get());
        system->render(t_wind, t_elevation);
    }

    void SystemManager::entityModified(const EntityId& t_entity, const Bitmask& t_bits)
    {
        for(auto &s_itr : m_systems)
        {
            auto& system = s_itr.second;
            if(system->fitsRequirements(t_bits))
            {
                if(!system->hasEntity(t_entity))
                {
                    system->addEntity(t_entity);
                }
            }
            else
            {
                if(system->hasEntity(t_entity))
                {
                    system->removeEntity(t_entity);
                }
            }
        }
    }

    void SystemManager::removeEntity(const EntityId& t_entity)
    {
        for(auto &system : m_systems)
        {
            system.second->removeEntity(t_entity);
        }
    }

    void SystemManager::purgeEntities()
    {
        for(auto &system : m_systems)
        {
            system.second->purge();
        }
    }

    void SystemManager::purgeSystems()
    {
        m_systems.clear();
    }
}
