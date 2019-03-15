#pragma once

#include "EventTypes.h"
#include "../gui/GUIEvent.h"

namespace ragnarok
{
    /* Information about the event (type[normal, GUI], code and gui_event)*/
    struct EventInfo
    {
        EventInfo() : m_code(0), m_type(EventInfoType::Normal)
        {}

        EventInfo(int t_eventCode) : m_code(t_eventCode), m_type(EventInfoType::Normal)
        {}

        EventInfo(const GUIEvent& t_guiEvent) : m_gui(t_guiEvent), m_type(EventInfoType::GUI)
        {}

        EventInfo(const EventInfoType& t_type)
        {
            if(m_type == EventInfoType::GUI)
            {
                DestroyGUIStrings();
            }

            m_type = t_type;

            if(m_type == EventInfoType::GUI)
            {
                CreateGUIStrings("", "");
            }
        }

        EventInfo(const EventInfo& t_rhs)
        {
            Move(t_rhs);
        }

        EventInfo& operator=(const EventInfo& t_rhs)
        {
            if(&t_rhs != this)
            {
                Move(t_rhs);
            }

            return *this;
        }

        ~EventInfo()
        {
            if(m_type == EventInfoType::GUI)
            {
                DestroyGUIStrings();
            }
        }

        union
        {
            int m_code;
            GUIEvent m_gui;
        };

        EventInfoType m_type;

        private:
        void Move(const EventInfo& t_rhs)
        {
            if(m_type == EventInfoType::GUI)
            {
                DestroyGUIStrings();
            }

            m_type = t_rhs.m_type;

            if(m_type == EventInfoType::Normal)
            {
                m_code = t_rhs.m_code;
            }
            else
            {
                CreateGUIStrings(t_rhs.m_gui.m_interface, t_rhs.m_gui.m_element); 
                m_gui = t_rhs.m_gui;
            }
        }

        void DestroyGUIStrings()
        {
            m_gui.m_interface.~basic_string();
            m_gui.m_element.~basic_string();
        }

        void CreateGUIStrings(const std::string& t_interface, const std::string& t_element)
        {
            new (&m_gui.m_interface) std::string(t_interface);
            new (&m_gui.m_element) std::string(t_element);
        }
    };
}