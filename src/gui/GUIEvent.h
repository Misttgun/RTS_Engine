#pragma once
#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <string>

namespace SExE
{
    enum class GUIEventType
    {
        None, Click, Release, Hover, Leave, Focus, Defocus
    };

    struct ClickCoordinates
    {
        ClickCoordinates& operator=(const ClickCoordinates& t_rhs)
        {
            if(&t_rhs != this)
            {
                x = t_rhs.x;
                y = t_rhs.y;
            }
            return *this;
        }
        float x, y;
    };

    struct GUIEvent
    {
        GUIEvent& operator=(const GUIEvent& t_rhs)
        {
            if(&t_rhs != this)
            {
                m_type = t_rhs.m_type;
                m_element = t_rhs.m_element;
                m_interface = t_rhs.m_interface;
                m_clickCoords = t_rhs.m_clickCoords;
            }
            return *this;
        }

        GUIEventType m_type;
        std::string m_element;
        std::string m_interface;
        
        union
        {
            ClickCoordinates m_clickCoords;
        };
    };
}

#endif // !GUIEVENT_H