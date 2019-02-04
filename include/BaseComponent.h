#pragma once
#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

#include "ECS_Types.h"
#include <sstream>

namespace Engine 
{
	class BaseComponent
	{
	public:
		BaseComponent(const Component& t_type) : m_type(t_type) {}
		virtual ~BaseComponent() {}

		Component getType() const { return m_type; }

		friend std::stringstream& operator >>(std::stringstream& t_stream, BaseComponent& t_base)
		{
			t_base.readIn(t_stream);
			return t_stream;
		}

		virtual void readIn(std::stringstream& t_stream) = 0;

	protected:
		Component m_type;
	};
}

#endif // !BASECOMPONENT_H
