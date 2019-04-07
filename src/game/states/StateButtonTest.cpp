#include <iostream>
#include "game/states/StateButtonTest.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/states/StateLoading.h"
#include "ragnarok/resources/TextureManager.h"

StateButtonTest::StateButtonTest(ragnarok::StateManager* t_stateManager) : BaseState(t_stateManager)
{}

StateButtonTest::~StateButtonTest() = default;

void StateButtonTest::OnCreate()
{
    const auto context = m_stateMgr->GetContext();

    ragnarok::GUIManager* gui = context->m_guiManager;
    if (gui->LoadInterface("ButtonTest.interface", "ButtonTest"))
    {
        std::cout << "Successfully loaded ButtonTest interface." << std::endl;
    }
    else
    {
        std::cerr << "Couldn't load ButtonTest interface." << std::endl;
    }
    gui->GetInterface("ButtonTest")->SetPosition(sf::Vector2f(250.f, 168.f));

    ragnarok::EventManager* eMgr = context->m_eventManager;
    eMgr->AddCallback("ButtonTest_Play", &StateButtonTest::Play, this);
    eMgr->AddCallback("ButtonTest_Quit", &StateButtonTest::Quit, this);
}

void StateButtonTest::OnDestroy()
{
    m_stateMgr->GetContext()->m_guiManager->RemoveInterface(ragnarok::StateType::Game, "ButtonTest");
    ragnarok::EventManager* eMgr = m_stateMgr->GetContext()->m_eventManager;
    eMgr->RemoveCallback(ragnarok::StateType::ButtonTest, "ButtonTest_Play");
    eMgr->RemoveCallback(ragnarok::StateType::ButtonTest, "ButtonTest_Quit");
}

void StateButtonTest::Activate()
{
    auto& play = *m_stateMgr->GetContext()->m_guiManager->GetInterface("ButtonTest")->GetElement("Play");

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

void StateButtonTest::Play(ragnarok::EventDetails* t_details)
{
    //m_stateMgr->SwitchTo(ragnarok::StateType::Game);
}

void StateButtonTest::Quit(ragnarok::EventDetails* t_details)
{
    m_stateMgr->GetContext()->m_wind->Close();
}

void StateButtonTest::Draw()
{}

void StateButtonTest::Update(const sf::Time& t_time)
{}

void StateButtonTest::Deactivate()
{}
