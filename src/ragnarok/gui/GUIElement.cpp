#include "ragnarok/gui/GUIElement.h"
#include "ragnarok/gui/GUIInterface.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/SharedContext.h"
#include "ragnarok/resources/TextureManager.h"
#include "ragnarok/resources/FontManager.h"
#include "ragnarok/utilities/Helpers.h"

namespace ragnarok
{
    GUIElement::GUIElement(const std::string& t_name, const GUIElementType& t_type, GUIInterface* t_owner)
        : m_name(t_name), m_type(t_type), m_state(GUIElementState::Neutral), m_owner(t_owner), m_needsRedraw(false), m_active(true), m_isControl(false)
    {}

    GUIElement::~GUIElement()
    {
        ReleaseResources();
    }

    /**
     * Releases all resources for all styles
     */
    void GUIElement::ReleaseResources()
    {
        for(auto &itr : m_style)
        {
            ReleaseTexture(itr.second.m_backgroundImage);
            ReleaseTexture(itr.second.m_glyph);
            ReleaseFont(itr.second.m_textFont);
        }
    }

    /**
     * Registers passed style for passed state, applying it if state is current
     *
     * Takes care of releasing old state's resources and requiring new resources
     * @param t_state The state to register the passed style for
     * @param t_style The style to register for the passed state
     */
    void GUIElement::UpdateStyle(const GUIElementState& t_state, const GUIStyle& t_style)
    {
        // Resource management.
        if(t_style.m_backgroundImage != m_style[t_state].m_backgroundImage)
        {
            ReleaseTexture(m_style[t_state].m_backgroundImage);
            RequireTexture(t_style.m_backgroundImage);
        }

        if(t_style.m_glyph != m_style[t_state].m_glyph)
        {
            ReleaseTexture(m_style[t_state].m_glyph);
            RequireTexture(t_style.m_glyph);
        }

        if(t_style.m_textFont != m_style[t_state].m_textFont)
        {
            ReleaseFont(m_style[t_state].m_textFont);
            RequireFont(t_style.m_textFont);
        }

        // Style application.
        m_style[t_state] = t_style;

        if(t_state == m_state)
        {
            SetRedraw(true); ApplyStyle();
        }
    }

    /**
     * Applies current style for all visuals and calls owner's AdjustContentSize
     */
    void GUIElement::ApplyStyle()
    {
        ApplyTextStyle();
        ApplyBgStyle();
        ApplyGlyphStyle();

        if(m_style[m_state].m_stretchToFit)
        {
            m_style[m_state].m_size = GetContentSize();
        }

        if(m_owner != this && !IsControl())
        {
            m_owner->AdjustContentSize(this);
        }
    }

    /**
     * Returns the greatest size of this element's content or style
     * @return The max size of the visuals or the size of the current style
     */
    sf::Vector2f GUIElement::GetContentSize() const
    {
        const GUIStyle& CurrentStyle = m_style.at(m_state);
        auto textBounds = m_visual.m_text.getGlobalBounds();
        auto textSize = sf::Vector2f(textBounds.width, textBounds.height);
        auto bgSize = m_visual.m_backgroundSolid.getSize();
        auto imageBounds = m_visual.m_backgroundImage.getGlobalBounds();
        auto bgImageSize = sf::Vector2f(imageBounds.width, imageBounds.height);
        auto glyphBounds = m_visual.m_glyph.getGlobalBounds();
        auto glyphSize = sf::Vector2f(glyphBounds.width, glyphBounds.height);

        sf::Vector2f max;
        max.x = std::max(textSize.x, bgSize.x);
        max.x = std::max(max.x, bgImageSize.x);
        max.x = std::max(max.x, glyphSize.x);
        max.x = std::max(max.x, CurrentStyle.m_size.x);
        max.y = std::max(textSize.y, bgSize.y);
        max.y = std::max(max.y, bgImageSize.y);
        max.y = std::max(max.y, glyphSize.y);
        max.y = std::max(max.y, CurrentStyle.m_size.y);
        return max;
    }

