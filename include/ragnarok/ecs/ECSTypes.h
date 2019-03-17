#pragma once

namespace ragnarok
{
    using ComponentType = unsigned int;

    /**
     * \brief Used to define component type in .entity files
     */
    enum class Component
    {
        Position = 0, SpriteSheet, State, Movable, Controller, Collidable, SoundEmitter, SoundListener, COUNT
    };

    enum class System
    {
        Renderer = 0, Movement, Collision, Control, State, SheetAnimation, Sound, COUNT
    };
}