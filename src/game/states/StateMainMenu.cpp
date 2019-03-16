#include "game/states/StateMainMenu.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/states/StateLoading.h"

StateMainMenu::StateMainMenu(ragnarok::StateManager* t_stateManager) : BaseState(t_stateManager)
{}

StateMainMenu::~StateMainMenu() = default;

void StateMainMenu::OnCreate()
{
    const auto context = m_stateMgr->GetContext();

    ragnarok::GUIManager* gui = context->m_guiManager;
    gui->LoadInterface("MainMenu.interface", "MainMenu");
    gui->GetInterface("MainMenu")->SetPosition(sf::Vector2f(250.f, 168.f));

    ragnarok::EventManager* eMgr = context->m_eventManager;
    eMgr->AddCallback("MainMenu_Play", &StateMainMenu::Play, this);
    eMgr->AddCallback("MainMenu_Quit", &StateMainMenu::Quit, this);
}

void StateMainMenu::OnDestroy()
{
    m_stateMgr->GetContext()->m_guiManager->RemoveInterface(ragnarok::StateType::Game, "MainMenu");
    ragnarok::EventManager* eMgr = m_stateMgr->GetContext()->m_eventManager;
    eMgr->RemoveCallback(ragnarok::StateType::MainMenu, "MainMenu_Play");
    eMgr->RemoveCallback(ragnarok::StateType::MainMenu, "MainMenu_Quit");
}

void StateMainMenu::Activate()
{
    auto& play = *m_stateMgr->GetContext()->m_guiManager->GetInterface("MainMenu")->GetElement("Play");

    if(m_stateMgr->HasState(ragnarok::StateType::Game))
    {
        // Resume
        play.SetText("Resume");
    }
    else
    {
        // Play
        play.SetText("Play");
    }
}

void StateMainMenu::Play(ragnarok::EventDetails* t_details)
{
    m_stateMgr->SwitchTo(ragnarok::StateType::Game);
}

void StateMainMenu::Quit(ragnarok::EventDetails* t_details)
{
    m_stateMgr->GetContext()->m_wind->Close();
}

void StateMainMenu::Draw()
{}

void StateMainMenu::Update(const sf::Time& t_time)
{}

void StateMainMenu::Deactivate()
{}
