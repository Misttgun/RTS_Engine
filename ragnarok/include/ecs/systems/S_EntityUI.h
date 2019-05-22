#pragma once

#include "../core/S_Base.h"
#include "../../window/Window.h"

namespace ragnarok
{
    /**
    * \brief Entity UI system, take care of rendering all the UI of an entity
    */
    class S_EntityUI : public S_Base
    {
        public:
        S_EntityUI(SystemManager* t_systemMgr);
        ~S_EntityUI() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;
        void Render(Window* t_wind);

        private:
        sf::RectangleShape m_healthBar;
        sf::Vector2f m_healthBarSize;
        sf::RectangleShape m_selectionRect;
    };
}