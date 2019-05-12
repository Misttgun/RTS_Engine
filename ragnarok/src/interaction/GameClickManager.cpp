//
// Created by florian on 08/05/19.
//

#include "../../include/interaction/GameClickManager.h"
#include "../../include/window/Window.h"
#include "../../include/ecs/core/EntityManager.h"

ragnarok::GameClickManager::GameClickManager(ragnarok::StateManager *t_stateManager)
        : m_stateManager(t_stateManager)
{}

void ragnarok::GameClickManager::setupHooks(ragnarok::EventManager *t_eventManager)
{
    t_eventManager->AddCallback("Mouse_Left", &GameClickManager::handleEvent,
                                this);
    t_eventManager->AddCallback("Mouse_Right", &GameClickManager::handleEvent,
                                this);
}

void ragnarok::GameClickManager::handleEvent(ragnarok::EventDetails *t_event)
{
    const SharedContext *context = m_stateManager->GetContext();
    ragnarok::Window* window = context->m_wind;
    ragnarok::EventManager *eventManager = context->m_eventManager;
    sf::RenderWindow *renderWindow = window->GetRenderWindow();
    const sf::Vector2i mousePosition = eventManager->GetMousePos(renderWindow);
    sf::Vector2f position = renderWindow->mapPixelToCoords(mousePosition);
    int entityID = context->m_entityManager->FindEntityAtPoint(position);

    if (t_event->m_name == "Mouse_Left")
    {
        selectEntity(entityID);
    }
    else if (t_event->m_name == "Mouse_Right")
    {
        actOnEntity(entityID, position);
    }
}

void ragnarok::GameClickManager::selectEntity(int t_entityID)
{
    m_selectedEntity = t_entityID;
    // TODO send message / event to inform of the selection
}

/**
 * Calls appropriate registered callbacks
 * @param t_entityID the ID of the entity that was clicked
 * @param t_position the position that was clicked in the map
 */
void ragnarok::GameClickManager::actOnEntity(int t_entityID,
                                             sf::Vector2f t_position)
{
    for (const auto& callback : m_callbacks)
    {
        callback.second(t_entityID, t_position);
    }
}
