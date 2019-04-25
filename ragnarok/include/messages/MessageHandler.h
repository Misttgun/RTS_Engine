#pragma once

#include <unordered_map>
#include "Communicator.h"
#include "../ecs/EntityMessages.h"

namespace ragnarok
{
    /**
     * \brief Each possible message type will have its own communicator 
     * that will broadcast the message to all observers
     */
    using Subscribtions = std::unordered_map<EntityMessage, Communicator>;


    /**
     * \brief A kind of message manager, dispatch a message to the right communicator.
     */
    class MessageHandler
    {
        public:
        bool Subscribe(const EntityMessage& t_type, Observer* t_observer)
        {
            return m_communicators[t_type].AddObserver(t_observer);
        }

        bool Unsubscribe(const EntityMessage& t_type, Observer* t_observer)
        {
            return m_communicators[t_type].RemoveObserver(t_observer);
        }

        void Dispatch(const Message& t_msg)
        {
            auto itr = m_communicators.find(static_cast<EntityMessage>(t_msg.m_type));
            
            if(itr == m_communicators.end())
            {
                return;
            }

            itr->second.Broadcast(t_msg);
        }

        private:
        Subscribtions m_communicators;
    };
}