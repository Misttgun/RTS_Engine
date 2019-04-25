#pragma once
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>

namespace SExE
{
    // All the event type we are planning to support
    enum class EventType
    {
        KeyDown = sf::Event::KeyPressed,
        KeyUp = sf::Event::KeyReleased,
        MButtonDown = sf::Event::MouseButtonPressed,
        MButtonUp = sf::Event::MouseButtonReleased,
        MouseWheel = sf::Event::MouseWheelMoved,
        WindowResized = sf::Event::Resized,
        GainedFocus = sf::Event::GainedFocus,
        LostFocus = sf::Event::LostFocus,
        MouseEntered = sf::Event::MouseEntered,
        MouseLeft = sf::Event::MouseLeft,
        Closed = sf::Event::Closed,
        TextEntered = sf::Event::TextEntered,
        Keyboard = sf::Event::Count + 1, Mouse
    };

    // Represent a code associated to an event (Example: key code for keyboard event)
    struct EventInfo
    {
        EventInfo()
        {
            m_code = 0;
        }

        EventInfo(int t_event)
        {
            m_code = t_event;
        }

        union
        {
            int m_code;
        };
    };

    // Represent all the information we can possibly get in an event
    struct EventDetails
    {
        EventDetails(const std::string& t_bindName) : m_name(t_bindName)
        {
            clear();
        }

        std::string m_name;
        sf::Vector2i m_size;
        sf::Uint32 m_textEntered;
        sf::Vector2i m_mouse;
        int m_mouseWheelDelta;
        int m_keyCode;

        void clear()
        {
            m_size = sf::Vector2i(0, 0);
            m_textEntered = 0;
            m_mouse = sf::Vector2i(0, 0);
            m_mouseWheelDelta = 0;
            m_keyCode = -1;
        }
    };

    using Events = std::vector<std::pair<EventType, EventInfo>>;

    // Represent an event type associated to an event details (Example: KeyDown : F11)
    struct Binding
    {
        Binding(const std::string& t_name) : m_name(t_name), c(0), m_details(t_name)
        {}

        void BindEvent(EventType t_type, EventInfo t_info = EventInfo())
        {
            m_events.emplace_back(t_type, t_info);
        }

        Events m_events;
        std::string m_name;
        int c; // Count of events that are "happening".
        EventDetails m_details;
    };

    using Bindings = std::unordered_map<std::string, Binding*>;
    using Callbacks = std::unordered_map<std::string, std::function<void(EventDetails*)>>;

    class EventManager
    {
        public:
        EventManager();
        ~EventManager();

        bool addBinding(Binding *t_binding);
        bool removeBinding(const std::string& t_name);

        void setFocus(const bool& t_focus);

        template<class T>
        bool addCallback(const std::string& t_name, void(T::*t_func)(EventDetails*), T* t_instance)
        {
            auto temp = std::bind(t_func, t_instance, std::placeholders::_1);
            return m_callbacks.emplace(t_name, temp).second;
        }

        void removeCallback(const std::string& t_name)
        {
            m_callbacks.erase(t_name);
        }

        void handleEvent(sf::Event& t_event);
        void update();

        sf::Vector2i getMousePos(sf::RenderWindow* t_window = nullptr) const;

        private:
        void loadBindings();

        Bindings m_bindings;
        Callbacks m_callbacks;
        bool m_hasFocus;
    };
}

#endif // !EVENTMANAGER_H