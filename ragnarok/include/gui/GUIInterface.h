#pragma once

#include <unordered_map>
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "GUIElement.h"

namespace ragnarok
{
    using Elements = std::unordered_map<std::string, std::unique_ptr<GUIElement>>;

    class GUIManager;

    class GUIInterface : public GUIElement
    {
        friend class GUIElement;
        friend class GUIManager;

        public:
        GUIInterface(const std::string& t_name, GUIManager* t_guiManager);
        ~GUIInterface();

        void SetPosition(const sf::Vector2f& t_pos);
        void PositionCenterScreen();

        bool AddElement(const GUIElementType& t_type, const std::string& t_name);
        GUIElement* GetElement(const std::string& t_name) const;
        bool RemoveElement(const std::string& t_name);
        bool RemoveElementsContaining(const std::string& t_snippet);

        bool HasParent() const;
        GUIManager* GetManager() const;

        bool IsInside(const sf::Vector2f& t_point) const;

        void Focus();
        void Defocus();
        bool IsFocused() const;

        void ReadIn(std::stringstream& t_stream);
        void OnClick(const sf::Vector2f& t_mousePos);
        void OnRelease();
        void OnHover(const sf::Vector2f& t_mousePos);
        void OnLeave();
        void OnTextEntered(const char& t_char);
        void OnFocus();
        void OnDefocus();
        void OnArrowKey(const std::string& t_eventName);

        bool IsBeingMoved() const;
        bool IsMovable() const;
        void BeginMoving();
        void StopMoving();

        void ApplyStyle();

        sf::Vector2f GetPadding() const;
        void SetPadding(const sf::Vector2f& t_padding);
        sf::Vector2f GetGlobalPosition() const;
        sf::Vector2f GetContentSize() const;
        sf::Vector2i GetContentRectSize() const;
        sf::Vector2f GetContentOffset() const;
        void SetContentRectSize(const sf::Vector2i& t_size);
        void SetContentOffset(const sf::Vector2f& t_offset);

        void Redraw();
        bool NeedsContentRedraw() const;
        void RedrawContent();
        bool NeedsControlRedraw() const;
        void RedrawControls();

        void RequestContentRedraw();

        void ToggleTitleBar();

        void Update(float t_dT);
        void Draw(sf::RenderTarget* t_target);

        void UpdateScrollHorizontal(unsigned int t_percent);
        void UpdateScrollVertical(unsigned int t_percent);

        private:
        bool ContentLayerContainsPoint(const sf::Vector2f& t_point);
        void DefocusTextfields();
        Elements m_elements;
        sf::Vector2f m_elementPadding;

        GUIInterface* m_parent;
        GUIManager* m_guiManager;

        std::unique_ptr<sf::RenderTexture> m_backdropTexture;
        sf::Sprite m_backdrop;

        // Movement.
        sf::RectangleShape m_titleBar;
        sf::Vector2f m_moveMouseLast;
        bool m_showTitleBar;
        bool m_movable;
        bool m_beingMoved;
        bool m_focused;

        // Variable size.
        void AdjustContentSize(const GUIElement* t_reference = nullptr);
        void SetContentSize(const sf::Vector2f& t_vec);
        std::unique_ptr<sf::RenderTexture> m_contentTexture;
        sf::Sprite m_content;
        sf::Vector2f m_contentSize;
        sf::Vector2i m_contentRectSize;
        sf::Vector2f m_contentPositionOffset;
        int m_scrollHorizontal;
        int m_scrollVertical;
        bool m_contentRedraw;

        // Control layer.
        std::unique_ptr<sf::RenderTexture> m_controlTexture;
        sf::Sprite m_control;
        bool m_controlRedraw;
    };
}