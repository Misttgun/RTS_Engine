#include "EventManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace SExE
{

    EventManager::EventManager()
        : m_hasFocus(true)
    {
        loadBindings();
    }

    EventManager::~EventManager()
    {
        for(auto &itr : m_bindings)
        {
            delete itr.second;
            itr.second = nullptr;
        }
    }

    bool EventManager::addBinding(Binding * t_binding)
    {
        if(m_bindings.find(t_binding->m_name) != m_bindings.end())
            return false;

        return m_bindings.emplace(t_binding->m_name, t_binding).second;
    }

    bool EventManager::removeBinding(const std::string& t_name)
    {
        auto itr = m_bindings.find(t_name);
        if(itr == m_bindings.end())
        {
            return false;
        }
        delete itr->second;
        m_bindings.erase(itr);

        return true;
    }

    void EventManager::setFocus(const bool & t_focus)
    {
        m_hasFocus = t_focus;
    }

    void EventManager::handleEvent(sf::Event & t_event)
    {
        for(auto &b_itr : m_bindings)
        {
            Binding* bind = b_itr.second;
            for(auto &e_itr : bind->m_events)
            {
                const auto sfmlEvent = static_cast<EventType>(t_event.type);
                if(e_itr.first != sfmlEvent)
                {
                    continue;
                }

                if(sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp)
                {
                    if(e_itr.second.m_code == t_event.key.code)
                    {
                        if(bind->m_details.m_keyCode != -1)
                        {
                            bind->m_details.m_keyCode = e_itr.second.m_code;
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
                        bind->m_details.m_mouse.x = t_event.mouseButton.x;
                        bind->m_details.m_mouse.y = t_event.mouseButton.y;

                        if(bind->m_details.m_keyCode != -1)
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

    void EventManager::update()
    {
        if(!m_hasFocus)
        {
            return;
        }

        for(auto &b_itr : m_bindings)
        {
            Binding* bind = b_itr.second;
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

                            // Increase count.
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

                            // Increase count.
                            ++(bind->c);
                        }
                        break;

                    default:;
                }
            }

            if(bind->m_events.size() == bind->c)
            {
                auto callItr = m_callbacks.find(bind->m_name);
                if(callItr != m_callbacks.end())
                {
                    callItr->second(&bind->m_details);
                }
            }

            bind->c = 0;
            bind->m_details.clear();
        }
    }

    sf::Vector2i EventManager::getMousePos(sf::RenderWindow* t_window = nullptr) const
    {
        return (t_window ? sf::Mouse::getPosition(*t_window) : sf::Mouse::getPosition());
    }

    void EventManager::loadBindings()
    {
        std::string delimiter = ":";

        std::ifstream bindings;
        bindings.open("keys.cfg");

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
            auto bind = new Binding(callbackName);
            while(!keystream.eof())
            {
                std::string keyval;
                keystream >> keyval;
                const int start = 0;
                const int end = keyval.find(delimiter);
                if(end == std::string::npos)
                {
                    delete bind;
                    bind = nullptr; 
                    break;
                }

                const EventType type = EventType(stoi(keyval.substr(start, end - start)));
                const int code = stoi(keyval.substr(end + delimiter.length(), keyval.find(delimiter, end + delimiter.length())));
                EventInfo eventInfo;
                eventInfo.m_code = code;
                bind->BindEvent(type, eventInfo);
            }

            if(!addBinding(bind))
            {
                delete bind;
            }

            bind = nullptr;
        }

        bindings.close();
    }

}
