#include "../../game/include/states/RessourceHandler.h"
#include "../../../include/ecs/core/SystemManager.h"

namespace ragnarok
{
	RessourceHandler::RessourceHandler(SystemManager* t_systemMgr) : m_systemManager(t_systemMgr)
	{
		m_gold = 0;
		m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Farming, this);
	}

	RessourceHandler::~RessourceHandler()
	{

	}

	
	/*void RessourceHandler::Update(float t_dT)
	{
		
	}

	void RessourceHandler::HandleEvent(const EntityId & t_entity, const EntityEvent & t_event)
	{
		if (t_event == EntityEvent::Targeted_Entity) {
			// TODO Start chasing the target, but in a different system, not here
		}
	}*/

	int RessourceHandler::GetGold()
	{
		return m_gold;
	}

	bool RessourceHandler::UseGold(int value)
	{
		if (value > m_gold)
			return false;
		m_gold -= value;
		return true;
	}

	/**
	 * Handles Attack_Action messages in order to farm
	 * @param t_message The message to handle
	 */
	void RessourceHandler::Notify(const Message& t_message)
	{
		const auto type = static_cast<EntityMessage>(t_message.m_type);
		if (type == EntityMessage::Farming)
			m_gold += 5;
	}
}
