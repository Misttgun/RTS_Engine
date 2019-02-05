#include "..\include\Game.h"
#include "Engine.h"
#include <iostream>

namespace SExE
{
    // - init static variables
    const sf::Time Game::refreshTime = sf::seconds(1.0f / 60.f); // 60 FPS frame rate

    // - constructor / destructor section
    Game::Game()
    {
        restartClock();
        srand(time(nullptr));
    }

    // - main loop
    void Game::run()
    {
        int x = 100;
        int modifier = 1;

        auto window = Engine::getWindow();

        // - for frame rate test
        sf::CircleShape shape(30.0f);
        shape.setPosition(100.0f, 100.0f);
        shape.setFillColor(sf::Color::Green);

        sf::Time timeSinceLastUpdate = sf::Time::Zero;

        while (!window->isDone())
        {
            restartClock();
            timeSinceLastUpdate += getElapsed();

            while (timeSinceLastUpdate > refreshTime)
            {
                // - test to check framerate

                x += modifier;

                if (x == 130)
                    modifier = -1;
                if (x == 70)
                    modifier = 1;

                shape.setPosition(x, 100.0f);


                timeSinceLastUpdate -= refreshTime;
                window->handleInput();
                update();
                fixedUpdate();
                render(window, shape);
            }
        }
    }

    // - update section
    void Game::update()
    {

    }

    void Game::fixedUpdate()
    {
        
    }

    void Game::render(const std::shared_ptr<Window> p_window, const sf::Drawable& shape)
    {
        p_window->beginDraw();
        p_window->draw(shape);
        p_window->endDraw();
    }

    // - input section (moved into Window.h)
    /*
    void Game::handleInput(const std::shared_ptr<Window> p_window)
    {
        sf::Event event;

        while (p_window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                p_window->close();
              
            else if (event.type == sf::Event::KeyPressed)
                handleKeyboardInput(event);

            else if (event.type == sf::Event::MouseButtonPressed)
                handleMouseInput(event);
        }
    }

    void Game::handleKeyboardInput(const sf::Event& event)
    {
        // - TODO : loop over key pressed and fire events

    }

    void Game::handleMouseInput(const sf::Event& event)
    {
        // - TODO : loop over button pressed and fire events
    }
    */

    // - time section
    sf::Time Game::getElapsed() const
    {
        return m_elapsed;
    }

    void Game::restartClock()
    {
        m_elapsed = m_clock.restart();
    }
}