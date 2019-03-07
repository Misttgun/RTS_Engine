#pragma once
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include <unordered_map>
#include <assert.h>
#include <functional>
#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Bindings.h"
#include "Utilities.h"
#include "StateDependent.h"

namespace SExE
{
    using Bindings = std::unordered_map<std::string, std::unique_ptr<Binding>>;
    using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
    
    enum class StateType;
    
    using Callbacks = std::unordered_map<StateType, CallbackContainer>;

    class EventManager : public StateDependent
    {
        public:
        EventManager();
        ~EventManager();

        bool addBinding(std::unique_ptr<Binding> t_binding);
        bool removeBinding(const std::string& t_name);

        void changeState(const StateType& t_state);
        void removeState(const StateType& t_state);
        void setFocus(bool t_focus);

        // Needs to be defined in the header!
        template<class T>
        bool addCallback(const StateType& t_state, const std::string& t_name, void(T::*t_func)(EventDetails*), T* t_instance)
        {
            auto itr = m_callbacks.emplace(t_state, CallbackContainer()).first;
            auto temp = std::bind(t_func, t_instance, std::placeholders::_1);
            return itr->second.emplace(t_name, temp).second;
        }

        template<class T>
        bool addCallback(const std::string& t_name, void(T::*t_func)(EventDetails*), T* t_instance)
        {
            return addCallback<T>(m_currentState, t_name, t_func, t_instance);
        }

        bool removeCallback(const StateType& t_state, const std::string& t_name)
        {
            auto itr = m_callbacks.find(t_state);
            if(itr == m_callbacks.end())
            {
                return false;
            }

            auto itr2 = itr->second.find(t_name);
            if(itr2 == itr->second.end())
            {
                return false;
            }

            itr->second.erase(t_name);
            return true;
        }

        void handleEvent(sf::Event& t_event);
        void handleEvent(GUIEvent& t_event);
        void update();

        sf::Vector2i getMousePos(sf::RenderWindow* t_wind = nullptr) const
        {
            return (t_wind ? sf::Mouse::getPosition(*t_wind) : sf::Mouse::getPosition());
        }

        private:
        void loadBindings();

        Bindings m_bindings;
        Callbacks m_callbacks;

        bool m_hasFocus;
    };
}

#endif // !EVENTMANAGER_H