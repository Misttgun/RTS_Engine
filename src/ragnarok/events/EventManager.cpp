#include "ragnarok/events/EventManager.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/utilities/Utilities.h"
#include <iostream>
#include <fstream>
#include "ragnarok/gui/GUIEvent.h"

namespace ragnarok
{
    EventManager::EventManager() : m_hasFocus(true)
    {
        LoadBindings();
    }

    bool EventManager::AddBinding(std::unique_ptr<Binding> t_binding)
    {
        // Is the binding already in the container ?
        if(m_bindings.find(t_binding->m_name) != m_bindings.end())
            return false;

        // Add the new binding to the m_bindings container
        return m_bindings.emplace(t_binding->m_name, std::move(t_binding)).second;
    }

    bool EventManager::RemoveBinding(const std::string& t_name)
    {
        // // Is the binding already in the container ?
        const auto itr = m_bindings.find(t_name);
        if(itr == m_bindings.end())
        {
            return false;
        }

        // Remove the binding from the m_bindings container
        m_bindings.erase(itr);
        return true;
    }

    void EventManager::ChangeState(const StateType& t_state)
    {
        SetState(t_state);
    }

    void EventManager::RemoveState(const StateType& t_state)
    {
        m_callbacks.erase(t_state);
    }

    void EventManager::SetFocus(const bool t_focus)
    {
        m_hasFocus = t_focus;
    }

    void EventManager::HandleEvent(sf::Event& t_event)
    {
        for(auto &b_itr : m_bindings)
        {
            // For each binding in m_bindings
            auto& bind = b_itr.second;

            // For each pair <EventType, EventInfo> in bind
            for(auto &e_itr : bind->m_events)
            {
                const auto sfmlEvent = static_cast<EventType>(t_event.type);

                // We dont treat GUI event in this function
                if(e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
                   e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave)
                {
                    continue;
                }

                // If the current EventType is not the right one
                if(e_itr.first != sfmlEvent)
                {
                    continue;
                }

                // Keyboard event
                if(sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp)
                {
                    // Matching keystroke
                    if(e_itr.second.m_code == t_event.key.code)
                    {
                        // If keycode in Eventdetails is not initialised
                        if(bind->m_details.m_keyCode == -1)
                        {
                            // Set the keycode value
                            bind->m_details.m_keyCode = e_itr.second.m_code;
                        }

                        // If key is pressed down
                        if(sfmlEvent == EventType::KeyDown)
                        {
                            // Set bool flag for ctrl, shift, alt and window button
                            bind->m_details.m_controlPressed = t_event.key.control;
                            bind->m_details.m_altPressed = t_event.key.alt;
                            bind->m_details.m_shiftPressed = t_event.key.shift;
                            bind->m_details.m_systemPressed = t_event.key.system;
                        }

                        // Increase count
                        ++(bind->c);
                        break;
                    }
                }
                // Mouse event
                else if(sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp)
                {
                    // Matching keystroke
                    if(e_itr.second.m_code == t_event.mouseButton.button)
                    {
                        // Set mouse x and y position
                        bind->m_details.m_mouse.x = t_event.mouseButton.x;
                        bind->m_details.m_mouse.y = t_event.mouseButton.y;

                        // If keycode in Eventdetails is unitialised
                        if(bind->m_details.m_keyCode == -1)
                        {
                            // Set the keycode value
                            bind->m_details.m_keyCode = e_itr.second.m_code;
                        }

                        // Increase count.
                        ++(bind->c);
                        break;
                    }
                }
                else
                {
                    if(sfmlEvent == EventType::MouseWheel)
                    {
                        // Set mousewheel delta
                        bind->m_details.m_mouseWheelDelta = t_event.mouseWheel.delta;
                    }
                    else if(sfmlEvent == EventType::WindowResized)
                    {
                        // Set window size
                        bind->m_details.m_size.x = t_event.size.width;
                        bind->m_details.m_size.y = t_event.size.height;
                    }
                    else if(sfmlEvent == EventType::TextEntered)
                    {                       
                        bind->m_details.m_textEntered = t_event.text.unicode;
                    }

                    // Increase count.
                    ++(bind->c);
                }
            }
        }
    }

    void EventManager::HandleEvent(GUIEvent& t_event)
    {
        for(auto &b_itr : m_bindings)
        {
            // For each binding in m_bindings
            auto& bind = b_itr.second;

            // For each pair <EventType, EventInfo> in bind
            for(auto &e_itr : bind->m_events)
            {
                if(e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Release &&
                   e_itr.first != EventType::GUI_Hover && e_itr.first != EventType::GUI_Leave &&
                   e_itr.first != EventType::GUI_Focus && e_itr.first != EventType::GUI_Defocus)
                {
                    continue;
                }

                if((e_itr.first == EventType::GUI_Click && t_event.m_type != GUIEventType::Click) ||
                    (e_itr.first == EventType::GUI_Release && t_event.m_type != GUIEventType::Release) ||
                   (e_itr.first == EventType::GUI_Hover && t_event.m_type != GUIEventType::Hover) ||
                   (e_itr.first == EventType::GUI_Leave && t_event.m_type != GUIEventType::Leave) ||
                   (e_itr.first == EventType::GUI_Focus && t_event.m_type != GUIEventType::Focus) ||
                   (e_itr.first == EventType::GUI_Defocus && t_event.m_type != GUIEventType::Defocus))
                {
                    continue;
                }

                // GUIEvent interface is different
                if(e_itr.second.m_gui.m_interface != t_event.m_interface)
                {
                    continue;
                }


                if(e_itr.second.m_gui.m_element == "*")
                {
                    // GUIEvent element is empty
                    if(t_event.m_element.empty())
                    {
                        continue;
                    }
                }
                // GUIEvent element is different
                else if(e_itr.second.m_gui.m_element != t_event.m_element)
                {
                    continue;
                }

                // Set the interface, element and mouse coordinates
                bind->m_details.m_guiInterface = t_event.m_interface;
                bind->m_details.m_guiElement = t_event.m_element;
                bind->m_details.m_mouse = sf::Vector2i(t_event.m_clickCoords.x, t_event.m_clickCoords.y);

                // Increase count.
                ++(bind->c);
            }
        }
    }

