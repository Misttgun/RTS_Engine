#include "BaseSystem.h"
#include "SystemManager.h"

namespace SExE
{
    BaseSystem::BaseSystem(const System& t_id, SystemManager* t_systemMgr) : m_id(t_id), m_systemManager(t_systemMgr)
    {}

    BaseSystem::~BaseSystem()
    {
        purge();
    }

    bool BaseSystem::addEntity(const EntityId& t_entity)
    {
        if(hasEntity(t_entity))
        {
            return false;
        }

        m_entities.emplace_back(t_entity);

        return true;
    }

    bool BaseSystem::hasEntity(const EntityId& t_entity) const
    {
        return std::find(m_entities.begin(), m_entities.end(), t_entity) != m_entities.end();
    }

    bool BaseSystem::removeEntity(const EntityId& t_entity)
    {
        auto entity = std::find_if(m_entities.begin(), m_entities.end(), [&t_entity](EntityId& id) { return id == t_entity; });
        
        if(entity == m_entities.end())
        {
            return false;
        }

        m_entities.erase(entity);

        return true;
    }

    System BaseSystem::getId() const
    {
        return m_id;
    }

    bool BaseSystem::fitsRequirements(const Bitmask& t_bits) const
    {
        return std::find_if(m_requiredComponents.cbegin(), m_requiredComponents.cend(),
                            [&t_bits](const Bitmask& b) { return b.matches(t_bits, b.getMask()); }) != m_requiredComponents.cend();
    }

    void BaseSystem::purge()
    {
        m_entities.clear();
    }
}
