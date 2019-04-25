#include "../../../include/gui/elements/GUIScrollbar.h"
#include "../../../include/gui/GUIInterface.h"
#include "../../../include/gui/GUIManager.h"
#include "../../../include/window/Window.h"
#include "../../../include/SharedContext.h"

namespace ragnarok
{
    GUIScrollbar::GUIScrollbar(const std::string& t_name, GUIInterface* t_owner) : GUIElement(t_name, GUIElementType::Scrollbar, t_owner)
    {
        m_isControl = true;
    }

    GUIScrollbar::~GUIScrollbar()
    {}

    void GUIScrollbar::SetPosition(const sf::Vector2f& t_pos)
    {
        GUIElement::SetPosition(t_pos);

        if(m_sliderType == SliderType::Horizontal)
        {
            m_position.x = 0;
        }
        else
        {
            m_position.y = 0;
        }
    }

    void GUIScrollbar::ReadIn(std::stringstream& t_stream)
    {
        std::string type;
        t_stream >> type;

        if(type == "Horizontal")
        {
            m_sliderType = SliderType::Horizontal;
        }
        else
        {
            m_sliderType = SliderType::Vertical;
        }

        if(m_sliderType == SliderType::Horizontal)
        {
            m_slider.setPosition(0, GetPosition().y);
        }
        else
        {
            m_slider.setPosition(GetPosition().x, 0);
        }
    }

    void GUIScrollbar::OnClick(const sf::Vector2f& t_mousePos)
    {
        if(!m_slider.getGlobalBounds().contains(t_mousePos + GetPosition()))
        {
            return;
        }

        SetState(GUIElementState::Clicked);
        auto context = m_owner->GetManager()->GetContext();
        m_moveMouseLast = sf::Vector2f(context->m_eventManager->GetMousePos(context->m_wind->GetRenderWindow()));
    }

    void GUIScrollbar::OnRelease()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUIScrollbar::OnHover(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Focused);
    }

    void GUIScrollbar::OnLeave()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUIScrollbar::UpdateStyle(const GUIElementState& t_state, const GUIStyle& t_style)
    {
        GUIElement::UpdateStyle(t_state, t_style);

        if(m_sliderType == SliderType::Horizontal)
        {
            m_style[t_state].m_size.x = m_owner->GetSize().x;
        }
        else
        {
            m_style[t_state].m_size.y = m_owner->GetSize().y;
        }
    }

    void GUIScrollbar::ApplyStyle()
    {
        GUIElement::ApplyStyle();
        m_slider.setFillColor(m_style[m_state].m_elementColor);
        bool horizontal = m_sliderType == SliderType::Horizontal;
        auto& bgSolid = m_visual.m_backgroundSolid;
        SetPosition((horizontal ? sf::Vector2f(0, m_owner->GetSize().y - bgSolid.getSize().y) : sf::Vector2f(m_owner->GetSize().x - bgSolid.getSize().x, 0)));
        bgSolid.setSize((horizontal ? sf::Vector2f(m_owner->GetSize().x, m_style[m_state].m_size.y) : sf::Vector2f(m_style[m_state].m_size.x, m_owner->GetSize().y)));
        m_slider.setPosition((horizontal ? m_slider.getPosition().x : GetPosition().x), (horizontal ? GetPosition().y : m_slider.getPosition().y));
        float SizeFactor = (horizontal ? m_owner->GetContentSize().x / m_owner->GetSize().x : m_owner->GetContentSize().y / m_owner->GetSize().y);
        
        if(SizeFactor < 1.f)
        {
            SizeFactor = 1.f;
        }

        float SliderSize = (horizontal ? m_owner->GetSize().x : m_owner->GetSize().y) / SizeFactor;
        m_slider.setSize((horizontal ? sf::Vector2f(SliderSize, bgSolid.getSize().y) : sf::Vector2f(bgSolid.getSize().x, SliderSize)));
        bgSolid.setPosition(GetPosition());
    }

    void GUIScrollbar::Update(float t_dT)
    {
        // Mouse-drag code.
        if(GetState() != GUIElementState::Clicked)
        {
            return;
        }

        SharedContext* context = m_owner->GetManager()->GetContext();
        sf::Vector2f mousePos = sf::Vector2f(context->m_eventManager->GetMousePos(context->m_wind->GetRenderWindow()));
        
        if(m_moveMouseLast == mousePos)
        {
            return;
        }

        sf::Vector2f difference = mousePos - m_moveMouseLast;
        m_moveMouseLast = mousePos;

        bool horizontal = m_sliderType == SliderType::Horizontal;
        m_slider.move((horizontal ? difference.x : 0), (horizontal ? 0 : difference.y));
        
        if(horizontal && m_slider.getPosition().x < 0)
        {
            m_slider.setPosition(0, m_slider.getPosition().y);
        }
        else if(m_slider.getPosition().y < 0)
        {
            m_slider.setPosition(m_slider.getPosition().x, 0);
        }
        
        if(horizontal && (m_slider.getPosition().x + m_slider.getSize().x > m_owner->GetSize().x))
        {
            m_slider.setPosition(m_owner->GetSize().x - m_slider.getSize().x, m_slider.getPosition().y);
        }
        else if(m_slider.getPosition().y + m_slider.getSize().y > m_owner->GetSize().y)
        {
            m_slider.setPosition(m_slider.getPosition().x, m_owner->GetSize().y - m_slider.getSize().y);
        }
        
        // using owner's size, because the slider is still as big as the owner interface window.
        float WorkArea = (horizontal ? m_owner->GetSize().x - m_slider.getSize().x : m_owner->GetSize().y - m_slider.getSize().y);
        int percentage = static_cast<int>(((horizontal ? m_slider.getPosition().x : m_slider.getPosition().y) / WorkArea) * 100);
        
        if(horizontal)
        {
            m_owner->UpdateScrollHorizontal(percentage);
        }
        else
        {
            m_owner->UpdateScrollVertical(percentage);
        }
        
        SetRedraw(true);
    }

    void GUIScrollbar::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_visual.m_backgroundSolid);
        t_target->draw(m_slider);
    }
}
