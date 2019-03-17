#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "../GUIElement.h"

namespace ragnarok
{
    class TextureManager;

    class GUISprite : public GUIElement
    {
        public:
        GUISprite(const std::string& t_name, GUIInterface* t_owner);
        ~GUISprite();

        void SetTexture(const std::string& t_texture);
        void SetTexture(sf::RenderTexture& t_renderTexture);

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void ApplyStyle();
        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        private:
        TextureManager * m_textureMgr;
        std::string m_texture;
        sf::Sprite m_sprite;
    };
}