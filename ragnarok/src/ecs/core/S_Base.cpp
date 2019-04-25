#include "../../../include/ecs/core/S_Base.h"
#include "../../../include/ecs/core/SystemManager.h"

namespace ragnarok
{
    S_Base::S_Base(const System& t_id, SystemManager* t_systemMgr) : m_id(t_id), m_systemManager(t_systemMgr)
    {}

    S_Base::~S_Base()
    {
        Purge();
    }

    bool S_Base::AddEntity(const EntityId& t_entity)
    {
        if(HasEntity(t_entity))
        {
            return false;
        }

        m_entities.emplace_back(t_entity);

        return true;
    }

    bool S_Base::HasEntity(const EntityId& t_entity) const
    {
        return std::find(m_entities.begin(), m_entities.end(), t_entity) != m_entities.end();
    }

    bool S_Base::RemoveEntity(const EntityId& t_entity)
    {
        const auto entity = std::find_if(m_entities.begin(), m_entities.end(), [&t_entity](EntityId& id) { return id == t_entity; });
        if(entity == m_entities.end())
        {
            return false;
        }

        m_entities.erase(entity);

        return true;
    }

    System S_Base::GetId() const
    {
        return m_id;
    }

    bool S_Base::FitsRequirements(const Bitmask& t_bits) const
    {
        return std::find_if(m_requiredComponents.cbegin(), m_requiredComponents.cend(),
                            [&t_bits](const Bitmask& b) { return b.Matches(t_bits, b.GetMask()); }) != m_requiredComponents.cend();
    }

    void S_Base::Purge()
    {
        m_entities.clear();
    }
}
