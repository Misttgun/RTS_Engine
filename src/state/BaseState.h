#pragma once
#ifndef BASESTATE_H
#define BASESTATE_H

#include <SFML/Graphics.hpp>

namespace SExE
{
    class StateManager;

    class BaseState
    {
        friend class StateManager;

        public:
        BaseState(StateManager* t_stateManager) : m_stateMgr(t_stateManager), m_transparent(false), m_transcendent(false)
        {}

        virtual ~BaseState()
        {}

        virtual void onCreate() = 0;
        virtual void onDestroy() = 0;

        virtual void activate() = 0;
        virtual void deactivate() = 0;

        virtual void update(const sf::Time& t_time) = 0;
        virtual void draw() = 0;

        void setTransparent(const bool& t_transparent)
        {
            m_transparent = t_transparent;
        }

        bool isTransparent() const
        {
            return m_transparent;
        }

        void setTranscendent(const bool& t_transcendence)
        {
            m_transcendent = t_transcendence;
        }

        bool isTranscendent() const
        {
            return m_transcendent;
        }

        sf::View& getView()
        {
            return m_view;
        }

        StateManager* getStateManager()
        {
            return m_stateMgr;
        }

        protected:
        StateManager* m_stateMgr;
        bool m_transparent;
        bool m_transcendent;
        sf::View m_view;
    };
}

#endif // !BASESTATE_H