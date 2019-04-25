#pragma once

#include <memory>
#include "C_Drawable.h"
#include "../../sprites/SpriteSheet.h"

namespace ragnarok
{

    /**
     * \brief Represents a spritesheet component.
     */
    class C_SpriteSheet : public C_Drawable
    {
        public:
        C_SpriteSheet() : C_Drawable(Component::SpriteSheet), m_spriteSheet(nullptr)
        {}

        ~C_SpriteSheet() = default;

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_sheetName;
        }

        void Create(TextureManager* t_textureMgr, const std::string& t_name = "")
        {
            if(m_spriteSheet)
            {
                m_spriteSheet.release();
            }

            m_spriteSheet = std::make_unique<SpriteSheet>(t_textureMgr);
            m_spriteSheet->LoadSheet("res/Spritesheets/" + (!t_name.empty() ? t_name : m_sheetName) + ".sheet");
        }

        SpriteSheet* GetSpriteSheet() const
        {
            return m_spriteSheet.get();
        }

        void UpdatePosition(const sf::Vector2f& t_vec) override
        {
            m_spriteSheet->SetSpritePosition(t_vec);
        }

        sf::Vector2u GetSize() const override
        {
            return m_spriteSheet->GetSpriteSize();
        }

        void Draw(Window* t_wind) override
        {
            if(!m_spriteSheet)
            {
                return;
            }

            m_spriteSheet->Draw(t_wind);
        }

        private:
        std::unique_ptr<SpriteSheet> m_spriteSheet;
        std::string m_sheetName;
    };
}