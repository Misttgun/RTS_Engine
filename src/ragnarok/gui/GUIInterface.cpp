#include "ragnarok/gui/GUIInterface.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/gui/elements/GUITextField.h"
#include "ragnarok/SharedContext.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/utilities/Utilities.h"

namespace ragnarok
{
    GUIInterface::GUIInterface(const std::string& t_name, GUIManager* t_guiManager)
        : GUIElement(t_name, GUIElementType::Window, this), m_parent(nullptr), m_guiManager(t_guiManager),
        m_movable(false), m_beingMoved(false), m_showTitleBar(false), m_focused(false), m_scrollHorizontal(0),
        m_scrollVertical(0), m_contentRedraw(true), m_controlRedraw(true)
    {
        m_backdropTexture = std::make_unique<sf::RenderTexture>();
        m_contentTexture = std::make_unique<sf::RenderTexture>();
        m_controlTexture = std::make_unique<sf::RenderTexture>();
    }

    GUIInterface::~GUIInterface()
    {}

    void GUIInterface::SetPosition(const sf::Vector2f& t_pos)
    {
        GUIElement::SetPosition(t_pos);
        m_backdrop.setPosition(t_pos);
        m_content.setPosition(t_pos + m_contentPositionOffset);
        m_control.setPosition(t_pos);
        m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
        m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
    }

    void GUIInterface::PositionCenterScreen()
    {
        auto size = m_style[m_state].m_size;
        auto w_size = m_guiManager->GetContext()->m_wind->GetWindowSize();
        SetPosition({(w_size.x / 2) - size.x / 2, (w_size.y / 2) - size.y / 2});
    }

    bool GUIInterface::AddElement(const GUIElementType& t_type,
                                  const std::string& t_name)
    {
        if(m_elements.find(t_name) != m_elements.end())
        {
            return false;
        }

        std::unique_ptr<GUIElement> element(m_guiManager->CreateElement(t_type, this));

        if(!element)
        {
            return false;
        }

        element->SetName(t_name);
        element->SetOwner(this);
        m_elements.emplace(t_name, std::move(element));
        m_contentRedraw = true;
        m_controlRedraw = true;

        return true;
    }

    GUIElement* GUIInterface::GetElement(const std::string& t_name) const
    {
        auto itr = m_elements.find(t_name);

        return(itr != m_elements.end() ? itr->second.get() : nullptr);
    }

    /**
     * Removes a GUI element by name and adjusts content size
     * @param t_name The name of the GUI element to remove from this interface
     * @return true upon success, false if the element was not found
     */
    bool GUIInterface::RemoveElement(const std::string& t_name)
    {
        auto itr = m_elements.find(t_name);

        if(itr == m_elements.end())
        {
            return false;
        }

        m_elements.erase(itr);
        m_contentRedraw = true;
        m_controlRedraw = true;
        AdjustContentSize();

        return true;
    }

    bool GUIInterface::RemoveElementsContaining(const std::string& t_snippet)
    {
        bool changed = false;

        for(auto itr = m_elements.begin(); itr != m_elements.end();)
        {
            if(itr->first.find(t_snippet) == std::string::npos)
            {
                ++itr; continue;
            }

            itr = m_elements.erase(itr);
            changed = true;
        }

        if(!changed)
        {
            return false;
        }

        m_contentRedraw = true;
        m_contentRedraw = true;
        AdjustContentSize();

        return true;
    }

    bool GUIInterface::HasParent() const
    {
        return m_parent != nullptr;
    }

    GUIManager* GUIInterface::GetManager() const
    {
        return m_guiManager;
    }

    /**
     * Returns whether passed point is inside this interface
     * @param t_point The point to be tested
     * @return true if the point is within this interface's borders, else false
     */
    bool GUIInterface::IsInside(const sf::Vector2f& t_point) const
    {
        if(GUIElement::IsInside(t_point))
        {
            return true;
        }

        return (m_titleBar.getGlobalBounds().contains(t_point) && m_showTitleBar);
    }

    void GUIInterface::Focus()
    {
        OnFocus();
    }

    void GUIInterface::Defocus()
    {
        OnDefocus();
    }

    bool GUIInterface::IsFocused() const
    {
        return m_focused;
    }

