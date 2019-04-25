#include "../../../include/gui/elements/GUICheckBox.h"

namespace ragnarok
{
    GUICheckBox::GUICheckBox(const std::string & t_name, GUIInterface * t_owner)
        : GUIElement(t_name, GUIElementType::CheckBox, t_owner)
    {}

    GUICheckBox::~GUICheckBox()
    {}

    void GUICheckBox::ReadIn(std::stringstream & t_stream)
    {}

    void GUICheckBox::OnClick(const sf::Vector2f & t_mousePos)
    {
        if(m_state == GUIElementState::Clicked)
        {
            SetState(GUIElementState::Neutral);
        }
        else
        {
            SetState(GUIElementState::Clicked);
        }
    }

    void GUICheckBox::OnRelease()
    {}

    void GUICheckBox::OnHover(const sf::Vector2f & t_mousePos)
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Focused);
    }

    void GUICheckBox::OnLeave()
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Neutral);
    }

    void GUICheckBox::Update(float t_dT)
    {}

    void GUICheckBox::Draw(sf::RenderTarget* t_target)
    {}
}
