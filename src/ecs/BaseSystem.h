#pragma once
#ifndef BASESYSTEM_H
#define BASESYSTEM_H

#include <vector>
#include "EntityManager.h"
#include "ECSTypes.h"
#include "EntityEvents.h"
#include "Bitmask.h"
#include "EventQueue.h"
#include "Observer.h"

namespace SExE
{
    using EntityList = std::vector<EntityId>;
    using Requirements = std::vector<Bitmask>;

    class SystemManager;

    class BaseSystem : public Observer
    {
        public:
        BaseSystem(const System& t_id, SystemManager* t_systemMgr);
        virtual ~BaseSystem();

        bool addEntity(const EntityId& t_entity);
        bool hasEntity(const EntityId& t_entity) const;
        bool removeEntity(const EntityId& t_entity);

        System getId() const;

        bool fitsRequirements(const Bitmask& t_bits) const;
        void purge();

        virtual void update(float t_dT) = 0;
        virtual void handleEvent(const EntityId& t_entity, const EntityEvent& t_event) = 0;

        protected:
        System m_id;
        Requirements m_requiredComponents;
        EntityList m_entities;

        SystemManager* m_systemManager;
    };
}

#endif // !BASESYSTEM_H
