#include <ecs/components/C_Attack.h>
#include <ecs/components/C_SpriteSheet.h>
#include <ecs/components/C_Selection.h>
#include "states/StateManager.h"
#include "window/Window.h"
#include "gui/GUIManager.h"
#include "gui/GUIStyle.h"
#include "ecs/core/SystemManager.h"
#include "ecs/systems/S_Collision.h"
#include "ecs/systems/S_Movement.h"
#include "states/StateLoading.h"
#include "sounds/SoundManager.h"
#include "utilities/RandomGenerator.h"
#include "map/Map.h"
#include "../../include/states/StateGame.h"

StateGame::StateGame(ragnarok::StateManager* t_stateManager) : BaseState(t_stateManager), m_destination(sf::Vector2f(-1, -1)), m_RessourceHandler(t_stateManager->GetContext()->m_systemManager)
{

}

StateGame::~StateGame() = default;

void StateGame::OnCreate()
{
    const auto context = m_stateMgr->GetContext();

    ragnarok::GUIManager* gui = context->m_guiManager;
    gui->LoadInterface("RessourceMenu.interface", "RessourceMenu");
    gui->GetInterface("RessourceMenu")->SetPosition(sf::Vector2f(0.f, 0.f));
    m_population = 0;
    m_max_population = 20;
    gui->LoadInterface("UnitMenu.interface", "UnitMenu");
    //gui->LoadInterface("SelectionSprite.interface", "SelectionSprite");

    ragnarok::EventManager* evMgr = context->m_eventManager;

    evMgr->AddCallback("Key_Escape", &StateGame::MainMenu, this);
    evMgr->AddCallback("Mouse_Right", &StateGame::RightClickAction, this);
    evMgr->AddCallback("Mouse_Left", &StateGame::LeftClickAction, this);

    const sf::Vector2u size = context->m_wind->GetWindowSize();
    m_view.setSize(static_cast<float>(size.x), static_cast<float>(size.y));
    m_view.setCenter(static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2);
    m_view.zoom(0.9f);
    gui->GetInterface("UnitMenu")->SetPosition(sf::Vector2f(0.f, static_cast<float>(size.y) - 50.f));

    auto loading = m_stateMgr->GetState<ragnarok::StateLoading>(ragnarok::StateType::Loading);
    context->m_gameMap->AddFile(ragnarok::Utils::GetWorkingDirectory() + "res/Maps/demo.map");
    //context->m_gameMap->AddFile(ragnarok::Utils::GetWorkingDirectory() + "res/Maps/forest.map");
    loading->AddLoader(context->m_gameMap);
    loading->SetManualContinue(true);
    //context->m_soundManager->PlayMusic("TownTheme", 50.f, true);
}

void StateGame::OnDestroy()
{
    auto context = m_stateMgr->GetContext();
    m_stateMgr->GetContext()->m_guiManager->RemoveInterface(ragnarok::StateType::Game, "GameMenu");
    ragnarok::EventManager* evMgr = context->m_eventManager;
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Key_Escape");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Mouse_Left");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Mouse_Right");

    context->m_gameMap->PurgeMap();
    context->m_gameMap->GetTileSet()->Purge();
}

void StateGame::Update(const sf::Time & t_time)
{
    auto context = m_stateMgr->GetContext();
    const auto pos = m_stateMgr->GetContext()->m_entityManager->GetComponent<ragnarok::C_Position>(m_player, ragnarok::Component::Position);
    if (pos)
    {
        const auto spriteSheet = context->m_entityManager->GetComponent<ragnarok::C_SpriteSheet>(m_player, ragnarok::Component::SpriteSheet);
        if (spriteSheet == nullptr || spriteSheet->GetSpriteSheet()->GetCurrentAnim()->GetName() != "Attack")
        {
            if (static_cast<sf::Vector2i>(pos->GetPosition()) != static_cast<sf::Vector2i>(m_destination) && m_destination != sf::Vector2f(-1, -1))
            {
                const auto diff = m_destination - pos->GetPosition();
                MovementLogic(diff);
            }
        }

        UpdateRessources();
        UpdateCamera();
    }

    context->m_gameMap->Update(t_time.asSeconds());
    context->m_systemManager->Update(t_time.asSeconds());
}

