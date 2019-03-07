#pragma once
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <SFML/System/Time.hpp>
#include "StateTypes.h"
#include "SharedContext.h"

namespace SExE
{
    class BaseState;
    class StateDependent;
    class StateLoading;

    using StateContainer = std::vector<std::pair<StateType, std::unique_ptr<BaseState>>>;

    using TypeContainer = std::vector<StateType>;

    using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;
    using StateDependents = std::vector<StateDependent*>;

    class StateManager
    {
        public:
        StateManager(SharedContext* t_shared);
        ~StateManager();

        void update(const sf::Time& t_time);
        void draw();
        
        void processRequests();

        SharedContext* getContext();
        bool hasState(const StateType& t_type) const;
        StateType getCurrentStateType() const;
        StateType getNextToLast() const;
        void switchTo(const StateType& t_type);
        void remove(const StateType& t_type);

        template<class T>
        T* getState(const StateType& t_type)
        {
            for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
            {
                if(itr->first != t_type)
                {
                    continue;
                }

                return static_cast<T*>(itr->second.get());
            }
            return nullptr;
        }

        template<class T>
        void registerState(const StateType& t_type)
        {
            m_stateFactory[t_type] = [this]() -> BaseState*
            {
                return new T(this);
            };
        }

        void addDependent(StateDependent* t_dependent);
        void removeDependent(StateDependent* t_dependent);

        private:
        void setGlobalState(const StateType& t_type);
        void createState(const StateType& t_type);
        void removeState(const StateType& t_type);

        SharedContext* m_shared;
        StateLoading* m_loading;
        StateContainer m_states;
        StateDependents m_dependents;
        TypeContainer m_toRemove;
        StateFactory m_stateFactory;
    };
}

#endif // !STATEMANAGER_H