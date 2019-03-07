#pragma once
#ifndef ECSTYPES_H
#define ECSTYPES_H

namespace SExE
{
    using ComponentType = unsigned int;

    enum class Component
    {
        Position = 0, SpriteSheet, State, Movable, Controller, Collidable, SoundEmitter, SoundListener, COUNT
    };

    enum class System
    {
        Renderer = 0, Movement, Collision, Control, State, SheetAnimation, Sound, COUNT
    };
}

#endif // !ECSTYPES_H