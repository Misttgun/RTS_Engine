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

		//void Update(float t_dT);
		//void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event);
		void Notify(const Message& t_message);

		int GetGold() const;
		bool UseGold(int value);

	private:
		int m_gold;
		SystemManager* m_systemManager;
	};
}