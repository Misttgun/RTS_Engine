#include "EventManager.h"
#include "StateManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace SExE
{

    EventManager::EventManager() : m_hasFocus(true)
    {
        loadBindings();
    }

    EventManager::~EventManager()
    {}

    bool EventManager::addBinding(std::unique_ptr<Binding> t_binding)
    {
        if(m_bindings.find(t_binding->m_name) != m_bindings.end())
            return false;

        return m_bindings.emplace(t_binding->m_name, std::move(t_binding)).second;
    }

    bool EventManager::removeBinding(const std::string& t_name)
    {
        auto itr = m_bindings.find(t_name);
        if(itr == m_bindings.end())
        {
            return false;
        }

        m_bindings.erase(itr);
        return true;
    }

    void EventManager::changeState(const StateType& t_state)
    {
        setState(t_state);
    }

    void EventManager::removeState(const StateType& t_state)
    {
        m_callbacks.erase(t_state);
    }

    void EventManager::setFocus(bool t_focus)
    {
        m_hasFocus = t_focus;
    }

    void EventManager::handleEvent(sf::Event& t_event)
    {
        // Handling SFML events.
        for(auto &b_itr : m_bindings)
        {
            auto& bind = b_itr.second;
            for(auto &e_itr : bind->m_events)
            {
                EventType sfmlEvent = static_cast<EventType>(t_event.type);
                if(e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
                   e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave)
                {
                    continue;
                }

                if(e_itr.first != sfmlEvent)
                {
                    continue;
                }

                if(sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp)
                {
                    if(e_itr.second.m_code == t_event.key.code)
                    {
                        // Matching event/keystroke.
                        if(bind->m_details.m_keyCode == -1)
                        {
                            bind->m_details.m_keyCode = e_itr.second.m_code;
                        }

                        if(sfmlEvent == EventType::KeyDown)
                        {
                            bind->m_details.m_controlPressed = t_event.key.control;
                            bind->m_details.m_altPressed = t_event.key.alt;
                            bind->m_details.m_shiftPressed = t_event.key.shift;
                            bind->m_details.m_systemPressed = t_event.key.system;
                        }

                        // Increase count.
                        ++(bind->c);
                        break;
                    }
                }
                else if(sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp)
                {
                    if(e_itr.second.m_code == t_event.mouseButton.button)
                    {
                        // Matching event/keystroke.
                        bind->m_details.m_mouse.x = t_event.mouseButton.x;
                        bind->m_details.m_mouse.y = t_event.mouseButton.y;

                        if(bind->m_details.m_keyCode == -1)
                        {
                            bind->m_details.m_keyCode = e_itr.second.m_code;
                        }

                        // Increase count.
                        ++(bind->c);
                        break;
                    }
                }
                else
                {
                    // No need for additional checking.
                    if(sfmlEvent == EventType::MouseWheel)
                    {
                        bind->m_details.m_mouseWheelDelta = t_event.mouseWheel.delta;
                    }

                    else if(sfmlEvent == EventType::WindowResized)
                    {
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

    void EventManager::handleEvent(GUIEvent& t_event)
    {
        for(auto &b_itr : m_bindings)
        {
            auto& bind = b_itr.second;
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

                if(e_itr.second.m_gui.m_interface != t_event.m_interface)
                {
                    continue;
                }

                if(e_itr.second.m_gui.m_element == "*")
                {
                    if(t_event.m_element.empty())
                    {
                        continue;
                    }
                }
                else if(e_itr.second.m_gui.m_element != t_event.m_element)
                {
                    continue;
                }

                bind->m_details.m_guiInterface = t_event.m_interface;
                bind->m_details.m_guiElement = t_event.m_element;
                bind->m_details.m_mouse = sf::Vector2i(t_event.m_clickCoords.x, t_event.m_clickCoords.y);

                // Increase count.
                ++(bind->c);
            }
        }
    }

    void EventManager::update()
    {
        if(!m_hasFocus)
        {
            return;
        }

        for(auto &b_itr : m_bindings)
        {
            auto& bind = b_itr.second;
            for(auto &e_itr : bind->m_events)
            {
                switch(e_itr.first)
                {
                    case(EventType::Keyboard):
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code)))
                        {
                            if(bind->m_details.m_keyCode != -1)
                            {
                                bind->m_details.m_keyCode = e_itr.second.m_code;
                            }

                            ++(bind->c);
                        }
                        break;
                    case(EventType::Mouse):
                        if(sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code)))
                        {
                            if(bind->m_details.m_keyCode != -1)
                            {
                                bind->m_details.m_keyCode = e_itr.second.m_code;
                            }

                            ++(bind->c);
                        }
                        break;
                }
            }

            if(bind->m_events.size() == bind->c)
            {
                auto otherCallbacks = m_callbacks.find(StateType(0));
                auto stateCallbacks = m_callbacks.find(m_currentState);

                if(otherCallbacks != m_callbacks.end())
                {
                    auto callItr = otherCallbacks->second.find(bind->m_name);
                    if(callItr != otherCallbacks->second.end())
                    {
                        // Pass in information about events.
                        callItr->second(&bind->m_details);
                    }
                }

                if(stateCallbacks != m_callbacks.end())
                {
                    auto callItr = stateCallbacks->second.find(bind->m_name);
                    if(callItr != stateCallbacks->second.end())
                    {
                        // Pass in information about events.
                        callItr->second(&bind->m_details);
                    }
                }
            }
            bind->c = 0;
            bind->m_details.clear();
        }
    }

    void EventManager::loadBindings()
    {
        std::string delimiter = ":";

        std::ifstream bindings;
        bindings.open(Utils::getWorkingDirectory() + "res/keys.cfg");
        
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

                EventType type = EventType(stoi(keyval.substr(start, end - start)));

                if(type == EventType::GUI_Click || type == EventType::GUI_Release ||
                   type == EventType::GUI_Hover || type == EventType::GUI_Leave ||
                   type == EventType::GUI_Focus || type == EventType::GUI_Defocus)
                {
                    start = end + delimiter.length();
                    end = keyval.find(delimiter, start);
                    std::string window = keyval.substr(start, end - start);
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
                    EventInfo eventInfo(guiEvent);
                    bind->bindEvent(type, std::move(eventInfo));
                }
                else
                {
                    int code = std::stoi(keyval.substr(end + delimiter.length(), keyval.find(delimiter, end + delimiter.length())));
                    EventInfo eventInfo(code);
                    bind->bindEvent(type, std::move(eventInfo));
                }
            }

            if(!addBinding(std::move(bind)))
            {
                std::cout << "Failed adding binding..." << std::endl;
            }
        }
        bindings.close();
    }
}
