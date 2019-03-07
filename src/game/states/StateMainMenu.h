#pragma once
#ifndef STATEMAINMENU_H
#define STATEMAINMENU_H

#include "BaseState.h"
#include "EventManager.h"

namespace SExE
{
    class StateMainMenu : public BaseState
    {
        public:
        StateMainMenu(StateManager* t_stateManager);
        ~StateMainMenu();

        void onCreate();
        void onDestroy();

        void activate();
        void deactivate();

        void update(const sf::Time& t_time);
        void draw();

        void mouseClick(EventDetails* t_details);

        private:
        sf::Font m_font;
        sf::Text m_text;

        sf::Vector2f m_buttonSize;
        sf::Vector2f m_buttonPos;
        unsigned int m_buttonPadding;

        sf::RectangleShape m_rects[3];
        sf::Text m_labels[3];
    };
}

#endif // !STATEMAINMENU_H