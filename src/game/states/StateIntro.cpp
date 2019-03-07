#include "StateIntro.h"
#include "EventManager.h"
#include "Window.h"
#include "StateManager.h"

namespace SExE
{
    StateIntro::StateIntro(StateManager* t_stateManager) : BaseState(t_stateManager)
    {}

    StateIntro::~StateIntro()
    {}

    void StateIntro::onCreate()
    {
        m_timePassed = 0.0f;

        sf::Vector2u windowSize = m_stateMgr->getContext()->m_wind->getRenderWindow()->getSize();

        m_introTexture.loadFromFile("res/intro.png");
        m_introSprite.setTexture(m_introTexture);
        m_introSprite.setOrigin(m_introTexture.getSize().x / 2.0f, m_introTexture.getSize().y / 2.0f);

        m_introSprite.setPosition(windowSize.x / 2.0f, 0);

        m_font.loadFromFile("res/arial.ttf");
        m_text.setFont(m_font);
        m_text.setString({"Press SPACE to continue"});
        m_text.setCharacterSize(15);
        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->addCallback("Intro_Continue", &StateIntro::carryOn, this);
    }

    void StateIntro::onDestroy()
    {
        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->removeCallback(StateType::Intro, "Intro_Continue");
    }

    void StateIntro::update(const sf::Time& t_time)
    {
        if(m_timePassed < 5.0f)
        {
            m_timePassed += t_time.asSeconds();
            m_introSprite.setPosition(m_introSprite.getPosition().x, m_introSprite.getPosition().y + (48 * t_time.asSeconds()));
        }
    }

    void StateIntro::draw()
    {
        sf::RenderWindow* window = m_stateMgr->getContext()->m_wind->getRenderWindow();

        window->draw(m_introSprite);
        if(m_timePassed >= 5.0f)
        {
            window->draw(m_text);
        }
    }

    void StateIntro::carryOn(EventDetails* t_details)
    {
        if(m_timePassed >= 5.0f)
        {
            m_stateMgr->switchTo(StateType::MainMenu);
            m_stateMgr->remove(StateType::Intro);
        }
    }

    void StateIntro::activate()
    {}
    void StateIntro::deactivate()
    {}
}