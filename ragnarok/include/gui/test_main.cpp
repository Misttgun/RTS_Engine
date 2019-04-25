//
// Created by florian on 06/03/19.
//

#include <game/states/StateButtonTest.h>
#include <ragnarok/window/Window.h>
#include <ragnarok/states/StateManager.h>
#include <ragnarok/resources/FontManager.h>
#include <ragnarok/resources/TextureManager.h>
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
    context.m_textureManager = new ragnarok::TextureManager();

    ragnarok::StateManager stateManager(&context);
    ragnarok::GUIManager guiManager(&eventManager, &context);

    context.m_guiManager = &guiManager;

    stateManager.AddDependent(context.m_eventManager);
    stateManager.AddDependent(&guiManager);

    //stateManager.registerState<StateIntro>(StateType::Intro);
    stateManager.RegisterState<StateButtonTest>(ragnarok::StateType::ButtonTest);
    //stateManager.registerState<StateGame>(StateType::Game);
    //stateManager.registerState<StatePaused>(StateType::Paused);

    stateManager.SwitchTo(ragnarok::StateType::ButtonTest);

    while (!window.IsDone()) {
        // Game::Update
        window.Update();
        stateManager.Update(elapsed);
        guiManager.Update(elapsed.asSeconds());
        ragnarok::GUIEvent guiEvent;
        while (guiManager.PollEvent(guiEvent))
        {
            window.GetEventManager()->HandleEvent(guiEvent);
        }

        // Game::Render
        window.BeginDraw();
        stateManager.Draw();
        guiManager.Render(window.GetRenderWindow());
        window.EndDraw();

        // Game::LateUpdate
        stateManager.ProcessRequests();
        elapsed = clock.restart();
    }


    return 0;
}

