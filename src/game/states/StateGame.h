#pragma once
#ifndef STATEGAME_H
#define STATEGAME_H

#include "BaseState.h"
#include "EventManager.h"

namespace SExE
{
    class StateGame : public BaseState
    {
        public:
        StateGame(StateManager* t_stateManager);
        ~StateGame();

        void onCreate();
        void onDestroy();

        void activate();
        void deactivate();

        void update(const sf::Time& t_time);
        void draw();

        void mainMenu(EventDetails* t_details);
        void pause(EventDetails* t_details);

        private:
        sf::Texture m_texture;
        sf::Sprite m_sprite;
        sf::Vector2f m_increment;
    };
}

#endif // !STATEGAME_H