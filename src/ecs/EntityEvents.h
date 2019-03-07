#pragma once
#ifndef ENTITYEVENTS_H
#define ENTITYEVENTS_H

namespace SExE
{
    enum class EntityEvent
    {
        Spawned, Despawned, Colliding_X, Colliding_Y, Moving_Left, Moving_Right, Moving_Up, Moving_Down,
        Elevation_Change, Became_Idle, Began_Moving
    };
}

#endif // !ENTITYEVENTS_H