    void EventManager::Update()
    {
        // Don't update if we lost focus
        if(!m_hasFocus)
        {
            return;
        }

        for(auto &b_itr : m_bindings)
        {
            // For each binding in m_bindings
            auto& bind = b_itr.second;

            // For each pair <EventType, EventInfo> in bind
            for(auto &e_itr : bind->m_events)
            {
                switch(e_itr.first)
                {
                    case(EventType::Keyboard):
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code)))
                        {
                            // If keycode in Eventdetails is initialised
                            if(bind->m_details.m_keyCode != -1)
                            {
                                // Set the keycode value
                                bind->m_details.m_keyCode = e_itr.second.m_code;
                            }

                            // Increase count.
                            ++(bind->c);
                        }

                        break;
                    case(EventType::Mouse):
                        if(sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code)))
                        {
                            // If keycode in Eventdetails is initialised
                            if(bind->m_details.m_keyCode != -1)
                            {
                                // Set the keycode value
                                bind->m_details.m_keyCode = e_itr.second.m_code;
                            }

                            // Increase count.
                            ++(bind->c);
                        }

                        break;
                default: ;
                }
            }

            // Number of events in container matches the count
            if(bind->m_events.size() == bind->c)
            {
                // Try to find callbacks that are state independant
                auto otherCallbacks = m_callbacks.find(StateType(0));
                // Try to find callbacks associated to the current state
                auto stateCallbacks = m_callbacks.find(m_currentState);

                // Found state independant callbacks
                if(otherCallbacks != m_callbacks.end())
                {
                    // Try to find the binding in state independant callbacks
                    auto callItr = otherCallbacks->second.find(bind->m_name);

                    // Found the binding
                    if(callItr != otherCallbacks->second.end())
                    {
                        // Pass in information about events.
                        callItr->second(&bind->m_details);
                    }
                }

                // Found current state callbacks
                if(stateCallbacks != m_callbacks.end())
                {
                    // Try to find the binding in current state callbacks
                    auto callItr = stateCallbacks->second.find(bind->m_name);

                    // Found the binding
                    if(callItr != stateCallbacks->second.end())
                    {
                        // Pass in information about events.
                        callItr->second(&bind->m_details);
                    }
                }
            }

            // Reset count
            bind->c = 0;

            // Clear details
            bind->m_details.Clear();
        }
    }

    void EventManager::LoadBindings()
    {
        std::string delimiter = ":";

        std::ifstream bindings;
        bindings.open(Utils::GetWorkingDirectory() + "res/keys.cfg");

        if(!bindings.is_open())
        {
            std::cout << "! Failed loading keys.cfg." << std::endl; return;
        }

        std::string line;
        while(std::getline(bindings, line))
        {
            std::stringstream keystream(line);
            std::string callbackName;
            keystream >> callbackName;
            auto bind = std::make_unique<Binding>(callbackName);

            while(!keystream.eof())
            {
                std::string keyval;
                keystream >> keyval;
                int start = 0;
                int end = keyval.find(delimiter);

                if(end == std::string::npos)
                {
                    bind.release(); break;
                }

                const EventType type = EventType(stoi(keyval.substr(start, end - start)));

                if(type == EventType::GUI_Click || type == EventType::GUI_Release ||
                   type == EventType::GUI_Hover || type == EventType::GUI_Leave ||
                   type == EventType::GUI_Focus || type == EventType::GUI_Defocus)
                {
                    start = end + delimiter.length();
                    end = keyval.find(delimiter, start);
                    const std::string window = keyval.substr(start, end - start);
                    std::string element;

                    if(end != std::string::npos)
                    {
                        start = end + delimiter.length();
                        end = keyval.length();
                        element = keyval.substr(start, end);
                    }

                    GUIEvent guiEvent;
                    guiEvent.m_interface = window;
                    guiEvent.m_element = element;
                    const EventInfo eventInfo(guiEvent);
                    bind->BindEvent(type, eventInfo);
                }
                else
                {
                    const int code = std::stoi(keyval.substr(end + delimiter.length(), keyval.find(delimiter, end + delimiter.length())));
                    const EventInfo eventInfo(code);
                    bind->BindEvent(type, eventInfo);
                }
            }

            if(!AddBinding(std::move(bind)))
            {
                std::cout << "Failed adding binding..." << std::endl;
            }
        }
        bindings.close();
    }
}
