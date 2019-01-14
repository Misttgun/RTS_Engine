#include "..\include\Game.h"

Engine::Game::Game()
	: m_window("Demo", sf::Vector2u(800, 600))
{
	restartClock();
	srand(time(nullptr));
}

void Engine::Game::handleInput()
{
	// Do input handling
}

void Engine::Game::update()
{
	m_window.update();
}

void Engine::Game::render()
{
	m_window.beginDraw();
	//m_window.draw(); // Draw shit
	m_window.endDraw();
}

void Engine::Game::restartClock()
{
	m_elapsed = m_clock.restart();
}
