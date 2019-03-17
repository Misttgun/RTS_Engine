#pragma once

#include "../GUIElement.h"

namespace ragnarok
{
    class GUITextfield : public GUIElement
    {
        public:
        GUITextfield(const std::string& t_name, GUIInterface* t_owner);
        ~GUITextfield();

        void HandleChar(const char& t_char);
        void HandleArrowKey(const std::string& t_eventName);

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void OnDefocus();
        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        private:
        void ChangeCursorPosition(int t_pos);
        float GetXForCursor();
        sf::Vertex m_cursor[2];
        int m_cursorPosition;
        size_t m_cursorSize;

        float m_time;
        bool m_showCursor;
    };
}