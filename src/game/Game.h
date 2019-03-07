#pragma once
#ifndef GAME_H
#define GAME_H

#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include <iostream>

namespace SExE
{
    class Game
    {
        public:
        Game();
        ~Game() = default;

        void update();
        void render();
        void lateUpdate();

        Window* getWindow();
        sf::Time getElapsed() const;

        private:
        void restartClock();
        void setUpClasses();
        void setUpECS();
        void setUpStates();

        SharedContext m_context;
        Window m_window;
        std::unique_ptr<StateManager> m_stateManager;
        sf::Clock m_clock;
        sf::Time m_elapsed;
    };
}

#endif // !GAME_H