    /**
     * Sets text properties based on current style
     */
    void GUIElement::ApplyTextStyle()
    {
        FontManager* fonts = m_owner->GetManager()->GetContext()->m_fontManager;
        const GUIStyle& CurrentStyle = m_style[m_state];

        if(!CurrentStyle.m_textFont.empty())
        {
            m_visual.m_text.setFont(*fonts->GetResource(CurrentStyle.m_textFont));
            m_visual.m_text.setFillColor(CurrentStyle.m_textColor);
            m_visual.m_text.setCharacterSize(CurrentStyle.m_textSize);

            if(CurrentStyle.m_textCenterOrigin)
            {
                Utils::CenterSFMLText(m_visual.m_text);
            }
            else
            {
                m_visual.m_text.setOrigin(0.f, 0.f);
            }
        }

        if(CurrentStyle.m_textCenterOrigin && CurrentStyle.m_stretchToFit)
        {
            auto contentSize = GetContentSize();
            auto position = m_position + (contentSize / 2.f);
            m_visual.m_text.setPosition(position + CurrentStyle.m_textPadding);
        }
        else
        {
            m_visual.m_text.setPosition(m_position + CurrentStyle.m_textPadding);
        }
    }

    /**
     * Sets background texture and properties based on style and own attributes
     */
    void GUIElement::ApplyBgStyle()
    {
        TextureManager* textures = m_owner->GetManager()->GetContext()->m_textureManager;
        const GUIStyle& CurrentStyle = m_style[m_state];
        if(!CurrentStyle.m_backgroundImage.empty())

        {
            m_visual.m_backgroundImage.setTexture(*textures->GetResource(CurrentStyle.m_backgroundImage));
            m_visual.m_backgroundImage.setColor(CurrentStyle.m_backgroundImageColor);
        }

        m_visual.m_backgroundImage.setPosition(m_position);
        m_visual.m_backgroundSolid.setSize(CurrentStyle.m_size);
        m_visual.m_backgroundSolid.setFillColor(CurrentStyle.m_backgroundColor);
        m_visual.m_backgroundSolid.setPosition(m_position);
    }

    /**
     * Sets glyph texture and properties based on style and own attributes
     */
    void GUIElement::ApplyGlyphStyle()
    {
        const GUIStyle& CurrentStyle = m_style[m_state];
        TextureManager* textures = m_owner->GetManager()->GetContext()->m_textureManager;

        if(!CurrentStyle.m_glyph.empty())
        {
            sf::Texture *texture = textures->GetResource(CurrentStyle.m_glyph);
            if (texture != nullptr)
            {
                m_visual.m_glyph.setTexture(*texture);
            }
            else
            {
                std::cerr << "Couldn't get texture " << CurrentStyle.m_glyph << std::endl;
            }
        }

        m_visual.m_glyph.setPosition(m_position + CurrentStyle.m_glyphPadding);
    }

    GUIElementType GUIElement::GetType() const
    {
        return m_type;
    }

    /**
     * Sets state and triggers a redraw if needed
     * @param t_state The new active state for this element
     */
    void GUIElement::SetState(const GUIElementState& t_state)
    {
        if(m_state == t_state)
        {
            return;
        }

        m_state = t_state;
        SetRedraw(true);
    }

    std::string GUIElement::GetName() const
    {
        return m_name;
    }

    std::string GUIElement::GetFileName() const
    {
        return m_fileName;
    }

    void GUIElement::SetName(const std::string& t_name)
    {
        m_name = t_name;
    }

    void GUIElement::SetFileName(const std::string & t_fileName)
    {
        m_fileName = t_fileName;
    }

    const sf::Vector2f& GUIElement::GetPosition() const
    {
        return m_position;
    }

    /**
     * Sets position to passed one, staying inside owner's padding as border
     * @param t_pos The new position to assume
     */
    void GUIElement::SetPosition(const sf::Vector2f& t_pos)
    {
        m_position = t_pos;

        if(m_owner == nullptr || m_owner == this)
        {
            return;
        }

        const auto& padding = m_owner->GetPadding();

        if(m_position.x < padding.x)
        {
            m_position.x = padding.x;
        }

        if(m_position.y < padding.y)
        {
            m_position.y = padding.y;
        }
    }

