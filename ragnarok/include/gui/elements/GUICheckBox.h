#pragma once

#include "../GUIElement.h"

namespace ragnarok
{
    class GUICheckBox : public GUIElement
    {
        public:
        GUICheckBox(const std::string& t_name, GUIInterface* t_owner);
        ~GUICheckBox();

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        private:

    };
}