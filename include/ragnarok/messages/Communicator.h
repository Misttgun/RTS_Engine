#pragma once

#include <vector>
#include <algorithm>
#include "Observer.h"

namespace ragnarok
{
    using ObserverContainer = std::vector<Observer*>; // Non-memory-owning pointers.


    /**
     * \brief A kind of observer manager, handles all the logic for all observers.
     */
    class Communicator
    {
        public:
        virtual ~Communicator()
        {
            m_observers.clear();
        }

        bool AddObserver(Observer* t_observer)
        {
            if(HasObserver(t_observer))
            {
                return false;
            }

            m_observers.emplace_back(t_observer);

            return true;
        }

        bool RemoveObserver(Observer* t_observer)
        {
            const auto observer = std::find_if(m_observers.begin(), m_observers.end(), [&t_observer](Observer* o) { return o == t_observer; });
            
            if(observer == m_observers.end())
            {
                return false;
            }

            m_observers.erase(observer);

            return true;
        }

        bool HasObserver(const Observer* t_observer) const
        {
            return (std::find_if(m_observers.begin(), m_observers.end(), [&t_observer](Observer* o) { return o == t_observer; }) != m_observers.end());
        }

        void Broadcast(const Message& t_msg)
        {
            for(auto& itr : m_observers)
            {
                itr->Notify(t_msg);
            }
        }

        private:
        ObserverContainer m_observers;
    };
}