#pragma once
#ifndef GAME_H
#define GAME_H

#include <memory>

#include "Window.h"

namespace SExE
{
    // - handle logic of the game
    class Game
    {
        public:
            // - constructor / destructor section
            Game();
            ~Game() = default;

            // - main loop
            void run();

            // - update section
            void update();
            void fixedUpdate();
            void render(const std::shared_ptr<Window> p_window, const sf::Drawable& shape);

            // - input section (moved into Window.h)
            /*
            void handleInput(const std::shared_ptr<Window> p_window);
            void handleKeyboardInput(const sf::Event& event);
            void handleMouseInput(const sf::Event& event);
            */

            // - time section
            sf::Time getElapsed() const;
            void restartClock();

        private:
            sf::Clock m_clock;
            sf::Time m_elapsed;
            const static sf::Time refreshTime;
    };
}

#endif