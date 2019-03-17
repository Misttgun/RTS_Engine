#pragma once

#include <queue>

namespace ragnarok
{
    using EventID = unsigned int;


    /**
     * \brief Represents a queue of all the events. 
     * Useful to proccess lot of events in the good order.
     */
    class EventQueue
    {
        public:
        void AddEvent(const EventID& t_event)
        {
            m_queue.push(t_event);
        }

        bool ProcessEvents(EventID& t_id)
        {
            if(m_queue.empty())
            {
                return false;
            }

            t_id = m_queue.front();
            m_queue.pop();

            return true;
        }

        void Clear()
        {
            while(!m_queue.empty())
            {
                m_queue.pop();
            }
        }

        private:
        std::queue<EventID> m_queue;
    };
}