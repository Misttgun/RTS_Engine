#pragma once
#ifndef ENTITYMESSAGES_H
#define ENTITYMESSAGES_H

namespace SExE
{
    enum class EntityMessage
    {
        Move, Is_Moving, Frame_Change, State_Changed, Direction_Changed, Switch_State, Attack_Action, Dead
    };
}

#endif // !ENTITYMESSAGES_H