#pragma once
#ifndef BINDINGS_H
#define BINDINGS_H

#include <vector>
#include "EventTypes.h"
#include "EventInfo.h"
#include "EventDetails.h"

namespace SExE
{
    using Events = std::vector<std::pair<EventType, EventInfo>>;

    struct Binding
    {
        Binding(const std::string& t_name) : m_name(t_name), m_details(t_name), c(0)
        {}

        void bindEvent(EventType t_type, EventInfo t_info = EventInfo())
        {
            m_events.emplace_back(t_type, std::move(t_info));
        }

        Events m_events;
        std::string m_name;
        int c; // Count of events that are "happening".

        EventDetails m_details;
    };
}

#endif // !BINDINGS_H