#pragma once
#ifndef EVENTDETAILS_H
#define EVENTDETAILS_H

#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>
#include "GUIEvent.h"

namespace SExE
{
    struct EventDetails
    {
        EventDetails(const std::string& l_bindName) : m_name(l_bindName), m_hasBeenProcessed(false), m_controlPressed(false), 
            m_altPressed(false), m_shiftPressed(false), m_systemPressed(false)
        {
            clear();
        }

        std::string m_name;
        sf::Vector2i m_size;
        sf::Uint32 m_textEntered;
        sf::Vector2i m_mouse;
        int m_mouseWheelDelta;
        int m_keyCode; // Single key code.

        std::string m_guiInterface;
        std::string m_guiElement;
        GUIEventType m_guiEvent;

        bool m_hasBeenProcessed;

        bool m_controlPressed;
        bool m_altPressed;
        bool m_shiftPressed;
        bool m_systemPressed;

        void clear()
        {
            m_size = sf::Vector2i(0, 0);
            m_textEntered = 0;
            m_mouse = sf::Vector2i(0, 0);
            m_mouseWheelDelta = 0;
            m_keyCode = -1;
            m_guiInterface.clear();
            m_guiElement.clear();
            m_guiEvent = GUIEventType::None;
            m_hasBeenProcessed = false;
            m_controlPressed = false;
            m_altPressed = false;
            m_shiftPressed = false;
            m_systemPressed = false;
        }
    };
}

#endif // !EVENTDETAILS_H