#pragma once

#include "messages/Observer.h"

namespace ragnarok
{

	class SystemManager;

	class RessourceHandler : public Observer
	{
	public:
		RessourceHandler(SystemManager* t_systemMgr);
		~RessourceHandler();

		void Notify(const Message& t_message);

		int GetGold() const;
		int GetPopulation() const;
		int GetMaxPopulation() const;
		bool UseGold(int value);
		bool canSpawn() const;
		void spawnOneUnit();

	private:
		int m_gold;
		int m_population;
		int m_maxPopulation;
		SystemManager* m_systemManager;
	};
}