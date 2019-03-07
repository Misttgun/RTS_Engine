#pragma once
#ifndef OBSERVER_H
#define OBSERVER_H

#include "Message.h"

namespace SExE
{
    class Observer
    {
        public:
        virtual ~Observer()
        {}

        virtual void notify(const Message& t_message) = 0;
    };
}

#endif // !OBSERVER_H