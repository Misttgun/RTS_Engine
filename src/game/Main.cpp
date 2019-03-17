#include "game/Game.h"

void main()
{
    {
        Game game;
        while(!game.GetWindow()->IsDone())
        {
            game.Update();
            game.Render();
            game.LateUpdate();
        }
    }
}