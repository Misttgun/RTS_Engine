#include "ragnarok/states/StateLoading.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/resources/FontManager.h"
#include "ragnarok/utilities/Helpers.h"

namespace ragnarok
{
    StateLoading::StateLoading(StateManager* t_stateManager) : BaseState(t_stateManager), m_originalWork(0), m_manualContinue(true)
    {}

    void StateLoading::OnCreate()
    {
        auto context = m_stateMgr->GetContext();
        context->m_fontManager->RequireResource("Main");
        m_text.setFont(*context->m_fontManager->GetResource("Main"));
        m_text.setCharacterSize(14);
        m_text.setStyle(sf::Text::Bold);

        const sf::Vector2u windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();

        m_rect.setFillColor(sf::Color(0, 150, 0, 255));
        m_rect.setSize(sf::Vector2f(0.f, 16.f));
        m_rect.setOrigin(0.f, 8.f);
        m_rect.setPosition(0.f, windowSize.y / 2.f);

        EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
        evMgr->AddCallback(StateType::Loading, "Key_Space", &StateLoading::Proceed, this);
    }

    void StateLoading::OnDestroy()
    {
        auto context = m_stateMgr->GetContext();
        EventManager* evMgr = context->m_eventManager;
        evMgr->RemoveCallback(StateType::Loading, "Key_Space");
        context->m_fontManager->ReleaseResource("Main");
    }

    void StateLoading::Update(const sf::Time& t_time)
    {
        if (m_loaders.empty())
        {
            if (!m_manualContinue)
            {
                Proceed(nullptr);
            }

            return;
        }

        const auto windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();

        if (m_loaders.back()->IsDone())
        {
            m_loaders.back()->OnRemove();
            m_loaders.pop_back();

            if (m_loaders.empty())
            {
                m_rect.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 16.f));
                UpdateText(".Press space to continue.", 100.f);
                return;
            }
        }

        if (!m_loaders.back()->HasStarted())
        {
            m_loaders.back()->Begin();
        }

        const auto percentage = CalculatePercentage();
        UpdateText("", percentage);
        m_rect.setSize(sf::Vector2f((windowSize.x / 100) * percentage, 16.f));
    }

    void StateLoading::Draw()
    {
        sf::RenderWindow* wind = m_stateMgr->GetContext()->m_wind->GetRenderWindow();
        wind->draw(m_rect);
        wind->draw(m_text);
    }

    void StateLoading::UpdateText(const std::string& t_text, float t_percentage)
    {
        m_text.setString(std::to_string(static_cast<int>(t_percentage)) + "%" + t_text);
        const auto windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();
        m_text.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
        Utils::CenterSFMLText(m_text);
    }

    float StateLoading::CalculatePercentage()
    {
        float absolute = 100.f;

        if (m_loaders.empty())
        {
            return absolute;
        }

        if (m_loaders.back()->GetTotalLines())
        {
            const float d = (100.f * (m_originalWork - m_loaders.size())) / static_cast<float>(m_originalWork);
            const float current = (100.f * m_loaders.back()->GetCurrentLine()) / static_cast<float>(m_loaders.back()->GetTotalLines());
            const float totalCurrent = current / static_cast<float>(m_originalWork);
            absolute = d + totalCurrent;
        }

        return absolute;
    }

    void StateLoading::SetManualContinue(bool t_continue)
    {
        m_manualContinue = t_continue;
    }

    void StateLoading::Proceed(EventDetails* t_details)
    {
        if (!m_loaders.empty())
        {
            return;
        }

        m_stateMgr->SwitchTo(m_stateMgr->GetNextToLast());
    }

    void StateLoading::AddLoader(FileLoader* t_loader)
    {
        m_loaders.emplace_back(t_loader);
        t_loader->OnAdd();
    }

    bool StateLoading::HasWork() const
    {
        return !m_loaders.empty();
    }

    void StateLoading::Activate()
    {
        m_originalWork = m_loaders.size();
    }

    void StateLoading::Deactivate()
    {}
}