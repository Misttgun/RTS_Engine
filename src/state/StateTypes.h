#pragma once
#ifndef STATETYPE_H
#define STATETYPE_H

namespace SExE
{
    enum class StateType
    {
        Intro = 1, MainMenu, Game, Loading, Paused, MapEditor
    };
}

#endif // !STATETYPE_H