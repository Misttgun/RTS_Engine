#include "../../game/include/states/RessourceHandler.h"
#include "../../../include/ecs/core/SystemManager.h"

namespace ragnarok
{
	RessourceHandler::RessourceHandler(SystemManager* t_systemMgr) : m_systemManager(t_systemMgr)
	{
		m_gold = 50;
		m_population = 0;
		m_maxPopulation = 20;
		m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Farming, this);
		m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Dead, this);
	}

	RessourceHandler::~RessourceHandler() = default;


	int RessourceHandler::GetGold() const
    {
		return m_gold;
	}

	int RessourceHandler::GetPopulation() const
	{
		return m_population;
	}

	int RessourceHandler::GetMaxPopulation() const
	{
		return m_maxPopulation;
	}

	bool RessourceHandler::canSpawn() const
	{
		return m_population < m_maxPopulation;
	}

	void RessourceHandler::spawnOneUnit()
	{
		m_population++;
	}

	bool RessourceHandler::UseGold(int value)
	{
		if (value > m_gold)
			return false;
		m_gold -= value;
		return true;
	}

	void RessourceHandler::Notify(const Message& t_message)
	{
		const auto type = static_cast<EntityMessage>(t_message.m_type);
		if (type == EntityMessage::Farming)
			m_gold++;
		else if (type == EntityMessage::Dead)
			m_population--;
	}
}
