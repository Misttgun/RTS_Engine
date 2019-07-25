#pragma once
#include <unordered_map>

#include "Unit.h"

using EntityId = unsigned int;
using UnitContainer = std::unordered_map<EntityId, Unit>;

class UnitManager {
public:
	UnitManager() {};
	~UnitManager()
	{
		m_Units.clear();
	};

	bool AddUnit(const EntityId& t_id, float t_x, float t_y)
	{
		if (!m_Units.emplace(t_id, Unit(t_x, t_y)).second)
			return false;
		return true;
	};

	bool RemoveUnit(const EntityId& t_id)
	{
		const auto itr = m_Units.find(t_id);
		if (itr == m_Units.end())
			return false;
		m_Units.erase(itr);
		return true;
	};

	sf::Vector2f getDestination(const EntityId& t_id)
	{
		const auto itr = m_Units.find(t_id);
		if (itr == m_Units.end())
			return sf::Vector2f(-1, -1);
		return itr->second.getDestination();
	};

	int getNumberUnits()
	{
		return m_Units.size();
	};

	void setDestination(const EntityId& t_id, const sf::Vector2f t_direction)
	{
		const auto itr = m_Units.find(t_id);
		itr->second.setDestination(t_direction);
	}

	void setDestination(const EntityId& t_id, float t_x, float t_y)
	{
		const auto itr = m_Units.find(t_id);
		itr->second.setDestination(t_x, t_y);
	}

private:
	UnitContainer m_Units;
};