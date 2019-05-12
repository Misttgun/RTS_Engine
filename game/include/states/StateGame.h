#pragma once

#include "states/BaseState.h"
#include "events/EventManager.h"
#include "interaction/GameClickManager.h"

class StateGame : public ragnarok::BaseState{
public:
	StateGame(ragnarok::StateManager* t_stateManager);
	~StateGame();

	void OnCreate() override;
	void OnDestroy() override;

	void Activate() override;
	void Deactivate() override;

	void Update(const sf::Time& t_time) override;
	void Draw() override;

	void MainMenu(ragnarok::EventDetails* t_details);
	void PlayerMove(int t_entityID, sf::Vector2f t_position);

private:
	void UpdateCamera();
    void MovementLogic(const sf::Vector2f& t_delta) const;
	int m_player;
    sf::Vector2f m_destination;

    ragnarok::GameClickManager m_gameClickManager;
};