#pragma once

#include "../../window/Window.h"
#include "../core/C_Base.h"

namespace ragnarok
{

    /**
     * \brief Interface for all drawable components.
     */
    class C_Drawable : public C_Base
    {
        public:
        C_Drawable(const Component& t_type) : C_Base(t_type)
        {}

        virtual ~C_Drawable() = default;

        virtual void UpdatePosition(const sf::Vector2f& t_vec) = 0;
        virtual sf::Vector2u GetSize() const = 0;
        virtual void Draw(Window* t_wind) = 0;
    };
}