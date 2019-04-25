#include "game/Game.h"
#include "game/states/StateIntro.h"
#include "game/states/StateMainMenu.h"
#include "game/states/StateGame.h"
#include "ragnarok/ecs/components/C_Position.h"
#include "ragnarok/ecs/components/C_SpriteSheet.h"
#include "ragnarok/ecs/components/C_State.h"
#include "ragnarok/ecs/components/C_Movable.h"
#include "ragnarok/ecs/components/C_Controller.h"
#include "ragnarok/ecs/components/C_Collidable.h"
#include "ragnarok/ecs/components/C_SoundEmitter.h"
#include "ragnarok/ecs/components/C_SoundListener.h"
#include "ragnarok/ecs/systems/S_Renderer.h"
#include "ragnarok/ecs/systems/S_Movement.h"
#include "ragnarok/ecs/systems/S_Collision.h"
#include "ragnarok/ecs/systems/S_Control.h"
#include "ragnarok/ecs/systems/S_State.h"
#include "ragnarok/ecs/systems/S_SheetAnimation.h"
#include "ragnarok/ecs/systems/S_Sound.h"

Game::Game() 
    : m_window("Chapter 2", sf::Vector2u(800, 600), false), m_entityManager(&m_systemManager, &m_textureManager),
    m_guiManager(m_window.GetEventManager(), &m_context), m_soundManager(&m_audioManager), 
    m_gameMap(&m_window, &m_entityManager, &m_textureManager)
{
    SetUpClasses();
    SetUpECS();
    SetUpStates();

    m_fontManager.RequireResource("Main");
    m_stateManager->SwitchTo(ragnarok::StateType::Intro);
}

Game::~Game()
{
    m_fontManager.ReleaseResource("Main");
}

sf::Time Game::GetElapsed()
{
    return m_clock.getElapsedTime();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();
}

ragnarok::Window* Game::GetWindow()
{
    return &m_window;
}

void Game::Update()
{
    m_window.Update();
    m_stateManager->Update(m_elapsed);
    m_guiManager.Update(m_elapsed.asSeconds());
    m_soundManager.Update(m_elapsed.asSeconds());

    ragnarok::GUIEvent guiEvent;
    while(m_context.m_guiManager->PollEvent(guiEvent))
    {
        m_window.GetEventManager()->HandleEvent(guiEvent);
    }
}

void Game::Render()
{
    m_window.BeginDraw();
    // Render here.
    m_stateManager->Draw();
    m_guiManager.Render(m_window.GetRenderWindow());
    m_window.EndDraw();
}

void Game::LateUpdate()
{
    m_stateManager->ProcessRequests();
    RestartClock();
}

void Game::SetUpClasses()
{
    m_clock.restart();
    m_context.m_rand = &m_rand;
    srand(static_cast<unsigned int>(time(nullptr)));
    m_systemManager.SetEntityManager(&m_entityManager);

    m_context.m_wind = &m_window;
    m_context.m_eventManager = m_window.GetEventManager();
    m_context.m_textureManager = &m_textureManager;
    m_context.m_fontManager = &m_fontManager;
    m_context.m_audioManager = &m_audioManager;
    m_context.m_soundManager = &m_soundManager;
    m_context.m_gameMap = &m_gameMap;
    m_context.m_systemManager = &m_systemManager;
    m_context.m_entityManager = &m_entityManager;
    m_context.m_guiManager = &m_guiManager;

    m_stateManager = std::make_unique<ragnarok::StateManager>(&m_context);
    m_gameMap.SetStateManager(m_stateManager.get());
}

void Game::SetUpECS()
{
    m_entityManager.AddComponentType<ragnarok::C_Position>(ragnarok::Component::Position);
    m_entityManager.AddComponentType<ragnarok::C_SpriteSheet>(ragnarok::Component::SpriteSheet);
    m_entityManager.AddComponentType<ragnarok::C_State>(ragnarok::Component::State);
    m_entityManager.AddComponentType<ragnarok::C_Movable>(ragnarok::Component::Movable);
    m_entityManager.AddComponentType<ragnarok::C_Controller>(ragnarok::Component::Controller);
    m_entityManager.AddComponentType<ragnarok::C_Collidable>(ragnarok::Component::Collidable);
    m_entityManager.AddComponentType<ragnarok::C_SoundEmitter>(ragnarok::Component::SoundEmitter);
    m_entityManager.AddComponentType<ragnarok::C_SoundListener>(ragnarok::Component::SoundListener);

    m_systemManager.AddSystem<ragnarok::S_State>(ragnarok::System::State);
    m_systemManager.AddSystem<ragnarok::S_Control>(ragnarok::System::Control);
    m_systemManager.AddSystem<ragnarok::S_Movement>(ragnarok::System::Movement);
    m_systemManager.AddSystem<ragnarok::S_Collision>(ragnarok::System::Collision);
    m_systemManager.AddSystem<ragnarok::S_SheetAnimation>(ragnarok::System::SheetAnimation);
    m_systemManager.AddSystem<ragnarok::S_Sound>(ragnarok::System::Sound);
    m_systemManager.AddSystem<ragnarok::S_Renderer>(ragnarok::System::Renderer);

    m_systemManager.GetSystem<ragnarok::S_Collision>(ragnarok::System::Collision)->SetMap(&m_gameMap);
    m_systemManager.GetSystem<ragnarok::S_Movement>(ragnarok::System::Movement)->SetMap(&m_gameMap);
    m_systemManager.GetSystem<ragnarok::S_Sound>(ragnarok::System::Sound)->SetUp(&m_audioManager, &m_soundManager);
}

void Game::SetUpStates()
{
    m_stateManager->AddDependent(m_context.m_eventManager);
    m_stateManager->AddDependent(&m_guiManager);
    m_stateManager->AddDependent(&m_soundManager);
    m_stateManager->RegisterState<StateIntro>(ragnarok::StateType::Intro);
    m_stateManager->RegisterState<StateMainMenu>(ragnarok::StateType::MainMenu);
    m_stateManager->RegisterState<StateGame>(ragnarok::StateType::Game);
}