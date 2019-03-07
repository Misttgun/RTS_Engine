#pragma once
#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>
#include "BaseSystem.h"
#include "Window.h"
#include "EventQueue.h"
#include "MessageHandler.h"

namespace SExE
{
    using SystemContainer = std::unordered_map<System, std::unique_ptr<BaseSystem>>;
    using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;

    class EntityManager;

    class SystemManager
    {
        public:
        SystemManager();
        ~SystemManager();

        void setEntityManager(EntityManager* t_entityMgr);
        EntityManager* getEntityManager();
        MessageHandler* getMessageHandler();

        template<class T>
        void addSystem(const System& t_system)
        {
            m_systems[t_system] = std::make_unique<T>(this);
        }

        template<class T>
        T* getSystem(const System& t_system)
        {
            auto itr = m_systems.find(t_system);
            return(itr != m_systems.end() ? dynamic_cast<T*>(itr->second.get()) : nullptr);
        }

        void addEvent(const EntityId& t_entity, const EventID& t_event);

        void update(float t_dT);
        void handleEvents();
        void draw(Window* t_wind, unsigned int t_elevation);

        void entityModified(const EntityId& t_entity, const Bitmask& t_bits);
        void removeEntity(const EntityId& t_entity);

        void purgeEntities();
        void purgeSystems();

        private:
        SystemContainer m_systems;
        EntityManager* m_entityManager;
        EntityEventContainer m_events;
        MessageHandler m_messages;
    };
}

#endif // !SYSTEMMANAGER_H