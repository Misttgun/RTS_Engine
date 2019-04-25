#include "../../../include/gui/elements/GUILabel.h"
#include "../../../include/utilities/Utilities.h"

namespace ragnarok
{
    GUILabel::GUILabel(const std::string& t_name, GUIInterface* t_owner) : GUIElement(t_name, GUIElementType::Label, t_owner)
    {}

    GUILabel::~GUILabel()
    {}

    void GUILabel::ReadIn(std::stringstream& t_stream)
    {
        std::string content;
        Utils::ReadQuotedString(t_stream, content);
        m_visual.m_text.setString(content);
    }

    void GUILabel::OnClick(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Clicked);
    }

    void GUILabel::OnRelease()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUILabel::OnHover(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Focused);
    }

    void GUILabel::OnLeave()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUILabel::Update(float t_dT)
    {}

    void GUILabel::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_visual.m_backgroundSolid);

        if(!m_style[m_state].m_glyph.empty())
        {
            t_target->draw(m_visual.m_glyph);
        }

        t_target->draw(m_visual.m_text);
    }
}
