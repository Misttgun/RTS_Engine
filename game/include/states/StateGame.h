#pragma once

#include "states/BaseState.h"
#include "events/EventManager.h"
#include "../../game/include/states/RessourceHandler.h"

class StateGame : public ragnarok::BaseState {
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
	void RightClickAction(ragnarok::EventDetails* t_details);
	void LeftClickAction(ragnarok::EventDetails* t_details);
	void UpdateRessources();

private:
	void UpdateCamera();
	void MovementLogic(const sf::Vector2f& t_delta) const;
	int m_player;
	int m_population;
	int m_max_population;
	sf::Vector2f m_destination;
	ragnarok::RessourceHandler m_RessourceHandler;
};