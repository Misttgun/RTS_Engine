#pragma once
#ifndef ECSTYPES_H
#define ECSTYPES_H

namespace Engine 
{
	using ComponentType = unsigned int;
	#define N_COMPONENT_TYPES 32;

	enum class Component 
	{
		Position = 0, SpriteSheet, State, Movable, Controller, Collidable
	};

	enum class System 
	{
		Renderer = 0, Movement, Collision, Control, State, SheetAnimation
	};
}

#endif // !ECSTYPES_H