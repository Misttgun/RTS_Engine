#include <SFML/Graphics.hpp>

#include "Engine.h"


namespace SExE
{
    // - init static variables
    std::shared_ptr<Game> Engine::m_game = nullptr;
    std::shared_ptr<Window> Engine::m_window = nullptr;

    // - subsystem section
    bool Engine::init()
    {
        // - init the game instance
        m_window = std::make_shared<Window>(Window("SFML works!", sf::Vector2u(200, 200)));
        m_game = std::make_shared<Game>();

        return true;
    }

    bool Engine::terminate()
    {
        // terminate all subsystems
        return true;
    }
}