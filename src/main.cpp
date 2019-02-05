#include <iostream>

#include "Engine.h"

int main()
{   
    SExE::Engine engine;

    if (!engine.init())
    {
        std::cout << "FAILED TO INIT THE ENGINE\n";
        return EXIT_FAILURE;
    }

    SExE::Engine::getGame()->run();

    if (!engine.terminate())
    {
        std::cout << "FAILED TO TERMINATE THE ENGINE\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}