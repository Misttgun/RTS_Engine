#pragma once

#include <unordered_map>
#include <memory>
#include <SFML/Graphics/Sprite.hpp>
#include "AnimDirectional.h"
#include "../resources/TextureManager.h"
#include "../window/Window.h"

namespace ragnarok
{
	/**
	 * \brief Base class for the manipulation of a spritesheet
	 */
    using Animations = std::unordered_map<std::string, std::unique_ptr<AnimBase>>;

    class SpriteSheet
    {
        public:
        SpriteSheet(TextureManager* t_textMgr);
        ~SpriteSheet();

        void CropSprite(const sf::IntRect& t_rect);
        sf::Vector2u GetSpriteSize()const;
        sf::Vector2f GetSpritePosition()const;
        void SetSpriteSize(const sf::Vector2u& t_size);
        void SetSpritePosition(const sf::Vector2f& t_pos);

        void SetDirection(const Direction& t_dir);
        Direction GetDirection() const;
        void SetSheetPadding(const sf::Vector2f& t_padding);
        void SetSpriteSpacing(const sf::Vector2f& t_spacing);
        sf::Vector2f GetSheetPadding()const;
        sf::Vector2f GetSpriteSpacing()const;
        sf::FloatRect GetSpriteBounds()const;


        bool LoadSheet(const std::string& t_file);
        void ReleaseSheet();

        AnimBase* GetCurrentAnim();
		/**
		 * \brief Set up the current animation, if it is not already the same
		 * \param t_name name of the animation
		 * \param t_play boolean stating if the animation must be played immediately
		 */
        bool SetAnimation(const std::string& t_name, bool t_play = false, bool t_loop = false);

        void Update(float t_dT);
        void Draw(Window* t_wnd);

        private:
        std::string m_texture;
        sf::Sprite m_sprite;
        sf::Vector2u m_spriteSize;
        sf::Vector2f m_spriteScale;
        Direction m_direction;

        sf::Vector2f m_sheetPadding;
        sf::Vector2f m_spriteSpacing;

        std::string m_animType;
        Animations m_animations;
        AnimBase* m_animationCurrent;

        TextureManager* m_textureManager;
    };
}