#include "ragnarok/ecs/systems/S_Renderer.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/ecs/components/C_Position.h"
#include "ragnarok/ecs/components/C_SpriteSheet.h"

namespace ragnarok
{
    S_Renderer::S_Renderer(SystemManager* t_systemMgr) : S_Base(System::Renderer, t_systemMgr)
    {
        // Requirements an entity must meet to belong to this system
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::SpriteSheet));
        m_requiredComponents.push_back(req);
        req.Clear();

        // Used to define what kind of drawables, we want to handle. Can be used to add more component (drawable) types
        m_drawableTypes.TurnOnBit(static_cast<unsigned int>(Component::SpriteSheet));

        m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
    }

    void S_Renderer::Update(float t_dT)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        for (auto &entity : m_entities)
        {
            // Get the entity current position
            const auto position = entityMgr->GetComponent<C_Position>(entity, Component::Position);

            C_Drawable* drawable = GetDrawableFromType(entity);
            if (!drawable)
            {
                continue;
            }

            // All drawables need to update their position
            drawable->UpdatePosition(position->GetPosition());
        }
    }

    void S_Renderer::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {
        if (t_event == EntityEvent::Moving_Left || t_event == EntityEvent::Moving_Right ||
            t_event == EntityEvent::Moving_Up || t_event == EntityEvent::Moving_Down ||
            t_event == EntityEvent::Elevation_Change || t_event == EntityEvent::Spawned)
        {
            SortDrawables();
        }
    }

    void S_Renderer::Notify(const Message& t_message)
    {
        // We get notified if the entity changed direction
        if (HasEntity(t_message.m_receiver))
        {
            const auto m = static_cast<EntityMessage>(t_message.m_type);

            switch (m)
            {
                case EntityMessage::Direction_Changed:
                    SetSheetDirection(t_message.m_receiver, static_cast<Direction>(t_message.m_int));
                    break;

                default:;
            }
        }
    }

    void S_Renderer::Render(Window* t_wind, unsigned int t_layer)
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        t_wind->GetRenderer()->UseShader("default");

        for (auto &entity : m_entities)
        {
            const auto position = entityMgr->GetComponent<C_Position>(entity, Component::Position);
            if (position->GetElevation() < t_layer)
            {
                continue;
            }

            // We break here because entities are sorted by elevetation in SortDrawables()
            if (position->GetElevation() > t_layer)
            {
                break;
            }

            C_Drawable* drawable = GetDrawableFromType(entity);
            if (!drawable)
            {
                continue;
            }

            // If we are on the right layer (elevation) go ahead an draw to the window
            drawable->Draw(t_wind);
        }
    }

    void S_Renderer::SetSheetDirection(const EntityId& t_entity, const Direction& t_dir) const
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        if (!entityMgr->HasComponent(t_entity, Component::SpriteSheet))
        {
            return;
        }

        // Get the spritesheet component and set its direction
        const auto sheet = entityMgr->GetComponent<C_SpriteSheet>(t_entity, Component::SpriteSheet);
        sheet->GetSpriteSheet()->SetDirection(t_dir);
    }

    void S_Renderer::SortDrawables()
    {
        // Logic for sorting entities by elevation
        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        std::sort(m_entities.begin(), m_entities.end(), [entityMgr](unsigned int t_1, unsigned int t_2)
        {
            const auto pos1 = entityMgr->GetComponent<C_Position>(t_1, Component::Position);
            const auto pos2 = entityMgr->GetComponent<C_Position>(t_2, Component::Position);

            if (pos1->GetElevation() == pos2->GetElevation())
            {
                // On same elevation, sort by y axis
                return pos1->GetPosition().y < pos2->GetPosition().y;
            }

            // Sort by elevation
            return pos1->GetElevation() < pos2->GetElevation();
        });
    }

    C_Drawable* S_Renderer::GetDrawableFromType(const EntityId& t_entity) const
    {
        EntityManager* entityMgr = m_systemManager->GetEntityManager();
        for (size_t i = 0; i < static_cast<size_t>(Component::COUNT); ++i)
        {
            if (!m_drawableTypes.GetBit(i))
            {
                continue;
            }

            auto component = static_cast<Component>(i);

            if (!entityMgr->HasComponent(t_entity, component))
            {
                continue;
            }

            return entityMgr->GetComponent<C_Drawable>(t_entity, component);
        }

        return nullptr;
    }
}