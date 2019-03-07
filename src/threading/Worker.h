#pragma once
#ifndef WORKER_H
#define WORKER_H

#include <SFML/System/Thread.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Mutex.hpp>

namespace SExE
{
    class Worker
    {
        public:
        Worker() : m_thread(&Worker::Work, this), m_done(false), m_started(false)
        {}

        virtual void onAdd()
        {}

        virtual void onRemove()
        {}

        void begin()
        {
            sf::Lock lock(m_mutex);
            if(m_done || m_started)
            {
                return;
            }
            m_started = true;
            m_thread.launch();
        }

        bool isDone()
        {
            sf::Lock lock(m_mutex); return m_done;
        }

        bool hasStarted()
        {
            sf::Lock lock(m_mutex); return m_started;
        }

        void resetWorker()
        {
            sf::Lock lock(m_mutex); m_done = false; m_started = false;
        }

        protected:
        void done()
        {
            m_done = true;
        }

        virtual void Work() = 0;
        sf::Thread m_thread;
        sf::Mutex m_mutex;
        bool m_done;
        bool m_started;
    };
}

#endif // !WORKER_H