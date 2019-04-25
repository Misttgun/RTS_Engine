#pragma once

#include "../GUIElement.h"
#include "../specialized/VerticalDropDown.h"

namespace ragnarok
{
    class GUIDropDownMenu : public GUIElement
    {
        public:
        GUIDropDownMenu(const std::string& t_name, GUIInterface* t_owner);
        ~GUIDropDownMenu();

        GUIVerticalDropDown* GetMenu();

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnInterfaceClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void OnFocus();
        void OnDefocus();
        void Setup();
        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        private:
        GUIVerticalDropDown m_dropDown;
        std::string m_fileName;
    };
}