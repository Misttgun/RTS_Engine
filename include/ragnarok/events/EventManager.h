#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Binding.h"
#include "../states/StateDependent.h"

namespace ragnarok
{
    struct GUIEvent;
    using Bindings = std::unordered_map<std::string, std::unique_ptr<Binding>>;
    // Callback container.
    using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
    // State callback container.
    enum class StateType;
    using Callbacks = std::unordered_map<StateType, CallbackContainer>;

    /* Manage all event and input related functionnality.*/
    class EventManager : public StateDependent
    {
        public:
        EventManager();
        ~EventManager() = default;

        bool AddBinding(std::unique_ptr<Binding> t_binding);
        bool RemoveBinding(const std::string& t_name);

        void ChangeState(const StateType& t_state) override;
        void RemoveState(const StateType& t_state) override;
        void SetFocus(bool t_focus);

        // Add a callback using the a state type as parameter
        template<class T>
        bool AddCallback(const StateType& t_state, const std::string& t_name, void(T::*t_func)(EventDetails*), T* t_instance)
        {
            /* Attempt to add a new callnack container paired to t_state. If successful, itr point to the new element in m_callbacks.
            On the other hand if an element with t_state already exist in m_callbacks itr point to that element instead.*/
            auto itr = m_callbacks.emplace(t_state, CallbackContainer()).first;

            // Bind the function into a callback
            auto temp = std::bind(t_func, t_instance, std::placeholders::_1);

            // Attempt to add the callback inside the callback container and return the second of the pair value (bool)
            return itr->second.emplace(t_name, temp).second;
        }

        // Add a callback using the current state as parameter
        template<class T>
        bool AddCallback(const std::string& t_name, void(T::*t_func)(EventDetails*), T* t_instance)
        {
            return AddCallback<T>(m_currentState, t_name, t_func, t_instance);
        }

        // Remove a callback
        bool RemoveCallback(const StateType& t_state, const std::string& t_name)
        {
            // Find the callback container associated to t_state
            auto itr = m_callbacks.find(t_state);
            if(itr == m_callbacks.end())
            {
                return false;
            }
            
            // Find the callback associated to t_name in the callback container
            const auto itr2 = itr->second.find(t_name);
            if(itr2 == itr->second.end())
            {
                return false;
            }

            // Erase dis mothafucka :D
            itr->second.erase(t_name);

            return true;
        }

        // Handle one time SFML events
        void HandleEvent(sf::Event& t_event);
        
        // Handle one time GUI Events
        void HandleEvent(GUIEvent& t_event);
        
        // Handle continous events
        void Update();

        // Return mouse coordinates relative to the window or not
        sf::Vector2i GetMousePos(sf::RenderWindow* t_wind = nullptr) const
        {
            return (t_wind ? sf::Mouse::getPosition(*t_wind) : sf::Mouse::getPosition());
        }

        private:
        // Load the bindings from file
        void LoadBindings();

        Bindings m_bindings;
        Callbacks m_callbacks;

        bool m_hasFocus;
    };
}