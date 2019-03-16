#pragma once

#include "../../ragnarok/states/BaseState.h"
#include "../../ragnarok/map/Map.h"
#include "../../ragnarok/events/EventManager.h"

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
	void PlayerMove(ragnarok::EventDetails* t_details);

private:
	void UpdateCamera();
	int m_player;
};