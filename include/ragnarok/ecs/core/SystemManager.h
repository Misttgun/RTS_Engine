#pragma once

#include <unordered_map>
#include <memory>
#include "S_Base.h"
#include "../../window/Window.h"
#include "../../types/EventQueue.h"
#include "../../messages/MessageHandler.h"

namespace ragnarok
{
    // Links system identifiers to actual systems
    using SystemContainer = std::unordered_map<System, std::unique_ptr<S_Base>>;

    // Links an entity to its events
    using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;

    class EntityManager;

    /**
     * \brief Manage all the systems in the game
     */
    class SystemManager
    {
        public:
        SystemManager();
        ~SystemManager();

        void SetEntityManager(EntityManager* t_entityMgr);
        EntityManager* GetEntityManager() const;
        MessageHandler* GetMessageHandler();

        template<class T>
        void AddSystem(const System& t_system)
        {
            m_systems[t_system] = std::make_unique<T>(this);
        }

        template<class T>
        T* GetSystem(const System& t_system)
        {
            const auto itr = m_systems.find(t_system);
            return(itr != m_systems.end() ? dynamic_cast<T*>(itr->second.get()) : nullptr);
        }

        void AddEvent(const EntityId& t_entity, const EventID& t_event);

        void Update(float t_dT);
        void HandleEvents();
        void Draw(Window* t_wind, unsigned int t_elevation);

        void EntityModified(const EntityId& t_entity, const Bitmask& t_bits);
        void RemoveEntity(const EntityId& t_entity);

        void PurgeEntities();
        void PurgeSystems();

        private:
        SystemContainer m_systems;
        EntityManager* m_entityManager;
        EntityEventContainer m_events;
        MessageHandler m_messages;
    };
}