    void GUIInterface::ReadIn(std::stringstream& t_stream)
    {
        std::string movableState;
        std::string titleShow;
        std::string title;
        t_stream >> m_elementPadding.x >> m_elementPadding.y >> movableState >> titleShow;
        Utils::ReadQuotedString(t_stream, title);
        m_visual.m_text.setString(title);
        
        if(movableState == "Movable")
        {
            m_movable = true;
        }

        if(titleShow == "Title")
        {
            m_showTitleBar = true;
        }
    }

    bool GUIInterface::ContentLayerContainsPoint(const sf::Vector2f& t_point)
    {
        auto pos = GetGlobalPosition();
        auto size = (m_contentRectSize == sf::Vector2i(0, 0) ? sf::Vector2i(m_style.at(m_state).m_size) : m_contentRectSize);
        pos += m_contentPositionOffset;
        sf::FloatRect rect;
        rect.left = pos.x;
        rect.top = pos.y;
        rect.width = size.x;
        rect.height = size.y;

        return rect.contains(t_point);
    }

    void GUIInterface::OnClick(const sf::Vector2f& t_mousePos)
    {
        DefocusTextfields();
        
        if(m_titleBar.getGlobalBounds().contains(t_mousePos) && m_showTitleBar)
        {
            if(m_movable)
            {
                m_beingMoved = true;
            }

            for(auto& itr : m_elements)
            {
                if(!itr.second->IsActive())
                {
                    continue;
                }

                itr.second->OnInterfaceClick(t_mousePos - GetPosition());
            }

            return;
        }

        GUIEvent event;
        event.m_type = GUIEventType::Click;
        event.m_interface = m_name;
        event.m_element.clear();
        auto interfacePos = GetPosition();
        event.m_clickCoords.x = t_mousePos.x - interfacePos.x;
        event.m_clickCoords.y = t_mousePos.y - interfacePos.y;
        m_guiManager->AddEvent(event);

        bool contentHasMouse = ContentLayerContainsPoint(t_mousePos);

        for(auto &itr : m_elements)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            auto position = itr.second->GetPosition();
            auto scrollOffset = sf::Vector2f(0.f, 0.f);

            if(!itr.second->IsInside(t_mousePos))
            {
                itr.second->OnInterfaceClick(t_mousePos - interfacePos - position + scrollOffset);
                continue;
            }

            if(!contentHasMouse && !itr.second->IsControl())
            {
                continue;
            }

            event.m_element = itr.second->m_name;

            if(!itr.second->IsControl())
            {
                position += m_contentPositionOffset;
                scrollOffset = sf::Vector2f(m_scrollHorizontal, m_scrollVertical);
            }

            event.m_clickCoords.x = t_mousePos.x - interfacePos.x - position.x + scrollOffset.x;
            event.m_clickCoords.y = t_mousePos.y - interfacePos.y - position.y + scrollOffset.y;

            itr.second->OnClick({event.m_clickCoords.x, event.m_clickCoords.y});
            m_guiManager->AddEvent(event);
        }

