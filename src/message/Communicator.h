#pragma once
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <vector>
#include <algorithm>
#include "Observer.h"

namespace SExE
{
    using ObserverContainer = std::vector<Observer*>; // Non-memory-owning pointers.

    class Communicator
    {
        public:
        virtual ~Communicator()
        {
            m_observers.clear();
        }

        bool addObserver(Observer* t_observer)
        {
            if(hasObserver(t_observer))
            {
                return false;
            }

            m_observers.emplace_back(t_observer);
            return true;
        }

        bool removeObserver(Observer* t_observer)
        {
            auto observer = std::find_if(m_observers.begin(), m_observers.end(), [&t_observer](Observer* o) {return o == t_observer; });

            if(observer == m_observers.end())
            {
                return false;
            }

            m_observers.erase(observer);
            return true;
        }

        bool hasObserver(const Observer* t_observer) const
        {
            return (std::find_if(m_observers.begin(), m_observers.end(), [&t_observer](Observer* o) {return o == t_observer;}) != m_observers.end());
        }

        void broadcast(const Message& t_msg)
        {
            for(auto& itr : m_observers)
            {
                itr->notify(t_msg);
            }
        }

        private:
        ObserverContainer m_observers;
    };
}

#endif // !COMMUNICATOR_H