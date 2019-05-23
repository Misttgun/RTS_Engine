#include "../../../include/ecs/systems/S_EntityUI.h"
#include "../../../include/ecs/core/SystemManager.h"
#include "../../../include/ecs/components/C_Position.h"
#include "../../../include/ecs/components/C_Health.h"
#include "../../../include/ecs/components/C_Selection.h"

namespace ragnarok
{
    S_EntityUI::S_EntityUI(SystemManager* t_systemMgr) : S_Base(System::EntityUI, t_systemMgr)
    {
        Bitmask req;
        req.TurnOnBit(static_cast<unsigned int>(Component::Position));
        req.TurnOnBit(static_cast<unsigned int>(Component::Health));
        req.TurnOnBit(static_cast<unsigned int>(Component::Selection));
        m_requiredComponents.push_back(req);
        req.Clear();

        // Set up of the health bar
        m_healthBarSize = sf::Vector2f(40.0f, 3.0f);
        m_healthBar = sf::RectangleShape(m_healthBarSize);
        m_healthBar.setFillColor(sf::Color::Green);

        // Set up of the selection rectangle
        EntityManager* entities = m_systemManager->GetEntityManager();
        m_selectionRect = sf::RectangleShape(sf::Vector2f(32.0f, 32.0f));
        m_selectionRect.setFillColor(sf::Color::Transparent);
        m_selectionRect.setOrigin(16.0f, 16.0f);
        m_selectionRect.setOutlineThickness(1.f);
        m_selectionRect.setOutlineColor(sf::Color::Green);
    }

    void S_EntityUI::Update(float t_dT)
    {}
    void S_EntityUI::HandleEvent(const EntityId& t_entity, const EntityEvent& t_event)
    {}
    void S_EntityUI::Notify(const Message& t_message)
    {}

    void S_EntityUI::Render(Window* t_wind)
    {
        EntityManager* entities = m_systemManager->GetEntityManager();
        for(auto &entity : m_entities)
        {
            C_Health* health = entities->GetComponent<C_Health>(entity, Component::Health);
            C_Position* pos = entities->GetComponent<C_Position>(entity, Component::Position);
            C_Selection* selection = entities->GetComponent<C_Selection>(entity, Component::Selection);

            // If the components are not null and we can show the health on screen
            if(health && selection && pos)
            {
                if(health->ShowHealth())
                {
                    const auto ratio = static_cast<float>(health->GetHealth()) / static_cast<float>(health->GetMaxHealth());
                    auto newsize = sf::Vector2f(m_healthBarSize.x * ratio, m_healthBarSize.y);
                    m_healthBar.setSize(newsize);
                    m_healthBar.setPosition(pos->GetPosition() - health->GetOffset());

                    if(ratio > 0.65f)
                    {
                        m_healthBar.setFillColor(sf::Color::Green);
                    }
                    else if(ratio < 0.65f && ratio > 0.25f)
                    {
                        m_healthBar.setFillColor(sf::Color::Yellow);
                    }
                    else if(ratio < 0.25f)
                    {
                        m_healthBar.setFillColor(sf::Color::Red);
                    }

                    t_wind->GetRenderer()->Draw(m_healthBar);
                }

                if(selection->ShowSelection())
                {
                    const auto size = selection->GetSize();
                    m_selectionRect.setSize(sf::Vector2f(size, size));
                    m_selectionRect.setPosition(pos->GetPosition());
                    t_wind->GetRenderer()->Draw(m_selectionRect);
                }
            }
        }
    }
}