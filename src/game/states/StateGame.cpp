#include "StateGame.h"
#include "Window.h"
#include "StateManager.h"
#include "EventManager.h"

namespace SExE
{
    StateGame::StateGame(StateManager* t_stateManager): BaseState(t_stateManager)
    {}

    StateGame::~StateGame()
    {}

    void StateGame::onCreate()
    {
        m_texture.loadFromFile("res/Mushroom.png");
        m_sprite.setTexture(m_texture);
        m_sprite.setPosition(0, 0);
        m_increment = sf::Vector2f(400.0f, 400.0f);

        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->addCallback(StateType::Game, "Key_Escape", &StateGame::mainMenu, this);
        evMgr->addCallback(StateType::Game, "Key_P", &StateGame::pause, this);
    }

    void StateGame::onDestroy()
    {
        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->removeCallback(StateType::Game, "Key_Escape");
        evMgr->removeCallback(StateType::Game, "Key_P");
    }

    void StateGame::update(const sf::Time& t_time)
    {
        sf::Vector2u l_windSize = m_stateMgr->getContext()->m_wind->getWindowSize();
        sf::Vector2u l_textSize = m_texture.getSize();

        if((m_sprite.getPosition().x > l_windSize.x - l_textSize.x && m_increment.x > 0) ||
            (m_sprite.getPosition().x < 0 && m_increment.x < 0))
        {
            m_increment.x = -m_increment.x;
        }

        if((m_sprite.getPosition().y > l_windSize.y - l_textSize.y && m_increment.y > 0) ||
            (m_sprite.getPosition().y < 0 && m_increment.y < 0))
        {
            m_increment.y = -m_increment.y;
        }

        m_sprite.setPosition(m_sprite.getPosition().x + (m_increment.x * t_time.asSeconds()),
                             m_sprite.getPosition().y + (m_increment.y * t_time.asSeconds()));
    }

    void StateGame::draw()
    {
        m_stateMgr->getContext()->m_wind->getRenderWindow()->draw(m_sprite);
    }

    void StateGame::mainMenu(EventDetails* t_details)
    {
        m_stateMgr->switchTo(StateType::MainMenu);
    }

    void StateGame::pause(EventDetails* t_details)
    {
        m_stateMgr->switchTo(StateType::Paused);
    }

    void StateGame::activate()
    {}
    void StateGame::deactivate()
    {}
}