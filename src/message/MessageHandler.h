#pragma once
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <unordered_map>
#include "Communicator.h"
#include "EntityMessages.h"

namespace SExE
{
    using Subscribtions = std::unordered_map<EntityMessage, Communicator>;

    class MessageHandler
    {
        public:
        bool subscribe(const EntityMessage& t_type, Observer* t_observer)
        {
            return m_communicators[t_type].addObserver(t_observer);
        }

        bool unsubscribe(const EntityMessage& t_type, Observer* t_observer)
        {
            return m_communicators[t_type].removeObserver(t_observer);
        }

        void dispatch(const Message& t_msg)
        {
            auto itr = m_communicators.find(static_cast<EntityMessage>(t_msg.m_type));
            if(itr == m_communicators.end())
            {
                return;
            }

            itr->second.broadcast(t_msg);
        }

        private:
        Subscribtions m_communicators;
    };
}

#endif // !MESSAGEHANDLER_H