#pragma once

#include "../GUIElement.h"

namespace ragnarok
{
    class GUILabel : public GUIElement
    {
        public:
        GUILabel(const std::string& t_name, GUIInterface* t_owner);
        ~GUILabel();

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);
    };
}