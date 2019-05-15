#pragma once

#include "states/BaseState.h"
#include "events/EventManager.h"

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
	void UnitSpawn(ragnarok::EventDetails* t_details);
	void UpdateRessources();

private:
	void UpdateCamera();
    void MovementLogic(const sf::Vector2f& t_delta) const;
	int m_player;
    sf::Vector2f m_destination;
};