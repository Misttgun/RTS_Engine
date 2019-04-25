#include <algorithm>
#include <iostream>
#include "ragnarok/gui/elements/GUITextField.h"
#include "ragnarok/utilities/Utilities.h"

namespace ragnarok
{
    GUITextfield::GUITextfield(const std::string& t_name, GUIInterface* t_owner)
        : GUIElement(t_name, GUIElementType::Textfield, t_owner), m_cursorPosition(0), m_cursorSize(0), m_time(0.f), m_showCursor(false)
    {}

    GUITextfield::~GUITextfield()
    {}

    void GUITextfield::HandleChar(const char& t_char)
    {
        if(GetState() != GUIElementState::Clicked)
        {
            return;
        }

        if(t_char == 8)
        {
            // Backspace.
            if(!m_cursorPosition)
            {
                return;
            }

            auto text = GetText();
            text.erase(m_cursorPosition - 1, 1);
            SetText(text);
            ChangeCursorPosition(-1);

            return;
        }

        if(t_char < 32 || t_char > 126)
        {
            return;
        }

        auto text = GetText();
        text.insert(m_cursorPosition, 1, t_char);
        SetText(text);
        ChangeCursorPosition(+1);

        return;
    }

    void GUITextfield::HandleArrowKey(const std::string& t_eventName)
    {
        if(m_state != GUIElementState::Clicked)
        {
            return;
        }

        if(t_eventName == "Key_LeftArrow")
        {
            ChangeCursorPosition(-1);
        }
        else if(t_eventName == "Key_RightArrow")
        {
            ChangeCursorPosition(1);
        }

        m_needsRedraw = true;
    }

    void GUITextfield::ReadIn(std::stringstream& t_stream)
    {
        std::string content;
        Utils::ReadQuotedString(t_stream, content);
        m_visual.m_text.setString(content);
    }

    void GUITextfield::OnClick(const sf::Vector2f& t_mousePos)
    {
        // check the local coordinates and move cursor to click pos!
        SetState(GUIElementState::Clicked);
        m_cursorPosition = GetText().size();

        if(m_cursorPosition < 0)
        {
            m_cursorPosition = 0;
        }
    }

    void GUITextfield::OnRelease()
    {}

    void GUITextfield::OnHover(const sf::Vector2f& t_mousePos)
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Focused);
    }

    void GUITextfield::OnLeave()
    {
        if(m_state == GUIElementState::Clicked)
        {
            return;
        }

        SetState(GUIElementState::Neutral);
    }

    void GUITextfield::OnDefocus()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUITextfield::Update(float t_dT)
    {
        if(m_state != GUIElementState::Clicked)
        {
            return;
        }

        m_time += t_dT;

        if(m_time >= 0.5f)
        {
            m_showCursor = !m_showCursor; m_time = 0.f; m_needsRedraw = true;
        }

        m_cursorSize = m_visual.m_text.getCharacterSize();
        auto pos = GetPosition();
        pos.x += GetXForCursor();
        pos.x += 1;
        pos.y += 1;
        m_cursor[0].position = pos;
        m_cursor[1].position = pos;
        m_cursor[1].position.y += m_cursorSize;
    }

    void GUITextfield::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_visual.m_backgroundSolid);
        if(!m_style[m_state].m_glyph.empty())
        {
            t_target->draw(m_visual.m_glyph);
        }

        t_target->draw(m_visual.m_text);
        if(m_state != GUIElementState::Clicked)
        {
            return;
        }

        if(!m_showCursor)
        {
            return;
        }

        t_target->draw(m_cursor, 2, sf::Lines);
    }

    void GUITextfield::ChangeCursorPosition(int t_pos)
    {
        m_cursorPosition += t_pos;

        if(m_cursorPosition < 0)
        {
            m_cursorPosition = 0;
        }
        else if(m_cursorPosition > m_visual.m_text.getString().getSize())
        {
            m_cursorPosition = m_visual.m_text.getString().getSize();
        }
    }

    float GUITextfield::GetXForCursor()
    {
        auto str = m_visual.m_text.getString().toAnsiString();
        auto font = m_visual.m_text.getFont();
        float result = 0.f;

        for(size_t i = 0; i < m_cursorPosition; ++i)
        {
            auto glyph = font->getGlyph(str[i], m_visual.m_text.getCharacterSize(), false);
            result += glyph.advance;
        }

        return result;
    }
}