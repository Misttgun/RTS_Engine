#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../../ragnarok/states/BaseState.h"
#include "../../ragnarok/events/EventManager.h"

class StateIntro : public ragnarok::BaseState
{
    public:
    StateIntro(ragnarok::StateManager* t_stateManager);
    ~StateIntro();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& t_time) override;
    void Draw() override;

    void Continue(ragnarok::EventDetails* t_details);

    private:
    sf::Sprite m_introSprite;
    sf::Text m_text;
    float m_timePassed;
};