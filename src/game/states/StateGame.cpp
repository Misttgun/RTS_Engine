#include "game/states/StateGame.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/ecs/core/SystemManager.h"
#include "ragnarok/ecs/systems/S_Collision.h"
#include "ragnarok/ecs/systems/S_Movement.h"
#include "ragnarok/states/StateLoading.h"
#include "ragnarok/sounds/SoundManager.h"

StateGame::StateGame(ragnarok::StateManager* t_stateManager) : BaseState(t_stateManager)
{}

StateGame::~StateGame() = default;

void StateGame::OnCreate()
{
    const auto context = m_stateMgr->GetContext();
    ragnarok::EventManager* evMgr = context->m_eventManager;

    evMgr->AddCallback("Key_Escape", &StateGame::MainMenu, this);
    evMgr->AddCallback("Player_MoveLeft", &StateGame::PlayerMove, this);
    evMgr->AddCallback("Player_MoveRight", &StateGame::PlayerMove, this);
    evMgr->AddCallback("Player_MoveUp", &StateGame::PlayerMove, this);
    evMgr->AddCallback("Player_MoveDown", &StateGame::PlayerMove, this);

    const sf::Vector2u size = context->m_wind->GetWindowSize();
    m_view.setSize(static_cast<float>(size.x), static_cast<float>(size.y));
    m_view.setCenter(static_cast<float>(size.x) / 2, static_cast<float>(size.y) / 2);
    m_view.zoom(0.6f);

    auto loading = m_stateMgr->GetState<ragnarok::StateLoading>(ragnarok::StateType::Loading);
    context->m_gameMap->AddFile(ragnarok::Utils::GetWorkingDirectory() + "res/Maps/map1.map");
    loading->AddLoader(context->m_gameMap);
    loading->SetManualContinue(true);
    context->m_soundManager->PlayMusic("TownTheme", 50.f, true);
}

void StateGame::OnDestroy()
{
    auto context = m_stateMgr->GetContext();

    ragnarok::EventManager* evMgr = context->m_eventManager;
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Key_Escape");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Key_O");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Player_MoveLeft");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Player_MoveRight");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Player_MoveUp");
    evMgr->RemoveCallback(ragnarok::StateType::Game, "Player_MoveDown");

    context->m_gameMap->PurgeMap();
    context->m_gameMap->GetTileSet()->Purge();
}

void StateGame::Update(const sf::Time& t_time)
{
    auto context = m_stateMgr->GetContext();
    UpdateCamera();
    context->m_gameMap->Update(t_time.asSeconds());
    context->m_systemManager->Update(t_time.asSeconds());
}

void StateGame::UpdateCamera()
{
    if(m_player == -1)
    {
        return;
    }

    ragnarok::SharedContext* context = m_stateMgr->GetContext();
    const auto pos = m_stateMgr->GetContext()->m_entityManager->GetComponent<ragnarok::C_Position>(m_player, ragnarok::Component::Position);

    m_view.setCenter(pos->GetPosition());
    context->m_wind->GetRenderWindow()->setView(m_view);

    const sf::FloatRect viewSpace = context->m_wind->GetViewSpace();

    if(viewSpace.left <= 0)
    {
        m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
    else if(viewSpace.left + viewSpace.width > (context->m_gameMap->GetMapSize().x) * ragnarok::Sheet::Tile_Size)
    {
        m_view.setCenter(((context->m_gameMap->GetMapSize().x) * ragnarok::Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }

    if(viewSpace.top <= 0)
    {
        m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
    else if(viewSpace.top + viewSpace.height > (context->m_gameMap->GetMapSize().y) * ragnarok::Sheet::Tile_Size)
    {
        m_view.setCenter(m_view.getCenter().x, ((context->m_gameMap->GetMapSize().y) * ragnarok::Sheet::Tile_Size) - (viewSpace.height / 2));
        context->m_wind->GetRenderWindow()->setView(m_view);
    }
}

void StateGame::Draw()
{
    auto context = m_stateMgr->GetContext();
    for(unsigned int i = 0; i < ragnarok::Sheet::Num_Layers; ++i)
    {
        context->m_gameMap->Draw(i);
        m_stateMgr->GetContext()->m_systemManager->Draw(m_stateMgr->GetContext()->m_wind, i);
    }
}

void StateGame::MainMenu(ragnarok::EventDetails* t_details)
{
    m_stateMgr->SwitchTo(ragnarok::StateType::MainMenu);
}

void StateGame::Activate()
{
    auto map = m_stateMgr->GetContext()->m_gameMap;

    m_player = map->GetPlayerId();
    map->Redraw();
}

void StateGame::Deactivate()
{}

void StateGame::PlayerMove(ragnarok::EventDetails* t_details)
{
    ragnarok::Message msg(static_cast<ragnarok::MessageType>(ragnarok::EntityMessage::Move));

    if(t_details->m_name == "Player_MoveLeft")
    {
        msg.m_int = static_cast<int>(ragnarok::Direction::Left);
    }
    else if(t_details->m_name == "Player_MoveRight")
    {
        msg.m_int = static_cast<int>(ragnarok::Direction::Right);
    }
    else if(t_details->m_name == "Player_MoveUp")
    {
        msg.m_int = static_cast<int>(ragnarok::Direction::Up);
    }
    else if(t_details->m_name == "Player_MoveDown")
    {
        msg.m_int = static_cast<int>(ragnarok::Direction::Down);
    }

    msg.m_receiver = m_player;
    m_stateMgr->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
}