#include "game/states/StateMainMenu.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/states/StateLoading.h"
#include "ragnarok/particles/Emitter.h"
#include "ragnarok/particles/ParticleSystem.h"
#include "ragnarok/ecs/systems/S_SheetAnimation.h"

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

    const auto w_size = context->m_wind->GetWindowSize();
    context->m_particles->CreateState(ragnarok::StateType::MainMenu);
    auto emitter = std::make_unique<ragnarok::Emitter>(sf::Vector3f(
        static_cast<float>(w_size.x) / 3.f,
        static_cast<float>(w_size.y) - 64.f,
        33.f));

    emitter->SetEmitRate(60);
    emitter->SetGenerators("MenuSmoke");
    context->m_particles->AddEmitter(std::move(emitter));
    const auto f = ragnarok::ForceApplicator(
        sf::Vector3f(static_cast<float>(w_size.x) / 2.f, static_cast<float>(w_size.y) / 2.f, 64.f),
        sf::Vector3f(500.f, 500.f, 500.f), 256.f);
    context->m_particles->AddForce(f);
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
{
    auto context = m_stateMgr->GetContext();
    for (unsigned int i = 0; i < ragnarok::Sheet::Num_Layers; ++i)
    {
        m_stateMgr->GetContext()->m_particles->Draw(m_stateMgr->GetContext()->m_wind, i);
    }

    m_stateMgr->GetContext()->m_particles->Draw(m_stateMgr->GetContext()->m_wind, -1);
}

void StateMainMenu::Update(const sf::Time& t_time)
{
    m_stateMgr->GetContext()->m_particles->Update(t_time.asSeconds());
}

void StateMainMenu::Deactivate()
{}
