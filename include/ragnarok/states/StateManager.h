#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <SFML/System/Time.hpp>
#include "StateTypes.h"
#include "../SharedContext.h"

namespace ragnarok
{
    class BaseState;
    class StateDependent;
    class StateLoading;

    // State container.
    using StateContainer = std::vector<std::pair<StateType, std::unique_ptr<BaseState>>>;
    // Type container.
    using TypeContainer = std::vector<StateType>;
    // State factory.
    using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;
    using StateDependents = std::vector< StateDependent* >;


    /**
     * \brief Manages all the states in the game.
     */
    class StateManager
    {
        public:
        StateManager(SharedContext* t_shared);
        ~StateManager();

        void Update(const sf::Time& t_time);
        void Draw();

        /**
         * \brief Takes care of removing all the states in m_toRemove.
         */
        void ProcessRequests();

        SharedContext* GetContext() const;
        bool HasState(const StateType& t_type) const;
        StateType GetCurrentStateType() const;
        StateType GetNextToLast() const;
        void SwitchTo(const StateType& t_type);
        void Remove(const StateType& t_type);

        template<class T>
        T* GetState(const StateType& t_type)
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
        void RegisterState(const StateType& t_type)
        {
            m_stateFactory[t_type] = [this]() -> BaseState* { return new T(this); };
        }

        void AddDependent(StateDependent* t_dependent);
        void RemoveDependent(StateDependent* t_dependent);

        private:
        void SetGlobalState(const StateType& t_type);
        void CreateState(const StateType& t_type);
        void RemoveState(const StateType& t_type);

        SharedContext* m_shared;
        StateLoading* m_loading;
        StateContainer m_states;
        StateDependents m_dependents;
        TypeContainer m_toRemove;
        StateFactory m_stateFactory;
    };
}