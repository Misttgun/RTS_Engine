#include "../include/Game.h"

#ifdef __linux
#include <X11/Xlib.h>
#endif

int main()
{
    {
#ifdef __linux
        XInitThreads();
#endif

        Game game;
        while(!game.GetWindow()->IsDone())
        {
            game.Update();
            game.Render();
            game.LateUpdate();
        }
    }

    return EXIT_SUCCESS;
}