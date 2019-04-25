#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include "C_Base.h"
#include "../../types/Bitmask.h"
#include "../../resources/TextureManager.h"
#include "../../map/MapLoadee.h"
#include "../../map/MapDefinitions.h"

namespace ragnarok
{
    using EntityId = unsigned int;

    using EntityIdList = std::vector<EntityId>;

    using C_Type = std::unique_ptr<C_Base>;
    using ComponentContainer = std::vector<C_Type>;
    using EntityData = std::pair<Bitmask, ComponentContainer>;
    using EntityContainer = std::unordered_map<EntityId, EntityData>;
    using EntityTypes = std::unordered_map<EntityId, std::string>;
    using ComponentFactory = std::unordered_map<Component, std::function<C_Base*(void)>>;

    class SystemManager;

    /**
     * \brief Manage all the entities in the game
     */
    class EntityManager : public MapLoadee
    {
        public:
        EntityManager(SystemManager* t_sysMgr, TextureManager* t_textureMgr);
        ~EntityManager();

        int AddEntity(const Bitmask& t_mask);
        int AddEntity(const std::string& t_entityFile);
        bool RemoveEntity(const EntityId& t_id);

        int FindEntityAtPoint(const sf::Vector2f& t_point, unsigned int t_elevationMin = 0, unsigned int t_elevationMax = Sheet::Num_Layers - 1) const;

        void ReadMapLine(const std::string& t_type, std::stringstream& t_stream) override;
        void SaveMap(std::ofstream& t_fileStream) override;

        bool AddComponent(const EntityId& t_entity, const Component& t_component);

        template<class T>
        void AddComponentType(const Component& t_id)
        {
            m_cFactory[t_id] = []()->C_Base* { return new T(); };
        }

        template<class T>
        T* GetComponent(const EntityId& t_entity, const Component& t_component)
        {
            auto itr = m_entities.find(t_entity);
            if(itr == m_entities.end())
            {
                return nullptr;
            }

            // Found the entity.
            if(!itr->second.first.GetBit(static_cast<unsigned int>(t_component)))
            {
                return nullptr;
            }

            // Component exists.
            auto& container = itr->second.second;
            auto component = std::find_if(container.begin(), container.end(), [&t_component](C_Type& c) { return c->GetType() == t_component; });
            
            return (component != container.end() ? dynamic_cast<T*>(&(*component->get())) : nullptr);
        }

        bool RemoveComponent(const EntityId& t_entity, const Component& t_component);
        bool HasComponent(const EntityId& t_entity, const Component& t_component) const;

        void Purge();

        private:
        unsigned int m_idCounter;
        EntityContainer m_entities;
        EntityTypes m_entityTypes;
        ComponentFactory m_cFactory;

        SystemManager* m_systems;
        TextureManager* m_textureManager;
    };
}