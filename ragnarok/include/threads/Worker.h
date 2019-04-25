#pragma once

#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>

namespace ragnarok
{
    /* Interface any thread worker can use.*/
    class Worker
    {
        public:
        Worker() : m_thread(&Worker::Work, this), m_done(false), m_started(false)
        {}

        virtual void OnAdd()
        {}

        virtual void OnRemove()
        {}

        void Begin()
        {
            sf::Lock lock(m_mutex);

            if(m_done || m_started)
            {
                return;
            }

            m_started = true;
            m_thread.launch();
        }

        bool IsDone()
        {
            sf::Lock lock(m_mutex); 
            return m_done;
        }

        bool HasStarted()
        {
            sf::Lock lock(m_mutex); 
            return m_started;
        }

        void ResetWorker()
        {
            sf::Lock lock(m_mutex);
            m_done = false; 
            m_started = false;
        }

        protected:
        /* Tell outside code that the process is finished.
        It is the only way to tell that a thread is actually finished.*/
        void Done()
        {
            m_done = true;
        }

        virtual void Work() = 0;

        sf::Thread m_thread;
        sf::Mutex m_mutex; // Usage of mutex to protect data from multiple threads access
        bool m_done;
        bool m_started;
    };
}
