#pragma once
#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include "BaseComponent.h"
#include "Bitmask.h"
#include "TextureManager.h"
#include "MapLoadee.h"
#include "MapDefinitions.h"

namespace SExE
{
    using EntityId = unsigned int;

    using EntityIdList = std::vector<EntityId>;

    using ComponentType = std::unique_ptr<BaseComponent>;
    using ComponentContainer = std::vector<ComponentType>;
    using EntityData = std::pair<Bitmask, ComponentContainer>;
    using EntityContainer = std::unordered_map<EntityId, EntityData>;
    using EntityTypes = std::unordered_map<EntityId, std::string>;
    using ComponentFactory = std::unordered_map<Component, std::function<BaseComponent*(void)>>;

    class SystemManager;
    class EntityManager : public MapLoadee
    {
        public:
        EntityManager(SystemManager* t_sysMgr, TextureManager* t_textureMgr);
        ~EntityManager();

        int addEntity(const Bitmask& t_mask);
        int addEntity(const std::string& t_entityFile);
        bool removeEntity(const EntityId& t_id);

        int findEntityAtPoint(const sf::Vector2f& t_point, unsigned int t_elevationMin = 0, unsigned int t_elevationMax = Sheet::Num_Layers - 1) const;

        void readMapLine(const std::string& t_type, std::stringstream& t_stream);
        void saveMap(std::ofstream& t_fileStream);

        bool addComponent(const EntityId& t_entity, const Component& t_component);

        template<class T>
        void addComponentType(const Component& t_id)
        {
            m_cFactory[t_id] = []()->BaseComponent* { return new T(); };
        }

        template<class T>
        T* getComponent(const EntityId& t_entity, const Component& t_component)
        {
            auto itr = m_entities.find(t_entity);
            if(itr == m_entities.end())
            {
                return nullptr;
            }

            // Found the entity.
            if(!itr->second.first.GetBit((unsigned int)t_component))
            {
                return nullptr;
            }

            // Component exists.
            auto& container = itr->second.second;
            auto component = std::find_if(container.begin(), container.end(), [&t_component](ComponentType& c) { return c->GetType() == t_component; });
            return (component != container.end() ? dynamic_cast<T*>(&(*component->get())) : nullptr);
        }

        bool removeComponent(const EntityId& t_entity, const Component& t_component);
        bool hasComponent(const EntityId& t_entity, const Component& t_component) const;

        void purge();

        private:
        unsigned int m_idCounter;
        EntityContainer m_entities;
        EntityTypes m_entityTypes;
        ComponentFactory m_cFactory;

        SystemManager* m_systems;
        TextureManager* m_textureManager;
    };
}

#endif // !ENTITYMANAGER_H
