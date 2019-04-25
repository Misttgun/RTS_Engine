#pragma once

#include "../GUIElement.h"

namespace ragnarok
{
    enum class SliderType
    {
        Horizontal, Vertical
    };

    class GUIScrollbar : public GUIElement
    {
        public:
        GUIScrollbar(const std::string& t_name, GUIInterface* t_owner);
        ~GUIScrollbar();

        void SetPosition(const sf::Vector2f& t_pos);

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();

        void ApplyStyle();
        void UpdateStyle(const GUIElementState& t_state, const GUIStyle& t_style);

        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        private:
        SliderType m_sliderType;
        sf::RectangleShape m_slider;
        sf::Vector2f m_moveMouseLast;
        int m_percentage;
    };
}