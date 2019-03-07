#pragma once
#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <queue>

namespace SExE
{
    using EventID = unsigned int;

    class EventQueue
    {
        public:
        void addEvent(const EventID& t_event)
        {
            m_queue.push(t_event);
        }

        bool processEvents(EventID& t_id)
        {
            if(m_queue.empty())
            {
                return false;
            }

            t_id = m_queue.front();
            m_queue.pop();

            return true;
        }

        void clear()
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

#endif // !EVENTQUEUE_H
