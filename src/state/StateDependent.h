#pragma once
#ifndef STATEDEPENDENT_H
#define STATEDEPENDENT_H

namespace SExE
{
    enum class StateType;
    class StateManager;

    class StateDependent
    {
        public:
        StateDependent() : m_currentState((StateType)0)
        {}

        virtual ~StateDependent()
        {}

        virtual void createState(const StateType& t_state)
        {}

        virtual void changeState(const StateType& t_state) = 0;
        virtual void removeState(const StateType& t_state) = 0;

        StateType getCurrentState()const
        {
            return m_currentState;
        }

        protected:
        void setState(const StateType& t_state)
        {
            m_currentState = t_state;
        }

        StateType m_currentState;
    };
}

#endif // !STATEDEPENDENT_H