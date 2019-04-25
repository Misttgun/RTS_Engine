#pragma once

namespace ragnarok
{
    enum class StateType;
    class StateManager;

    /**
     * \brief Interface for classes that deals with sounds, UI, entity management and need to support different states.
     */
    class StateDependent
    {
        public:
        StateDependent() : m_currentState(static_cast<StateType>(0))
        {}

        virtual ~StateDependent() = default;

        virtual void CreateState(const StateType& t_state)
        {}

        virtual void ChangeState(const StateType& t_state) = 0;
        virtual void RemoveState(const StateType& t_state) = 0;

        StateType GetCurrentState()const
        {
            return m_currentState;
        }

        protected:
        void SetState(const StateType& t_state)
        {
            m_currentState = t_state;
        }

        StateType m_currentState;
    };
}