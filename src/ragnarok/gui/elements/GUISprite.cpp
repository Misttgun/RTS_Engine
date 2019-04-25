
#include "ragnarok/gui/elements/GUISprite.h"
#include "ragnarok/gui//GUIManager.h"
#include "ragnarok/SharedContext.h"
#include "ragnarok/resources/TextureManager.h"

namespace ragnarok
{
    GUISprite::GUISprite(const std::string& t_name, GUIInterface* t_owner)
        : GUIElement(t_name, GUIElementType::Sprite, t_owner), m_textureMgr(m_owner->GetManager()->GetContext()->m_textureManager)
    {}

    GUISprite::~GUISprite()
    {
        if(!m_texture.empty())
        {
            m_textureMgr->ReleaseResource(m_texture);
        }
    }

    void GUISprite::SetTexture(const std::string& t_texture)
    {
        if(!m_texture.empty())
        {
            m_textureMgr->ReleaseResource(m_texture); m_texture = "";
        }

        if(!m_textureMgr->RequireResource(t_texture))
        {
            return;
        }

        m_sprite.setTexture(*m_textureMgr->GetResource(t_texture));
        m_texture = t_texture;
        m_needsRedraw = true;
        m_owner->SetRedraw(true);
    }

    void GUISprite::SetTexture(sf::RenderTexture& t_renderTexture)
    {
        if(!m_texture.empty())
        {
            m_textureMgr->ReleaseResource(m_texture); m_texture = "";
        }

        m_sprite.setTexture(t_renderTexture.getTexture());
        m_needsRedraw = true;
        m_owner->SetRedraw(true);
    }

    void GUISprite::ReadIn(std::stringstream& t_stream)
    {
        std::string TextureName;
        if(!(t_stream >> TextureName))
        {
            return;
        }

        SetTexture(TextureName);
    }

    void GUISprite::OnClick(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Clicked);
    }

    void GUISprite::OnRelease()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUISprite::OnHover(const sf::Vector2f& t_mousePos)
    {
        SetState(GUIElementState::Focused);
    }

    void GUISprite::OnLeave()
    {
        SetState(GUIElementState::Neutral);
    }

    void GUISprite::ApplyStyle()
    {
        GUIElement::ApplyStyle();

        auto texture = m_sprite.getTexture();

        if(texture)
        {
            m_style[m_state].m_size = sf::Vector2f(m_sprite.getTexture()->getSize());
        }

        m_sprite.setPosition(GetPosition());
    }

    void GUISprite::Update(float t_dT)
    {}

    void GUISprite::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_sprite);
        t_target->draw(m_visual.m_text);
    }
}