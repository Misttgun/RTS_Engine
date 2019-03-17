#pragma once

#include "../core/S_Base.h"
#include "../../window/Window.h"
#include "../../types/Directions.h"

namespace ragnarok
{
    class C_Drawable;


    /**
     * \brief Renderer system, take care of rendering a C_Drawable
     */
    class S_Renderer : public S_Base
    {
        public:
        S_Renderer(SystemManager* t_systemMgr);
        ~S_Renderer() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;
        void Render(Window* t_wind, unsigned int t_layer);

        private:

        /**
         * \brief Informs a spritesheet of a change in direction in order to reflect the change visually.
         * \param t_entity the entity that changed direction
         * \param t_dir the new direction
         */
        void SetSheetDirection(const EntityId& t_entity, const Direction& t_dir) const;

        /**
         * \brief Sort all entities along the y axis and by elevation on the map
         */
        void SortDrawables();

        /**
         * \brief Find a drawable registered in m_drawableTypes
         * \param t_entity the entity we want to render
         * \return a pointer to the drawable component of the entity
         */
        C_Drawable* GetDrawableFromType(const EntityId& t_entity) const;

        Bitmask m_drawableTypes; // Keeps track of all possible drawable component types
    };
}