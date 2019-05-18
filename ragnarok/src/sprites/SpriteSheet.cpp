#include "../../include/sprites/SpriteSheet.h"

namespace ragnarok
{
    SpriteSheet::SpriteSheet(TextureManager* t_textMgr)
        :m_textureManager(t_textMgr), m_animationCurrent(nullptr), m_spriteScale(1.f, 1.f), m_direction(Direction::Down)
    {}

    SpriteSheet::~SpriteSheet()
    {
        ReleaseSheet();
    }

    void SpriteSheet::ReleaseSheet()
    {
        m_textureManager->ReleaseResource(m_texture);
        m_animationCurrent = nullptr;
        m_animations.clear();
    }

    sf::Vector2u SpriteSheet::GetSpriteSize()const
    {
        return m_spriteSize;
    }

    sf::Vector2f SpriteSheet::GetSpritePosition()const
    {
        return m_sprite.getPosition();
    }

    Direction SpriteSheet::GetDirection() const
    {
        return m_direction;
    }

    AnimBase* SpriteSheet::GetCurrentAnim()
    {
        return m_animationCurrent;
    }

    void SpriteSheet::SetSpriteSize(const sf::Vector2u& t_size)
    {
        m_spriteSize = t_size;
        m_sprite.setOrigin(static_cast<float>(m_spriteSize.x) / 2, static_cast<float>(m_spriteSize.y) / 2);
    }

    void SpriteSheet::SetSpritePosition(const sf::Vector2f& t_pos)
    {
        m_sprite.setPosition(t_pos);
    }

    void SpriteSheet::SetDirection(const Direction& t_dir)
    {
        if(t_dir == m_direction)
        {
            return;
        }

        m_direction = t_dir;
        m_animationCurrent->CropSprite();
    }

    void SpriteSheet::SetSheetPadding(const sf::Vector2f& t_padding)
    {
        m_sheetPadding = t_padding;
    }

    void SpriteSheet::SetSpriteSpacing(const sf::Vector2f& t_spacing)
    {
        m_spriteSpacing = t_spacing;
    }

    sf::Vector2f SpriteSheet::GetSheetPadding()const
    {
        return m_sheetPadding;
    }

    sf::Vector2f SpriteSheet::GetSpriteSpacing()const
    {
        return m_spriteSpacing;
    }

    sf::FloatRect SpriteSheet::GetSpriteBounds() const
    {
        return m_sprite.getGlobalBounds();
    }

    void SpriteSheet::CropSprite(const sf::IntRect& t_rect)
    {
        m_sprite.setTextureRect(t_rect);
    }

    bool SpriteSheet::LoadSheet(const std::string& t_file)
    {
        std::ifstream sheet;
        sheet.open(Utils::GetWorkingDirectory() + t_file);
        
        if(!sheet.is_open())
        {
            std::cout << "! Failed loading spritesheet: "
                << t_file << std::endl;
            return false;
        }

        // Release the current spritesheet resources.
        ReleaseSheet();
        std::string line;
        
        while(std::getline(sheet, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;
            
            if(type == "Texture")
            {
                if(!m_texture.empty())
                {
                    std::cout << "! Duplicate texture entries in: " << t_file << std::endl;
                    continue;
                }

                std::string texture;
                keystream >> texture;

                if(!m_textureManager->RequireResource(texture))
                {
                    std::cout << "! Could not set up the texture: " << texture << std::endl;
                    continue;
                }

                m_texture = texture;
                m_sprite.setTexture(*m_textureManager->GetResource(m_texture));
            }
            else if(type == "Size")
            {
                keystream >> m_spriteSize.x >> m_spriteSize.y;
                SetSpriteSize(m_spriteSize);
            }
            else if(type == "Scale")
            {
                keystream >> m_spriteScale.x >> m_spriteScale.y;
                m_sprite.setScale(m_spriteScale);
            }
            else if(type == "Padding")
            {
                keystream >> m_sheetPadding.x >> m_sheetPadding.y;
            }
            else if(type == "Spacing")
            {
                keystream >> m_spriteSpacing.x >> m_spriteSpacing.y;
            }
            else if(type == "AnimationType")
            {
                keystream >> m_animType;
            }
            else if(type == "Animation")
            {
                std::string name;
                keystream >> name;

                if(m_animations.find(name) != m_animations.end())
                {
                    std::cerr << "! Duplicate animation(" << name << ") in: " << t_file << std::endl;
                    continue;
                }

                std::unique_ptr<AnimBase> anim;

                if(m_animType == "Directional")
                {
                    anim = std::make_unique<AnimDirectional>();
                }
                else
                {
                    std::cerr << "! Unknown animation type: " << m_animType << std::endl;
                    continue;
                }

                keystream >> *anim;
                anim->SetSpriteSheet(this);
                anim->SetName(name);
                anim->Reset();

                if(!m_animationCurrent)
                {
                    m_animationCurrent = anim.get();
                    m_animationCurrent->Play();
                }

                m_animations.emplace(name, std::move(anim));
            }
        }

        sheet.close();

        return true;
    }

    bool SpriteSheet::SetAnimation(const std::string& t_name, bool t_play, bool t_loop)
    {
        auto itr = m_animations.find(t_name);

        if(itr == m_animations.end())
        {
            return false;
        }

        if(itr->second.get() == m_animationCurrent)
        {
            return false;
        }

        if(m_animationCurrent)
        {
            m_animationCurrent->Stop();
        }

        m_animationCurrent = itr->second.get();
        m_animationCurrent->SetLooping(t_loop);

        if(t_play)
        {
            m_animationCurrent->Play();
        }

        m_animationCurrent->CropSprite();

        return true;
    }

    void SpriteSheet::Update(float t_dT)
    {
        m_animationCurrent->Update(t_dT);
    }

    void SpriteSheet::Draw(Window* t_wnd)
    {
        t_wnd->GetRenderer()->Draw(m_sprite);
    }
}
