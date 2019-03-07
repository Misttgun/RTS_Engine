#pragma once
#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventTypes.h"
#include "GUIEvent.h"

namespace SExE
{
    struct EventInfo
    {
        EventInfo() : m_type(EventInfoType::Normal), m_code(0)
        {}

        EventInfo(int t_eventCode) : m_type(EventInfoType::Normal), m_code(t_eventCode)
        {}

        EventInfo(const GUIEvent& t_guiEvent) : m_type(EventInfoType::GUI), m_gui(t_guiEvent)
        {}

        EventInfo(const EventInfoType& t_type)
        {
            if(m_type == EventInfoType::GUI)
            {
                destroyGUIStrings();
            }

            m_type = t_type;
            if(m_type == EventInfoType::GUI)
            {
                createGUIStrings("", "");
            }
        }

        EventInfo(const EventInfo& t_rhs)
        {
            move(t_rhs);
        }

        EventInfo& operator=(const EventInfo& t_rhs)
        {
            if(&t_rhs != this)
            {
                move(t_rhs);
            }

            return *this;
        }

        ~EventInfo()
        {
            if(m_type == EventInfoType::GUI)
            {
                destroyGUIStrings();
            }
        }

        union
        {
            int m_code;
            GUIEvent m_gui;
        };

        EventInfoType m_type;
        
        private:
        void move(const EventInfo& t_rhs)
        {
            if(m_type == EventInfoType::GUI)
            {
                destroyGUIStrings();
            }
            m_type = t_rhs.m_type;
            if(m_type == EventInfoType::Normal)
            {
                m_code = t_rhs.m_code;
            }
            else
            {
                createGUIStrings(t_rhs.m_gui.m_interface, t_rhs.m_gui.m_element); m_gui = t_rhs.m_gui;
            }
        }

        void destroyGUIStrings()
        {
            m_gui.m_interface.~basic_string();
            m_gui.m_element.~basic_string();
        }

        void createGUIStrings(const std::string& t_interface, const std::string& t_element)
        {
            new (&m_gui.m_interface) std::string(t_interface);
            new (&m_gui.m_element) std::string(t_element);
        }
    };
}

#endif // !EVENTINFO_H