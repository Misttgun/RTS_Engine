#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GUIStyle.h"

namespace ragnarok
{
    enum class GUIElementType
    {
        None = -1, Window, Label, Button, Scrollbar, Textfield, CheckBox, Sprite, DropDownMenu
    };

    using ElementStyles = std::unordered_map<GUIElementState, GUIStyle>;

    class GUIInterface;

    class GUIElement
    {
        friend class GUIInterface;

        public:
        GUIElement(const std::string& t_name, const GUIElementType& t_type, GUIInterface* t_owner);
        virtual ~GUIElement();

        // Event methods.
        virtual void ReadIn(std::stringstream& t_stream)
        {}

        virtual void OnClick(const sf::Vector2f& t_mousePos)
        {}

        virtual void OnInterfaceClick(const sf::Vector2f& t_mousePos)
        {}

        virtual void OnRelease()
        {}

        virtual void OnHover(const sf::Vector2f& t_mousePos)
        {}

        virtual void OnLeave()
        {}

        virtual void OnFocus()
        {}

        virtual void OnDefocus()
        {}

        virtual void Setup()
        {}

        virtual void Update(float t_dT) = 0;
        virtual void Draw(sf::RenderTarget* t_target) = 0;

        virtual void UpdateStyle(const GUIElementState& t_state, const GUIStyle& t_style);
        virtual void ApplyStyle();
        virtual sf::Vector2f GetContentSize() const;

        GUIElementType GetType() const;

        std::string GetName() const;
        std::string GetFileName() const;
        void SetName(const std::string& t_name);
        void SetFileName(const std::string& t_fileName);
        const sf::Vector2f& GetPosition() const;
        void SetPosition(const sf::Vector2f& t_pos);
        sf::Vector2f GetSize() const;
        GUIElementState GetState() const;
        void SetState(const GUIElementState& t_state);
        void SetRedraw(const bool& t_redraw);
        bool NeedsRedraw() const;
        void SetOwner(GUIInterface* t_owner);
        GUIInterface* GetOwner() const;
        bool HasOwner() const;
        bool IsActive() const;
        void SetActive(const bool& t_active);
        bool IsInside(const sf::Vector2f& t_point) const;
        sf::Vector2f GetGlobalPosition() const;
        bool IsControl() const;
        void SetControl(bool t_control);
        std::string GetText() const;
        void SetText(const std::string& t_text);
        std::string GetStyleName() const;

        friend std::stringstream& operator >>( std::stringstream& t_stream, GUIElement& b)
        {
            b.ReadIn(t_stream);
            return t_stream;
        }

        protected:
        void ApplyTextStyle();
        void ApplyBgStyle();
        void ApplyGlyphStyle();

        void RequireTexture(const std::string& t_name);
        void RequireFont(const std::string& t_name);
        void ReleaseTexture(const std::string& t_name);
        void ReleaseFont(const std::string& t_name);
        void ReleaseResources();
        std::string m_name;
        std::string m_fileName;
        sf::Vector2f m_position;
        ElementStyles m_style; // Style of drawables.
        GUIVisual m_visual; // Drawable bits.
        GUIElementType m_type;
        GUIElementState m_state;
        GUIInterface* m_owner;

        bool m_needsRedraw;
        bool m_active;
        bool m_isControl;
    };
}