        SetState(GUIElementState::Clicked);
    }

    void GUIInterface::OnRelease()
    {
        GUIEvent event;
        event.m_type = GUIEventType::Release;
        event.m_interface = m_name;
        event.m_element.clear();
        auto interfacePos = GetPosition();
        auto mousePos = m_guiManager->GetContext()->m_eventManager->GetMousePos(m_guiManager->GetContext()->m_wind->GetRenderWindow());
        event.m_clickCoords.x = mousePos.x - interfacePos.x;
        event.m_clickCoords.y = mousePos.y - interfacePos.y;
        m_guiManager->AddEvent(event);

        for(auto &itr : m_elements)
        {
            if(itr.second->GetState() != GUIElementState::Clicked)
            {
                continue;
            }

            event.m_element = itr.second->m_name;

            auto position = itr.second->GetPosition();
            auto scrollOffset = sf::Vector2f(0.f, 0.f);

            if(!itr.second->IsControl())
            {
                position += m_contentPositionOffset;
                scrollOffset = sf::Vector2f(m_scrollHorizontal, m_scrollVertical);
            }

            event.m_clickCoords.x = mousePos.x - interfacePos.x - position.x + scrollOffset.x;
            event.m_clickCoords.y = mousePos.y - interfacePos.y - position.y + scrollOffset.y;

            itr.second->OnRelease();
            m_guiManager->AddEvent(event);
        }

        SetState(GUIElementState::Focused);
    }

    void GUIInterface::OnHover(const sf::Vector2f& t_mousePos)
    {
        GUIEvent event;
        event.m_type = GUIEventType::Hover;
        event.m_interface = m_name;
        event.m_element.clear();
        event.m_clickCoords.x = t_mousePos.x;
        event.m_clickCoords.y = t_mousePos.y;
        m_guiManager->AddEvent(event);

        SetState(GUIElementState::Focused);
    }

    void GUIInterface::OnLeave()
    {
        GUIEvent event;
        event.m_type = GUIEventType::Leave;
        event.m_interface = m_name;
        event.m_element.clear();
        m_guiManager->AddEvent(event);

        SetState(GUIElementState::Neutral);
    }

    void GUIInterface::OnTextEntered(const char& t_char)
    {
        for(auto &itr : m_elements)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            if(itr.second->GetType() != GUIElementType::Textfield)
            {
                continue;
            }

            if(itr.second->GetState() != GUIElementState::Clicked)
            {
                continue;
            }

            auto field = static_cast<GUITextfield*>(itr.second.get());
            field->HandleChar(t_char);

            return;
        }
    }

    void GUIInterface::OnFocus()
    {
        m_guiManager->BringToFront(this);

        if(m_focused)
        {
            return;
        }

        m_focused = true;
        GUIEvent event;
        event.m_type = GUIEventType::Focus;
        event.m_interface = m_name;
        event.m_element.clear();
        m_guiManager->AddEvent(event);

        for(auto& itr : m_elements)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            itr.second->OnFocus();
            event.m_element = itr.second->m_name;
            m_guiManager->AddEvent(event);
        }
    }

    void GUIInterface::OnDefocus()
    {
        if(!m_focused)
        {
            return;
        }

        m_focused = false;
        GUIEvent event;
        event.m_type = GUIEventType::Defocus;
        event.m_interface = m_name;
        event.m_element.clear();

        for(auto& itr : m_elements)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            itr.second->OnDefocus();
            event.m_element = itr.second->m_name;
            m_guiManager->AddEvent(event);
        }
    }

    void GUIInterface::OnArrowKey(const std::string& t_eventName)
    {
        for(auto &itr : m_elements)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            if(itr.second->GetType() != GUIElementType::Textfield)
            {
                continue;
            }

            auto field = static_cast<GUITextfield*>(itr.second.get());
            field->HandleArrowKey(t_eventName);

            return;
        }
    }

    sf::Vector2f GUIInterface::GetPadding() const
    {
        return m_elementPadding;
    }

    void GUIInterface::SetPadding(const sf::Vector2f& t_padding)
    {
        m_elementPadding = t_padding;
    }

    void GUIInterface::Update(float t_dT)
    {
        auto mousePos = sf::Vector2f(
            m_guiManager->GetContext()->m_eventManager->GetMousePos(
            m_guiManager->GetContext()->m_wind->GetRenderWindow()));

        if(m_beingMoved && m_moveMouseLast != mousePos)
        {
            sf::Vector2f difference = mousePos - m_moveMouseLast;
            m_moveMouseLast = mousePos;
            sf::Vector2f newPosition = m_position + difference;
            SetPosition(newPosition);
        }

        bool contentHasMouse = ContentLayerContainsPoint(mousePos);

        for(auto &itr : m_elements)
        {
            if(itr.second->NeedsRedraw())
            {
                if(itr.second->IsControl())
                {
                    m_controlRedraw = true;
                }
                else
                {
                    m_contentRedraw = true;
                }
            }

            if(!itr.second->IsActive())
            {
                continue;
            }

            itr.second->Update(t_dT);

            if(m_beingMoved)
            {
                continue;
            }

            GUIEvent event;
            event.m_interface = m_name;
            event.m_element = itr.second->m_name;
            event.m_clickCoords.x = mousePos.x;
            event.m_clickCoords.y = mousePos.y;

            if(IsInside(mousePos) && itr.second->IsInside(mousePos)
               && !m_titleBar.getGlobalBounds().contains(mousePos)
               && !(!contentHasMouse && !itr.second->IsControl())
               && m_state != GUIElementState::Neutral)
            {
                if(itr.second->GetState() != GUIElementState::Neutral)
                {
                    continue;
                }

                itr.second->OnHover(mousePos);
                event.m_type = GUIEventType::Hover;
                m_guiManager->AddEvent(event);
            }
            else if(itr.second->GetState() == GUIElementState::Focused)
            {
                itr.second->OnLeave();
                event.m_type = GUIEventType::Leave;
                m_guiManager->AddEvent(event);
            }
        }
    }

    /**
     * Draws this interface to the passed render target
     * @param t_target The render target to use for drawing
     */
    void GUIInterface::Draw(sf::RenderTarget* t_target)
    {
        t_target->draw(m_backdrop);
        t_target->draw(m_content);
        t_target->draw(m_control);

        if(!m_showTitleBar)
        {
            return;
        }

        t_target->draw(m_titleBar);
        t_target->draw(m_visual.m_text);
    }

    bool GUIInterface::IsBeingMoved() const
    {
        return m_beingMoved;
    }

    bool GUIInterface::IsMovable() const
    {
        return m_movable;
    }

    void GUIInterface::BeginMoving()
    {
        if(!m_showTitleBar || !m_movable)
        {
            return;
        }

        m_beingMoved = true;
        SharedContext* context = m_guiManager->GetContext();
        m_moveMouseLast = sf::Vector2f(context->m_eventManager->
                                       GetMousePos(context->m_wind->GetRenderWindow()));
    }

    void GUIInterface::StopMoving()
    {
        m_beingMoved = false;
    }

    /**
     * Returns this interface's position relative to the screen
     * @return This interface's global position
     */
    sf::Vector2f GUIInterface::GetGlobalPosition() const
    {
        sf::Vector2f pos = m_position;
        GUIInterface* i = m_parent;
        while(i)
        {
            pos += i->GetPosition();
            i = i->m_parent;
        }
        return pos;
    }

    void GUIInterface::ApplyStyle()
    {
        GUIElement::ApplyStyle(); // Call base method.
        m_visual.m_backgroundSolid.setPosition(0.f, 0.f);
        m_visual.m_backgroundImage.setPosition(0.f, 0.f);
        m_titleBar.setSize(sf::Vector2f(m_style[m_state].m_size.x, 16.f));
        m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
        m_titleBar.setFillColor(m_style[m_state].m_elementColor);
        m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
        m_visual.m_glyph.setPosition(m_titleBar.getPosition() + m_style[m_state].m_glyphPadding);
    }

    void GUIInterface::Redraw()
    {
        if(m_backdropTexture->getSize().x != m_style[m_state].m_size.x ||
           m_backdropTexture->getSize().y != m_style[m_state].m_size.y)
        {
            m_backdropTexture->create(
                static_cast<unsigned int>(m_style[m_state].m_size.x),
                static_cast<unsigned int>(m_style[m_state].m_size.y));
        }

        m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
        ApplyStyle();
        m_backdropTexture->draw(m_visual.m_backgroundSolid);

        if(!m_style[m_state].m_backgroundImage.empty())
        {
            m_backdropTexture->draw(m_visual.m_backgroundImage);
        }

        m_backdropTexture->display();
        m_backdrop.setTexture(m_backdropTexture->getTexture());
        m_backdrop.setTextureRect(sf::IntRect(0, 0,
                                  static_cast<int>(m_style[m_state].m_size.x),
                                  static_cast<int>(m_style[m_state].m_size.y)));
        SetRedraw(false);
    }
    bool GUIInterface::NeedsContentRedraw() const
    {
        return m_contentRedraw;
    }

    void GUIInterface::RedrawContent()
    {
        if(m_contentTexture->getSize().x != m_contentSize.x ||
           m_contentTexture->getSize().y != m_contentSize.y)
        {
            m_contentTexture->create(
                static_cast<unsigned int>(m_contentSize.x),
                static_cast<unsigned int>(m_contentSize.y));
        }

        m_contentTexture->clear(sf::Color(0, 0, 0, 0));

        for(auto &itr : m_elements)
        {
            auto& element = itr.second;
            if(!element->IsActive() || element->IsControl())
            {
                continue;
            }

            element->ApplyStyle();
            element->Draw(m_contentTexture.get());
            element->SetRedraw(false);
        }

        m_contentTexture->display();
        m_content.setTexture(m_contentTexture->getTexture());

        sf::Vector2i size = (m_contentRectSize == sf::Vector2i(0, 0) ?
                             sf::Vector2i(m_style[m_state].m_size.x, m_style[m_state].m_size.y) :
                             m_contentRectSize);

        m_content.setTextureRect(sf::IntRect(
            m_scrollHorizontal, m_scrollVertical,
            size.x, size.y
        ));
        m_contentRedraw = false;
    }

    bool GUIInterface::NeedsControlRedraw() const
    {
        return m_controlRedraw;
    }

    void GUIInterface::RedrawControls()
    {
        if(m_controlTexture->getSize().x != m_style[m_state].m_size.x ||
           m_controlTexture->getSize().y != m_style[m_state].m_size.y)
        {
            m_controlTexture->create(
                static_cast<unsigned int>(m_style[m_state].m_size.x),
                static_cast<unsigned int>(m_style[m_state].m_size.y));
        }

        m_controlTexture->clear(sf::Color(0, 0, 0, 0));

        for(auto &itr : m_elements)
        {
            auto& element = itr.second;
            if(!element->IsActive() || !element->IsControl())
            {
                continue;
            }

            element->ApplyStyle();
            element->Draw(m_controlTexture.get());
            element->SetRedraw(false);
        }

        m_controlTexture->display();
        m_control.setTexture(m_controlTexture->getTexture());
        m_control.setTextureRect(sf::IntRect(0, 0,
                                 static_cast<int>(m_style[m_state].m_size.x),
                                 static_cast<int>(m_style[m_state].m_size.y)));
        m_controlRedraw = false;
    }

    void GUIInterface::RequestContentRedraw()
    {
        m_contentRedraw = true;
    }

    void GUIInterface::ToggleTitleBar()
    {
        m_showTitleBar = !m_showTitleBar;
    }

    void GUIInterface::AdjustContentSize(const GUIElement* t_reference)
    {
        if(t_reference)
        {
            sf::Vector2f bottomRight = t_reference->GetPosition() + t_reference->GetSize();
            if(bottomRight.x > m_contentSize.x)
            {
                m_contentSize.x = bottomRight.x; m_controlRedraw = true;
            }

            if(bottomRight.y > m_contentSize.y)
            {
                m_contentSize.y = bottomRight.y; m_controlRedraw = true;
            }

            return;
        }

        sf::Vector2f farthest = GetSize();

        for(auto &itr : m_elements)
        {
            auto& element = itr.second;

            if(!element->IsActive() || element->IsControl())
            {
                continue;
            }

            sf::Vector2f bottomRight = element->GetPosition() + element->GetSize();
            if(bottomRight.x > farthest.x)
            {
                farthest.x = bottomRight.x; m_controlRedraw = true;
            }

            if(bottomRight.y > farthest.y)
            {
                farthest.y = bottomRight.y; m_controlRedraw = true;
            }
        }
        SetContentSize(farthest);
    }

    void GUIInterface::SetContentSize(const sf::Vector2f& t_vec)
    {
        m_contentSize = t_vec;
    }

    void GUIInterface::UpdateScrollHorizontal(unsigned int t_percent)
    {
        if(t_percent > 100)
        {
            return;
        }

        float size = (m_contentRectSize.x == 0 ? GetSize().x : m_contentRectSize.x);
        m_scrollHorizontal = static_cast<int>(((m_contentSize.x - size) / 100) * t_percent);
        sf::IntRect rect = m_content.getTextureRect();
        m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
    }

    void GUIInterface::UpdateScrollVertical(unsigned int t_percent)
    {
        if(t_percent > 100)
        {
            return;
        }

        float size = (m_contentRectSize.y == 0 ? GetSize().y : m_contentRectSize.y);
        m_scrollVertical = static_cast<int>(((m_contentSize.y - size) / 100) * t_percent);
        sf::IntRect rect = m_content.getTextureRect();
        m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
    }

    sf::Vector2f GUIInterface::GetContentSize() const
    {
        return m_contentSize;
    }

    sf::Vector2i GUIInterface::GetContentRectSize() const
    {
        return m_contentRectSize;
    }

    sf::Vector2f GUIInterface::GetContentOffset() const
    {
        return m_contentPositionOffset;
    }

    void GUIInterface::SetContentRectSize(const sf::Vector2i& t_size)
    {
        m_contentRectSize = t_size;
    }

    void GUIInterface::SetContentOffset(const sf::Vector2f& t_offset)
    {
        m_contentPositionOffset = t_offset; m_content.setPosition(GetPosition() + t_offset);
    }

    void GUIInterface::DefocusTextfields()
    {
        GUIEvent event;
        event.m_type = GUIEventType::Defocus;
        event.m_interface = m_name;
        event.m_element.clear();
        for(auto &itr : m_elements)
        {
            if(itr.second->GetType() != GUIElementType::Textfield)
            {
                continue;
            }

            itr.second->OnDefocus();
            if(!itr.second->IsActive() || !IsActive())
            {
                continue;
            }

            event.m_element = itr.second->m_name;
            m_guiManager->AddEvent(event);
        }
    }
}
