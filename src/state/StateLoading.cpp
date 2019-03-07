#include "StateLoading.h"
#include "EventManager.h"
#include "Window.h"
#include "StateManager.h"
#include "Helpers.h"

namespace SExE
{

    StateLoading::StateLoading(StateManager* t_stateManager) : BaseState(t_stateManager), m_originalWork(0), m_percentage(0), m_manualContinue(true)
    {}

    StateLoading::~StateLoading()
    {}

    void StateLoading::onCreate()
    {
        auto context = m_stateMgr->getContext();
        //context->m_fontManager->RequireResource("Main");
        //m_text.setFont(*context->m_fontManager->GetResource("Main"));
        m_text.setCharacterSize(14);
        m_text.setStyle(sf::Text::Bold);

        sf::Vector2u windowSize = m_stateMgr->getContext()->m_wind->getRenderWindow()->getSize();

        m_rect.setFillColor(sf::Color(0, 150, 0, 255));
        m_rect.setSize(sf::Vector2f(0.f, 16.f));
        m_rect.setOrigin(0.f, 8.f);
        m_rect.setPosition(0.f, windowSize.y / 2.f);

        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->addCallback(StateType::Loading, "Key_Space", &StateLoading::proceed, this);
    }

    void StateLoading::onDestroy()
    {
        auto context = m_stateMgr->getContext();
        EventManager* evMgr = context->m_eventManager;
        evMgr->removeCallback(StateType::Loading, "Key_Space");
        //context->m_fontManager->ReleaseResource("Main");
    }

    void StateLoading::update(const sf::Time& t_time)
    {
        if(m_loaders.empty())
        {
            if(!m_manualContinue)
            {
                proceed(nullptr);
            }

            return;
        }
        
        auto windowSize = m_stateMgr->getContext()->m_wind->getRenderWindow()->getSize();
        if(m_loaders.back()->isDone())
        {
            m_loaders.back()->onRemove();
            m_loaders.pop_back();
            if(m_loaders.empty())
            {
                m_rect.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 16.f));
                updateText(".Press space to continue.", 100.f);
                return;
            }
        }

        if(!m_loaders.back()->hasStarted())
        {
            m_loaders.back()->begin();
        }

        auto percentage = calculatePercentage();
        updateText("", percentage);
        m_rect.setSize(sf::Vector2f((windowSize.x / 100) * percentage, 16.f));
    }

    void StateLoading::draw()
    {
        sf::RenderWindow* wind = m_stateMgr->getContext()->m_wind->getRenderWindow();
        wind->draw(m_rect);
        wind->draw(m_text);
    }

    void StateLoading::updateText(const std::string& t_text, float t_percentage)
    {
        m_text.setString(std::to_string(static_cast<int>(t_percentage)) + "%" + t_text);
        auto windowSize = m_stateMgr->getContext()->m_wind->getRenderWindow()->getSize();
        m_text.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
        Utils::centerSFMLText(m_text);
    }

    float StateLoading::calculatePercentage()
    {
        float absolute = 100.f;
        if(m_loaders.empty())
        {
            return absolute;
        }

        if(m_loaders.back()->getTotalLines())
        {
            float d = (100.f * (m_originalWork - m_loaders.size())) / static_cast<float>(m_originalWork);
            float current = (100.f * m_loaders.back()->getCurrentLine()) / static_cast<float>(m_loaders.back()->getTotalLines());
            float totalCurrent = current / static_cast<float>(m_originalWork);
            absolute = d + totalCurrent;
        }

        return absolute;
    }

    void StateLoading::setManualContinue(bool t_continue)
    {
        m_manualContinue = t_continue;
    }

    void StateLoading::proceed(EventDetails* t_details)
    {
        if(!m_loaders.empty())
        {
            return;
        }

        m_stateMgr->switchTo(m_stateMgr->getNextToLast());
    }

    void StateLoading::addLoader(FileLoader* t_loader)
    {
        m_loaders.emplace_back(t_loader); 
        t_loader->onAdd();
    }

    bool StateLoading::hasWork() const
    {
        return !m_loaders.empty();
    }

    void StateLoading::activate()
    {
        m_originalWork = m_loaders.size();
    }

    void StateLoading::deactivate()
    {}

}