#include "../../../include/gui/elements/GUIDropDownMenu.h"
#include "../../../include/gui/GUIManager.h"
#include "../../../include/SharedContext.h"

namespace ragnarok
{
    GUIDropDownMenu::GUIDropDownMenu(const std::string& t_name, GUIInterface* t_owner)
        : GUIElement(t_name, GUIElementType::DropDownMenu, t_owner),
        m_dropDown(m_owner->GetManager()->GetContext()->m_eventManager, m_owner->GetManager(), this)
    {
        m_dropDown.Hide();
        SetText("Select...");
    }

    GUIDropDownMenu::~GUIDropDownMenu()
    {}

    GUIVerticalDropDown* GUIDropDownMenu::GetMenu()
    {
        return &m_dropDown;
    }

    void GUIDropDownMenu::ReadIn(std::stringstream& t_stream)
    {
        if(!(t_stream >> m_fileName))
        {
            return;
        }
    }

    void GUIDropDownMenu::OnClick(const sf::Vector2f& t_mousePos)
    {
        if(m_state == GUIElementState::Clicked)
        {
            SetState(GUIElementState::Neutral);
            m_dropDown.Hide();
            return;
        }

        SetState(GUIElementState::Clicked);
        m_dropDown.Show();
        m_dropDown.SetPosition(GetGlobalPosition() + sf::Vector2f(0.f, GetSize().y));
    }

    void GUIDropDownMenu::OnInterfaceClick(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Neutral);
        m_dropDown.Hide();
    }

    void GUIDropDownMenu::OnRelease()
    {}

    void GUIDropDownMenu::OnHover(const sf::Vector2f& t_mousePos)
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Focused);
    }

    void GUIDropDownMenu::OnLeave()
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Neutral);
    }

    void GUIDropDownMenu::OnFocus()
    {}

    void GUIDropDownMenu::OnDefocus()
    {
        if(m_state != GUIElementState::Clicked)
        {
            return;
        }

        m_dropDown.BringToFront();
    }

    void GUIDropDownMenu::Setup()
    {
        m_dropDown.Setup(m_owner->GetManager()->GetCurrentState(), "_dropdown_" + m_name, m_fileName);
    }

    void GUIDropDownMenu::Update(float t_dT)
    {}

    void GUIDropDownMenu::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_visual.m_backgroundSolid);

        if(!m_style[m_state].m_glyph.empty())
        {
            t_target->draw(m_visual.m_glyph);
        }

        t_target->draw(m_visual.m_text);
    }
}
