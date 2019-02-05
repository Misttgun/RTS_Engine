#pragma once
#ifndef _ENGINE_H_INCLUDED_
#define _ENGINE_H_INCLUDED_

#include <memory>

#include "Game.h"
#include "Window.h"

namespace SExE
{
    class Engine
    {
        public : 
            Engine() = default;
            ~Engine() = default;

            // - susbsystem section
            bool init();
            bool terminate();

            // - static getters
            static std::shared_ptr<Game> getGame() { return m_game; };
            static std::shared_ptr<Window> getWindow() { return m_window;  };
        
        private:
            static std::shared_ptr<Game> m_game;
            static std::shared_ptr<Window> m_window;
    };
}

#endif