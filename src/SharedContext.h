#pragma once
#ifndef SHAREDCONTEXT_H
#define SHAREDCONTEXT_H

class Window;
class EventManager;

namespace SExE
{
    struct SharedContext
    {
        SharedContext() : m_wind(nullptr), m_eventManager(nullptr)
        {}

        Window* m_wind;
        EventManager* m_eventManager;
    };
}

#endif // !SHAREDCONTEXT_H