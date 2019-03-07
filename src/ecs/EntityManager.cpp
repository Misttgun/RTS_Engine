#include "EntityManager.h"
#include "SystemManager.h"
#include "EntityEvents.h"
#include "ecs/components/SpriteSheetComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CollidableComponent.h"
#include "Utilities.h"

namespace SExE
{
    EntityManager::EntityManager(SystemManager* t_sysMgr, TextureManager* t_textureMgr) : m_idCounter(0), m_systems(t_sysMgr), m_textureManager(t_textureMgr)
    {}

    EntityManager::~EntityManager()
    {
        purge();
    }

    int EntityManager::addEntity(const Bitmask& t_mask)
    {
        unsigned int entity = m_idCounter;
        if(!m_entities.emplace(entity, EntityData(0, ComponentContainer())).second)
        {
            return -1;
        }

        ++m_idCounter;

        for(unsigned int i = 0; i < (unsigned int)Component::COUNT; ++i)
        {
            if(t_mask.getBit(i))
            {
                addComponent(entity, (Component)i);
            }
        }

        // Notifying the system manager of a modified entity.
        m_systems->entityModified(entity, t_mask);
        m_systems->addEvent(entity, (EventID)EntityEvent::Spawned);
        return entity;
    }

    int EntityManager::addEntity(const std::string& t_entityFile)
    {
        int e_id = -1;

        std::ifstream file;
        file.open(Utils::getWorkingDirectory() + "res/entities/" + t_entityFile + ".entity");
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
                mask.setMask(set);
                e_id = addEntity(mask);

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
                auto component = getComponent<BaseComponent>(e_id, (Component)c_id);

                if(!component)
                {
                    continue;
                }

                keystream >> *component;

                if(component->getType() == Component::SpriteSheet)
                {
                    auto sheet = dynamic_cast<SpriteSheetComponent*>(component);
                    sheet->Create(m_textureManager);
                }
            }
        }

        file.close();
        m_entityTypes.emplace(std::make_pair(static_cast<EntityId>(e_id), entityType));
        return e_id;
    }

    bool EntityManager::removeEntity(const EntityId& t_id)
    {
        auto itr = m_entities.find(t_id);
        if(itr == m_entities.end())
        {
            return false;
        }

        m_entities.erase(itr);
        m_systems->removeEntity(t_id);

        return true;
    }

    int EntityManager::findEntityAtPoint(const sf::Vector2f& t_point, unsigned int t_elevationMin, unsigned int t_elevationMax) const
    {
        for(auto& entity : m_entities)
        {
            auto& bitmask = entity.second.first;
            auto& components = entity.second.second;
            if(!bitmask.getBit(static_cast<unsigned int>(Component::Position)))
            {
                continue;
            }

            auto position = static_cast<TransformComponent*>(std::find_if(components.begin(), components.end(),
                                                     [](const ComponentType& t_component) { return t_component->getType() == Component::Position; })->get());

            if(position->getElevation() < t_elevationMin || position->getElevation() > t_elevationMax)
            {
                continue;
            }

            if(bitmask.getBit(static_cast<unsigned int>(Component::Collidable)))
            {
                auto component = static_cast<CollidableComponent*>(std::find_if(components.begin(), components.end(),
                                                            [](const ComponentType& t_component) { return t_component->getType() == Component::Collidable; })->get());

                if(component->GetCollidable().contains(t_point))
                {
                    return entity.first;
                }
            }
            else if(bitmask.getBit(static_cast<unsigned int>(Component::SpriteSheet)))
            {
                auto component = static_cast<SpriteSheetComponent*>(std::find_if(components.begin(), components.end(),
                                                             [](const ComponentType& t_component) { return t_component->getType() == Component::SpriteSheet; })->get());

                if(component->GetSpriteSheet()->GetSpriteBounds().contains(t_point))
                {
                    return entity.first;
                }
            }
            else
            {
                sf::FloatRect rect;
                rect.left = position->getPosition().x - 16.f;
                rect.top = position->getPosition().y - 16.f;
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

    void EntityManager::readMapLine(const std::string& t_type, std::stringstream& t_stream)
    {}
    void EntityManager::saveMap(std::ofstream& t_fileStream)
    {
        for(auto& entity : m_entities)
        {
            auto type = m_entityTypes.find(entity.first);
            if(type == m_entityTypes.end())
            {
                continue;
            }

            Component comp = Component::Position;
            if(!entity.second.first.getBit(static_cast<unsigned int>(comp)))
            {
                continue;
            }

            auto component = std::find_if(entity.second.second.begin(), entity.second.second.end(), [comp](ComponentType& c) { return c->getType() == comp; });
            if(component == entity.second.second.end())
            {
                continue;
            }

            auto position = static_cast<TransformComponent*>(component->get());
            t_fileStream << "ENTITY " << type->second << " " << position->getPosition().x << " "
                << position->getPosition().y << " " << position->getElevation() << std::endl;
        }
    }

    bool EntityManager::addComponent(const EntityId& t_entity, const Component& t_component)
    {
        auto itr = m_entities.find(t_entity);
        if(itr == m_entities.end())
        {
            return false;
        }

        if(itr->second.first.getBit((unsigned int)t_component))
        {
            return false;
        }

        // Component doesn't exist.
        auto itr2 = m_cFactory.find(t_component);
        if(itr2 == m_cFactory.end())
        {
            return false;
        }

        // Component type does exist.
        itr->second.second.emplace_back(ComponentType(itr2->second()));
        itr->second.first.turnOnBit((unsigned int)t_component);

        // Notifying the system manager of a modified entity.
        m_systems->entityModified(t_entity, itr->second.first);

        return true;
    }

    bool EntityManager::removeComponent(const EntityId& t_entity, const Component& t_component)
    {
        auto itr = m_entities.find(t_entity);
        if(itr == m_entities.end())
        {
            return false;
        }

        // Found the entity.
        if(!itr->second.first.getBit((unsigned int)t_component))
        {
            return false;
        }

        // Component exists.
        auto& container = itr->second.second;
        auto component = std::find_if(container.begin(), container.end(), [&t_component](ComponentType& c) { return c->getType() == t_component; });

        if(component == container.end())
        {
            return false;
        }

        container.erase(component);
        itr->second.first.clearBit((unsigned int)t_component);

        m_systems->entityModified(t_entity, itr->second.first);

        return true;
    }

    bool EntityManager::hasComponent(const EntityId& t_entity, const Component& t_component) const
    {
        auto itr = m_entities.find(t_entity);
        if(itr == m_entities.end())
        {
            return false;
        }

        return itr->second.first.getBit((unsigned int)t_component);
    }

    void EntityManager::purge()
    {
        m_systems->purgeEntities();
        m_entities.clear();
        m_idCounter = 0;
    }
}
