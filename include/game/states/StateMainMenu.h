#pragma once

#include "../../ragnarok/states/BaseState.h"
#include "../../ragnarok/events/EventManager.h"

class StateMainMenu : public ragnarok::BaseState
{
    public:
    StateMainMenu(ragnarok::StateManager* t_stateManager);
    ~StateMainMenu();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& t_time) override;
    void Draw() override;

    void Play(ragnarok::EventDetails* t_details);
    void Quit(ragnarok::EventDetails* t_details);
};