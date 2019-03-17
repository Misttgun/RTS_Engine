#include "ragnarok/ecs/core/EntityManager.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/ecs/EntityEvents.h"
#include "ragnarok/ecs/components/C_SpriteSheet.h"
#include "ragnarok/ecs/components/C_Position.h"
#include "ragnarok/ecs/components/C_Collidable.h"

namespace ragnarok
{
    EntityManager::EntityManager(SystemManager* t_sysMgr, TextureManager* t_textureMgr) 
        : m_idCounter(0), m_systems(t_sysMgr), m_textureManager(t_textureMgr)
    {}

    EntityManager::~EntityManager()
    {
        Purge();
    }

    int EntityManager::AddEntity(const Bitmask& t_mask)
    {
        unsigned int entity = m_idCounter;

        if(!m_entities.emplace(entity, EntityData(0, ComponentContainer())).second)
        {
            return -1;
        }

        ++m_idCounter;

        for(unsigned int i = 0; i < static_cast<unsigned int>(Component::COUNT); ++i)
        {
            if(t_mask.GetBit(i))
            {
                AddComponent(entity, static_cast<Component>(i));
            }
        }

        // Notifying the system manager of a modified entity.
        m_systems->EntityModified(entity, t_mask);
        m_systems->AddEvent(entity, static_cast<EventID>(EntityEvent::Spawned));
        return entity;
    }

    int EntityManager::AddEntity(const std::string& t_entityFile)
    {
        int e_id = -1;

        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + "res/Entities/" + t_entityFile + ".entity");

        if(!file.is_open())
        {
            std::cout << "! Failed to load entity: " << t_entityFile << std::endl;
            return -1;
        }

        std::string line;
        std::string entityType;

        while(std::getline(file, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;

            if(type == "Name")
            {
                keystream >> entityType;
            }
            else if(type == "Attributes")
            {
                if(e_id != -1)
                {
                    continue;
                }

                Bitset set = 0;
                Bitmask mask;
                keystream >> set;
                mask.SetMask(set);
                e_id = AddEntity(mask);

                if(e_id == -1)
                {
                    return -1;
                }
            }
            else if(type == "Component")
            {
                if(e_id == -1)
                {
                    continue;
                }

                unsigned int c_id = 0;
                keystream >> c_id;
                const auto component = GetComponent<C_Base>(e_id, static_cast<Component>(c_id));

                if(!component)
                {
                    continue;
                }

                keystream >> *component;

                if(component->GetType() == Component::SpriteSheet)
                {
                    auto sheet = dynamic_cast<C_SpriteSheet*>(component);
                    sheet->Create(m_textureManager);
                }
            }
        }

        file.close();
        m_entityTypes.emplace(std::make_pair(static_cast<EntityId>(e_id), entityType));

        return e_id;
    }

    bool EntityManager::RemoveEntity(const EntityId& t_id)
    {
        const auto itr = m_entities.find(t_id);

        if(itr == m_entities.end())
        {
            return false;
        }

        m_entities.erase(itr);
        m_systems->RemoveEntity(t_id);

        return true;
    }

