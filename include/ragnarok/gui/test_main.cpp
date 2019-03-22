//
// Created by florian on 06/03/19.
//

#include <game/states/StateMainMenu.h>
#include <ragnarok/window/Window.h>
#include <ragnarok/states/StateManager.h>
#include <ragnarok/resources/FontManager.h>
#include "../SharedContext.h"
#include "GUIManager.h"

int main() {
    sf::Clock clock;
    sf::Time elapsed;

    ragnarok::EventManager eventManager;
    ragnarok::SharedContext context;
    ragnarok::Window window;

    context.m_wind = &window;
    context.m_eventManager = window.GetEventManager();
    context.m_fontManager = new ragnarok::FontManager();

    ragnarok::StateManager stateManager(&context);
    ragnarok::GUIManager guiManager(&eventManager, &context);

    stateManager.AddDependent(context.m_eventManager);
    stateManager.AddDependent(&guiManager);
    //stateManager.registerState<StateIntro>(StateType::Intro);
    stateManager.RegisterState<StateMainMenu>(ragnarok::StateType::MainMenu);
    //stateManager.registerState<StateGame>(StateType::Game);
    //stateManager.registerState<StatePaused>(StateType::Paused);

    while (!window.IsDone()) {
        window.Update();
        stateManager.Update(elapsed);

        window.BeginDraw();
        stateManager.Draw();
        window.EndDraw();

        stateManager.ProcessRequests();
        elapsed = clock.restart();
    }


    return 0;
}

