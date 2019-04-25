#pragma once

#include "Message.h"

namespace ragnarok
{
    
    /**
     * \brief Allow its users to pick and choose what they will be notified of.
     */
    class Observer
    {
        public:
        virtual ~Observer() = default;

        virtual void Notify(const Message& t_message) = 0;
    };
}