#include "ragnarok/sprites/AnimDirectional.h"
#include "ragnarok/sprites/SpriteSheet.h"

namespace ragnarok
{
    void AnimDirectional::CropSprite()
    {
        sf::Vector2f padding = m_spriteSheet->GetSheetPadding();
        sf::Vector2f spacing = m_spriteSheet->GetSpriteSpacing();

        sf::IntRect rect(
            (m_spriteSheet->GetSpriteSize().x * m_frameCurrent) + static_cast<unsigned int>(padding.x) +
            (static_cast<unsigned int>(spacing.x) * m_frameCurrent),
            (m_spriteSheet->GetSpriteSize().y * (m_frameRow + static_cast<short>(m_spriteSheet->GetDirection())))
            + static_cast<unsigned int>(padding.y) + ((m_frameRow + static_cast<short>(m_spriteSheet->GetDirection())) *
            static_cast<unsigned int>(spacing.y)),
            m_spriteSheet->GetSpriteSize().x,
            m_spriteSheet->GetSpriteSize().y);

        m_spriteSheet->CropSprite(rect);
    }

    void AnimDirectional::FrameStep()
    {
        bool b = SetFrame(m_frameCurrent + (m_frameStart <= m_frameEnd ? 1 : -1));
        if(b)
        {
            return;
        }

        if(m_loop)
        {
            SetFrame(m_frameStart);
        }
        else
        {
            SetFrame(m_frameEnd); Pause();
        }
    }

    void AnimDirectional::ReadIn(std::stringstream& t_stream)
    {
        t_stream >> m_frameStart >> m_frameEnd >> m_frameRow >> m_frameTime >> m_frameActionStart >> m_frameActionEnd;
    }
}