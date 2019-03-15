#pragma once

#include <vector>
#include "EventDetails.h"
#include "EventTypes.h"
#include "EventInfo.h"

namespace ragnarok
{
    using Events = std::vector<std::pair<EventType, EventInfo>>;

    /* Allow events to be grouped together to form complex requirements.
    For example we can have a behind that needs multiple keys to be pressed at the same time (Ctrl + C).
    With this struct and the others include in the file we can achieve that easily.*/
    struct Binding
    {
        Binding(const std::string& t_name) : m_name(t_name), c(0), m_details(t_name)
        {}

        void BindEvent(EventType t_type, EventInfo t_info = EventInfo())
        {
            m_events.emplace_back(t_type, std::move(t_info));
        }

        Events m_events; // All the <EventType, EventInfo> pairs associated to the binding
        std::string m_name; // Name of the binding
        int c; // Count of events that are "happening".

        EventDetails m_details; // The event detail of the binding
    };
}