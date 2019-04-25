#pragma once
#ifndef GAME_H
#define GAME_H

#include "Window.h"

namespace SExE
{
    class Game
    {
        public:
        Game();
        ~Game() = default;

        void update();
        void render();

        Window* getWindow();
        sf::Time getElapsed() const;

        void restartClock();

        private:
        Window m_window;
        sf::Clock m_clock;
        sf::Time m_elapsed;
    };
}

#endif // !GAME_H