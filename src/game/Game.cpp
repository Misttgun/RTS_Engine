#include "Game.h"
#include "states/StateIntro.h"
#include "states/StateGame.h"
#include "states/StateMainMenu.h"
#include "states/StatePaused.h"

namespace SExE
{
    Game::Game() : m_window("Demo", sf::Vector2u(800, 600), false)
    {
        setUpClasses();
        setUpECS();
        setUpStates();

        m_stateManager->switchTo(StateType::Intro);
    }

    void Game::update()
    {
        m_window.update();
        m_stateManager->update(m_elapsed);
    }

    void Game::render()
    {
        m_window.beginDraw();
        m_stateManager->draw();
        m_window.endDraw();
    }

    void Game::lateUpdate()
    {
        m_stateManager->processRequests();
        restartClock();
    }

    Window* Game::getWindow()
    {
        return &m_window;
    }
    sf::Time Game::getElapsed() const
    {
        return m_elapsed;
    }

    void Game::restartClock()
    {
        m_elapsed = m_clock.restart();
    }

    void Game::setUpClasses()
    {
        m_clock.restart();
        //m_context.m_rand = &m_rand;
        srand(static_cast<unsigned int>(time(nullptr)));

        m_context.m_wind = &m_window;
        m_context.m_eventManager = m_window.getEventManager();

        m_stateManager = std::make_unique<StateManager>(&m_context);
    }

    void Game::setUpECS()
    {}

    void Game::setUpStates()
    {
        m_stateManager->addDependent(m_context.m_eventManager);
        //m_stateManager->addDependent(&m_guiManager);
        //m_stateManager->addDependent(&m_soundManager);
        m_stateManager->registerState<StateIntro>(StateType::Intro);
        m_stateManager->registerState<StateMainMenu>(StateType::MainMenu);
        m_stateManager->registerState<StateGame>(StateType::Game);
        m_stateManager->registerState<StatePaused>(StateType::Paused);
    }
}