    int EntityManager::FindEntityAtPoint(const sf::Vector2f& t_point, unsigned int t_elevationMin, unsigned int t_elevationMax) const
    {
        for(auto& entity : m_entities)
        {
            auto& bitmask = entity.second.first;
            auto& components = entity.second.second;

            if(!bitmask.GetBit(static_cast<unsigned int>(Component::Position)))
            {
                continue;
            }

            const auto position = dynamic_cast<C_Position*>(std::find_if(components.begin(), components.end(),
                                                     [](const C_Type& t_component) { return t_component->GetType() == Component::Position; })->get());

            if(position->GetElevation() < t_elevationMin || position->GetElevation() > t_elevationMax)
            {
                continue;
            }

            if(bitmask.GetBit(static_cast<unsigned int>(Component::Collidable)))
            {
                const auto component = dynamic_cast<C_Collidable*>(std::find_if(components.begin(), components.end(),
                                                            [](const C_Type& t_component) { return t_component->GetType() == Component::Collidable; })->get());

                if(component->GetCollidable().contains(t_point))
                {
                    return entity.first;
                }
            }
            else if(bitmask.GetBit(static_cast<unsigned int>(Component::SpriteSheet)))
            {
                auto component = dynamic_cast<C_SpriteSheet*>(std::find_if(components.begin(), components.end(),
                                                             [](const C_Type& t_component) { return t_component->GetType() == Component::SpriteSheet; })->get());
                
                if(component->GetSpriteSheet()->GetSpriteBounds().contains(t_point))
                {
                    return entity.first;
                }
            }
            else
            {
                sf::FloatRect rect;
                rect.left = position->GetPosition().x - 16.f;
                rect.top = position->GetPosition().y - 16.f;
                rect.width = 32.f;
                rect.height = 32.f;
                
                if(rect.contains(t_point))
                {
                    return entity.first;
                }
            }
        }

        return -1;
    }

    void EntityManager::ReadMapLine(const std::string& t_type, std::stringstream& t_stream)
    {}

    void EntityManager::SaveMap(std::ofstream& t_fileStream)
    {
        for(auto& entity : m_entities)
        {
            auto type = m_entityTypes.find(entity.first);
            
            if(type == m_entityTypes.end())
            {
                continue;
            }

            Component comp = Component::Position;

            if(!entity.second.first.GetBit(static_cast<unsigned int>(comp)))
            {
                continue;
            }

            auto component = std::find_if(entity.second.second.begin(), entity.second.second.end(), [comp](C_Type& c) { return c->GetType() == comp; });
            
            if(component == entity.second.second.end())
            {
                continue;
            }

            const auto position = dynamic_cast<C_Position*>(component->get());
            t_fileStream << "ENTITY " << type->second << " " << position->GetPosition().x << " " << position->GetPosition().y << " " << position->GetElevation() << std::endl;
        }
    }

    bool EntityManager::AddComponent(const EntityId& t_entity, const Component& t_component)
    {
        auto itr = m_entities.find(t_entity);

        if(itr == m_entities.end())
        {
            return false;
        }

        if(itr->second.first.GetBit(static_cast<unsigned int>(t_component)))
        {
            return false;
        }

        // Component doesn't exist.
        const auto itr2 = m_cFactory.find(t_component);
        if(itr2 == m_cFactory.end())
        {
            return false;
        }

        // Component type does exist.
        itr->second.second.emplace_back(C_Type(itr2->second()));
        itr->second.first.TurnOnBit(static_cast<unsigned int>(t_component));

        // Notifying the system manager of a modified entity.
        m_systems->EntityModified(t_entity, itr->second.first);

        return true;
    }

    bool EntityManager::RemoveComponent(const EntityId& t_entity, const Component& t_component)
    {
        auto itr = m_entities.find(t_entity);

        if(itr == m_entities.end())
        {
            return false;
        }

        // Found the entity.
        if(!itr->second.first.GetBit(static_cast<unsigned int>(t_component)))
        {
            return false;
        }

        // Component exists.
        auto& container = itr->second.second;
        const auto component = std::find_if(container.begin(), container.end(), [&t_component](C_Type& c) { return c->GetType() == t_component; });
        
        if(component == container.end())
        {
            return false;
        }

        container.erase(component);
        itr->second.first.ClearBit(static_cast<unsigned int>(t_component));

        m_systems->EntityModified(t_entity, itr->second.first);

        return true;
    }

    bool EntityManager::HasComponent(const EntityId& t_entity, const Component& t_component) const
    {
        const auto itr = m_entities.find(t_entity);

        if(itr == m_entities.end())
        {
            return false;
        }

        return itr->second.first.GetBit(static_cast<unsigned int>(t_component));
    }

    void EntityManager::Purge()
    {
        m_systems->PurgeEntities();
        m_entities.clear();
        m_idCounter = 0;
    }
}