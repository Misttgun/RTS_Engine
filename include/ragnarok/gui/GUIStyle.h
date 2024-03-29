#pragma once

#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace ragnarok
{
    enum class GUIElementState
    {
        Neutral, Focused, Clicked
    };

    struct GUIStyle
    {
        GUIStyle() : m_textSize(12), m_textCenterOrigin(false), m_stretchToFit(false), m_backgroundImageColor(255, 255, 255, 255)
        {
            sf::Color none = sf::Color(0, 0, 0, 0);
            m_backgroundColor = none;
            m_elementColor = none;
            m_textColor = sf::Color(255, 255, 255, 255);
        }

        std::string m_styleName;

        sf::Vector2f m_size; // Element size.
        // Background properties.
        sf::Color m_backgroundColor;
        sf::Color m_elementColor;
        std::string m_backgroundImage;
        sf::Color m_backgroundImageColor;
        // Text properties.
        sf::Color m_textColor;
        std::string m_textFont;
        sf::Vector2f m_textPadding;
        unsigned int m_textSize;
        bool m_textCenterOrigin;
        bool m_stretchToFit;
        // Glyph properties.
        std::string m_glyph;
        sf::Vector2f m_glyphPadding;
    };

    struct GUIVisual
    {
        sf::RectangleShape m_backgroundSolid;
        sf::Sprite m_backgroundImage;
        sf::Sprite m_glyph;
        sf::Text m_text;
    };
}