#pragma once

#include <SFML/Graphics.hpp>

namespace ragnarok
{
    class StateManager;

    /**
     * \brief Interface for all the states in the game engine.
     */
    class BaseState
    {
        friend class StateManager;

        public:
        BaseState(StateManager* t_stateManager) :m_stateMgr(t_stateManager), m_transparent(false), m_transcendent(false)
        {}

        virtual ~BaseState() = default;

        virtual void OnCreate() = 0;
        virtual void OnDestroy() = 0;

        virtual void Activate() = 0;
        virtual void Deactivate() = 0;

        virtual void Update(const sf::Time& t_time) = 0;
        virtual void Draw() = 0;

        void SetTransparent(const bool& t_transparent)
        {
            m_transparent = t_transparent;
        }

        bool IsTransparent()const
        {
            return m_transparent;
        }

        void SetTranscendent(const bool& t_transcendence)
        {
            m_transcendent = t_transcendence;
        }

        bool IsTranscendent()const
        {
            return m_transcendent;
        }

        sf::View& GetView()
        {
            return m_view;
        }

        StateManager* GetStateManager() const
        {
            return m_stateMgr;
        }

        protected:
        StateManager * m_stateMgr;
        bool m_transparent; // Do we render the previous state behind the current state
        bool m_transcendent; // Do we update the previous state while the current state is active
        sf::View m_view;
    };
}