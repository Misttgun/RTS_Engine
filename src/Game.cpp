#include "..\include\Game.h"

namespace SExE
{
    Game::Game() : m_window("Demo", sf::Vector2u(800, 600))
    {
        restartClock();
        srand(time(nullptr));
    }

    void Game::update()
    {
        m_window.update();
    }

    void Game::render()
    {
        m_window.beginDraw();
        //m_window.draw(); // Draw shit
        m_window.endDraw();
    }

    Window* Game::getWindow()
    {
        return &m_window;
    }
    sf::Time Game::getElapsed() const
    {
        return m_elapsed;
    }

    void Game::restartClock()
    {
        m_elapsed = m_clock.restart();
    }

}
