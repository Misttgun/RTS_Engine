#pragma once
#ifndef GAME_H
#define GAME_H

#include "Window.h"

namespace Engine 
{
	class Game 
	{
	public:
		Game();
		~Game() = default;

		void handleInput();
		void update();
		void render();

		Window* getWindow() { return &m_window; }
		sf::Time getElapsed() const { return m_elapsed; }

		void restartClock();

	private:

		Window m_window;
		sf::Clock m_clock;
		sf::Time m_elapsed;
	};
}

#endif // !GAME_H