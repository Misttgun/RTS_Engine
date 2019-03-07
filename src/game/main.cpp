#include "Game.h"

int main()
{
    SExE::Game game;
    while(!game.getWindow()->isDone())
    {
        game.update();
        game.render();
        game.lateUpdate();
    }
}