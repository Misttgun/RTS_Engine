#pragma once
#ifndef STATEPAUSED_H
#define STATEPAUSED_H

#include "BaseState.h"
#include "EventManager.h"

namespace SExE
{
    class StatePaused : public BaseState
    {
        public:
        StatePaused(StateManager* t_stateManager);
        ~StatePaused();

        void onCreate();
        void onDestroy();

        void activate();
        void deactivate();

        void update(const sf::Time& t_time);
        void draw();

        void unpause(EventDetails* t_details);

        private:
        sf::Font m_font;
        sf::Text m_text;
        sf::RectangleShape m_rect;
    };
}

#endif // !STATEPAUSED_H