void StateGame::UpdateCamera()
{
    if (m_player == -1)
    {
        return;
    }

    ragnarok::SharedContext* context = m_stateMgr->GetContext();
    const auto pos = m_stateMgr->GetContext()->m_entityManager->GetComponent<ragnarok::C_Position>(m_player, ragnarok::Component::Position);

    m_view.setCenter(pos->GetPosition());
    context->m_wind->GetRenderWindow()->setView(m_view);

    const sf::FloatRect viewSpace = context->m_wind->GetViewSpace();

    if (viewSpace.left <= 0)
    {
        m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
    else if (viewSpace.left + viewSpace.width > (context->m_gameMap->GetMapSize().x) * ragnarok::Sheet::Tile_Size)
    {
        m_view.setCenter(((context->m_gameMap->GetMapSize().x) * ragnarok::Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }

    if (viewSpace.top <= 0)
    {
        m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
    else if (viewSpace.top + viewSpace.height > (context->m_gameMap->GetMapSize().y) * ragnarok::Sheet::Tile_Size)
    {
        m_view.setCenter(m_view.getCenter().x, ((context->m_gameMap->GetMapSize().y) * ragnarok::Sheet::Tile_Size) - (viewSpace.height / 2));
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
}

void StateGame::MovementLogic(const sf::Vector2f & t_delta) const
{
    ragnarok::Message msgx(static_cast<ragnarok::MessageType>(ragnarok::EntityMessage::Move));
    msgx.m_receiver = m_player;

    if (static_cast<int>(t_delta.x) > 0)
    {
        msgx.m_int = static_cast<int>(ragnarok::Direction::Right);
    }
    else if (static_cast<int>(t_delta.x) < 0)
    {
        msgx.m_int = static_cast<int>(ragnarok::Direction::Left);
    }

    ragnarok::Message msgy(static_cast<ragnarok::MessageType>(ragnarok::EntityMessage::Move));
    msgy.m_receiver = m_player;

    if (static_cast<int>(t_delta.y) > 0)
    {
        msgy.m_int = static_cast<int>(ragnarok::Direction::Down);
    }
    else if (static_cast<int>(t_delta.y) < 0)
    {
        msgy.m_int = static_cast<int>(ragnarok::Direction::Up);
    }

    m_stateMgr->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msgx);
    m_stateMgr->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msgy);
}

void StateGame::Draw()
{
    auto context = m_stateMgr->GetContext();
    for (unsigned int i = 0; i < ragnarok::Sheet::Num_Layers; ++i)
    {
        context->m_gameMap->Draw(i);
        m_stateMgr->GetContext()->m_systemManager->Draw(m_stateMgr->GetContext()->m_wind, i);
    }
}

void StateGame::MainMenu(ragnarok::EventDetails * t_details)
{
    m_stateMgr->SwitchTo(ragnarok::StateType::MainMenu);
}

void StateGame::Activate()
{
    auto map = m_stateMgr->GetContext()->m_gameMap;

    m_player = -1;
    map->Redraw();
}

void StateGame::Deactivate()
{}

void StateGame::RightClickAction(ragnarok::EventDetails * t_details)
{
    if (t_details->m_name != "Mouse_Right")
    {
        return;
    }

    const auto context = m_stateMgr->GetContext();
    ragnarok::GUIManager* gui = context->m_guiManager;
    if (!(gui->GetInterface("RessourceMenu")->IsFocused())
        && !(gui->GetInterface("UnitMenu")->IsFocused()))
    {
        ragnarok::Window* window = context->m_wind;
        const sf::Vector2i mousePos = context->m_eventManager->GetMousePos(window->GetRenderWindow());
        const sf::Vector2f clickedPosition = window->GetRenderWindow()->mapPixelToCoords(mousePos);
        int clickedEntity = context->m_entityManager->FindEntityAtPoint(clickedPosition);
        if (clickedEntity != -1 && clickedEntity != m_player)
        {
            const auto attack = context->m_entityManager->GetComponent<ragnarok::C_Attack>(m_player,
                                                                                           ragnarok::Component::Attack);
            const auto spriteSheet =
                context->m_entityManager->GetComponent<ragnarok::C_SpriteSheet>(m_player,
                                                                                ragnarok::Component::SpriteSheet);
            if (attack == nullptr || spriteSheet->GetSpriteSheet()->GetCurrentAnim()->GetName() == "Attack")
            {
                return;
            }
            attack->SetTargetEntity(clickedEntity);
        }
        else
        {
            m_destination = clickedPosition;
        }
    }
}

void StateGame::LeftClickAction(ragnarok::EventDetails * t_details)
{
    if (t_details->m_name == "Mouse_Left")
    {
        const auto context = m_stateMgr->GetContext();
        auto& rand = *(context->m_rand);				//TEMP
        ragnarok::GUIManager* gui = context->m_guiManager;
        if (gui->GetInterface("UnitMenu")->IsFocused())
        {
            if (gui->GetInterface("UnitMenu")->GetElement("UnitPeasant")->GetState() == ragnarok::GUIElementState::Clicked)
            {
                if (m_population < m_max_population)
                {
                    std::cout << "on spawn un peasant" << std::endl;
                    const int entityId = context->m_entityManager->AddEntity("Peasant");
                    const auto pos = context->m_entityManager->GetComponent<ragnarok::C_Position>(entityId, ragnarok::Component::Position);
                    const auto rposx = rand(0.0f, 900.0f);				//TEMP
                    const auto rposy = rand(0.0f, 900.0f);				//TEMP
                    pos->SetPosition(rposx, rposy);
                    ++m_population;
                }
                else
                {
                    std::cout << "Population maximale atteinte !" << std::endl;
                }
            }
            else if (gui->GetInterface("UnitMenu")->GetElement("UnitSoldier")->GetState() == ragnarok::GUIElementState::Clicked)
            {
                if (m_population < m_max_population)
                {
                    std::cout << "on spawn un soldier" << std::endl;
                    const int entityId = context->m_entityManager->AddEntity("Soldier");
                    const auto pos = context->m_entityManager->GetComponent<ragnarok::C_Position>(entityId, ragnarok::Component::Position);
                    const auto rposx = rand(0.0f, 900.0f);				//TEMP
                    const auto rposy = rand(0.0f, 900.0f);				//TEMP
                    pos->SetPosition(rposx, rposy);
                    ++m_population;
                }
                else
                {
                    std::cout << "Population maximale atteinte !" << std::endl;
                }
            }
            else if (gui->GetInterface("UnitMenu")->GetElement("UnitArcher")->GetState() == ragnarok::GUIElementState::Clicked)
            {
                if (m_population < m_max_population)
                {
                    std::cout << "on spawn un archer" << std::endl;
                    const int entityId = context->m_entityManager->AddEntity("Archer");
                    const auto pos = context->m_entityManager->GetComponent<ragnarok::C_Position>(entityId, ragnarok::Component::Position);
                    const auto rposx = rand(0.0f, 900.0f);				//TEMP
                    const auto rposy = rand(0.0f, 900.0f);				//TEMP
                    pos->SetPosition(rposx, rposy);
                    ++m_population;
                }
                else
                {
                    std::cout << "Population maximale atteinte !" << std::endl;
                }
            }
        }
        else if (!gui->GetInterface("RessourceMenu")->IsFocused())
        {
            ragnarok::Window* window = context->m_wind;
            const sf::Vector2i mousePos = context->m_eventManager->GetMousePos(window->GetRenderWindow());
            const sf::Vector2f clickedPosition = window->GetRenderWindow()->mapPixelToCoords(mousePos);
            const int clickedEntity = context->m_entityManager->FindEntityAtPoint(clickedPosition);
            if (clickedEntity != -1)
            {
                const auto pos = context->m_entityManager->GetComponent<ragnarok::C_Position>(clickedEntity, ragnarok::Component::Position);
                const auto selection = context->m_entityManager->GetComponent<ragnarok::C_Selection>(clickedEntity, ragnarok::Component::Selection);
                if (!selection)
                    return;

                const auto currentEntity = m_player;
                m_player = clickedEntity;
                const auto oldSelection = context->m_entityManager->GetComponent<ragnarok::C_Selection>(currentEntity, ragnarok::Component::Selection);
                if (oldSelection)
                    oldSelection->SetSelection(false);

                if (selection)
                    selection->SetSelection(true);

                m_destination = pos->GetPosition();
                const auto diff = m_destination - pos->GetPosition();
                MovementLogic(sf::Vector2f(0, 0));
            }
        }
    }
}

void StateGame::UpdateRessources()
{
    const auto context = m_stateMgr->GetContext();
    ragnarok::GUIManager* gui = context->m_guiManager;
    const int m_gold = m_RessourceHandler.GetGold();
    gui->GetInterface("RessourceMenu")->GetElement("RessourceBar")->SetText("Gold : " + std::to_string(m_gold) + "\nPopulation : " + std::to_string(m_population) + "/10");
}