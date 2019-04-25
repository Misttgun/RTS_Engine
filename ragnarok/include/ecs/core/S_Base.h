#pragma once

#include <vector>
#include "EntityManager.h"
#include "../ECSTypes.h"
#include "../EntityEvents.h"
#include "../../types/Bitmask.h"
#include "../../messages/Observer.h"

namespace ragnarok
{
    using EntityList = std::vector<EntityId>;
    using Requirements = std::vector<Bitmask>;

    class SystemManager;


    /**
     * \brief Base class for all the systems.
     */
    class S_Base : public Observer
    {
        public:
        S_Base(const System& t_id, SystemManager* t_systemMgr);
        virtual ~S_Base();

        bool AddEntity(const EntityId& t_entity);
        bool HasEntity(const EntityId& t_entity) const;
        bool RemoveEntity(const EntityId& t_entity);

        System GetId() const;

        bool FitsRequirements(const Bitmask& t_bits) const;


        /**
         * \brief Clears all the entities associated with this system
         */
        void Purge();

        virtual void Update(float t_dT) = 0;
        virtual void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) = 0;

        protected:
        System m_id;
        Requirements m_requiredComponents;
        EntityList m_entities;

        SystemManager* m_systemManager;
    };
}