    sf::Vector2f GUIElement::GetSize() const
    {
        return m_style.at(m_state).m_size;
    }

    GUIElementState GUIElement::GetState() const
    {
        return m_state;
    }

    void GUIElement::SetRedraw(const bool& t_redraw)
    {
        m_needsRedraw = t_redraw;
    }

    bool GUIElement::NeedsRedraw() const
    {
        return m_needsRedraw;
    }

    void GUIElement::SetOwner(GUIInterface* t_owner)
    {
        m_owner = t_owner;
    }

    GUIInterface* GUIElement::GetOwner() const
    {
        return m_owner;
    }

    bool GUIElement::HasOwner() const
    {
        return m_owner != nullptr;
    }

    bool GUIElement::IsActive() const
    {
        return m_active;
    }

    /**
     * Sets active boolean and triggers a redraw if needed
     * @param t_active The new active state for this element
     */
    void GUIElement::SetActive(const bool& t_active)
    {
        if(t_active != m_active)
        {
            m_active = t_active; SetRedraw(true);
        }
    }

    bool GUIElement::IsControl() const
    {
        return m_isControl;
    }

    void GUIElement::SetControl(bool t_control)
    {
        m_isControl = t_control;
    }

    std::string GUIElement::GetText() const
    {
        return m_visual.m_text.getString();
    }

    /**
     * Sets text and triggers a redraw
     * @param t_text The text to put in this element
     */
    void GUIElement::SetText(const std::string& t_text)
    {
        m_visual.m_text.setString(t_text); SetRedraw(true);
    }

    std::string GUIElement::GetStyleName() const
    {
        return m_style.at(GUIElementState::Neutral).m_styleName;
    }

    /**
     * Returns whether passed point is inside this element
     * @param t_point The point to test for collision
     * @return True if the point is inside this element, false otherwise
     */
    bool GUIElement::IsInside(const sf::Vector2f& t_point) const
    {
        sf::Vector2f position = GetGlobalPosition();
        return(t_point.x >= position.x &&
               t_point.y >= position.y &&
               t_point.x <= position.x + m_style.at(m_state).m_size.x &&
               t_point.y <= position.y + m_style.at(m_state).m_size.y);
    }

    /**
     * Returns this element's position relative to the screen
     * @return This element's global position
     */
    sf::Vector2f GUIElement::GetGlobalPosition() const
    {
        sf::Vector2f position = GetPosition();

        if(m_owner == nullptr || m_owner == this)
        {
            return position;
        }

        position += m_owner->GetGlobalPosition();

        if(IsControl())
        {
            return position;
        }

        position += m_owner->GetContentOffset();
        position.x -= m_owner->m_scrollHorizontal;
        position.y -= m_owner->m_scrollVertical;

        return position;
    }

    /**
     * Requires the texture resource with passed name to the texture manager
     * @param t_name The name of the texture to require
     */
    void GUIElement::RequireTexture(const std::string& t_name)
    {
        if(t_name.empty())
        {
            return;
        }

        m_owner->GetManager()->GetContext()->m_textureManager->RequireResource(t_name);
    }

    /**
     * Requires the font resource with passed name to the font manager
     * @param t_name The name of the font to require
     */
    void GUIElement::RequireFont(const std::string& t_name)
    {
        if(t_name.empty())
        {
            return;
        }

        m_owner->GetManager()->GetContext()->m_fontManager->RequireResource(t_name);
    }

    /**
     * Releases the texture resource with passed name using the texture manager
     * @param t_name The name of the texture to release
     */
    void GUIElement::ReleaseTexture(const std::string& t_name)
    {
        if(t_name.empty())
        {
            return;
        }

        m_owner->GetManager()->GetContext()->m_textureManager->ReleaseResource(t_name);
    }

    /**
     * Releases the font resource with passed name using the font manager
     * @param t_name The name of the font to release
     */
    void GUIElement::ReleaseFont(const std::string& t_name)
    {
        if(t_name.empty())
        {
            return;
        }

        m_owner->GetManager()->GetContext()->m_fontManager->ReleaseResource(t_name);
    }
}