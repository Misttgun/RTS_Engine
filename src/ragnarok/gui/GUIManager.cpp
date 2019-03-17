#include <iostream>
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/gui/elements/GUILabel.h"
#include "ragnarok/gui/elements/GUIScrollbar.h"
#include "ragnarok/gui/elements/GUITextField.h"
#include "ragnarok/gui/elements/GUICheckBox.h"
#include "ragnarok/gui/elements/GUISprite.h"
#include "ragnarok/SharedContext.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/utilities/Utilities.h"

namespace ragnarok
{
    GUIManager::GUIManager(EventManager* t_evMgr, SharedContext* t_shared) : m_eventMgr(t_evMgr), m_context(t_shared)
    {
        RegisterElement<GUILabel>(GUIElementType::Label);
        RegisterElement<GUIScrollbar>(GUIElementType::Scrollbar);
        RegisterElement<GUITextfield>(GUIElementType::Textfield);
        RegisterElement<GUICheckBox>(GUIElementType::CheckBox);
        RegisterElement<GUISprite>(GUIElementType::Sprite);
        RegisterElement<GUIDropDownMenu>(GUIElementType::DropDownMenu);
        m_elemTypes.emplace("Label", GUIElementType::Label);
        m_elemTypes.emplace("Button", GUIElementType::Button);
        m_elemTypes.emplace("Scrollbar", GUIElementType::Scrollbar);
        m_elemTypes.emplace("TextField", GUIElementType::Textfield);
        m_elemTypes.emplace("Interface", GUIElementType::Window);
        m_elemTypes.emplace("CheckBox", GUIElementType::CheckBox);
        m_elemTypes.emplace("Sprite", GUIElementType::Sprite);
        m_elemTypes.emplace("DropDownMenu", GUIElementType::DropDownMenu);

        m_eventMgr->AddCallback(StateType(0), "Mouse_Left", &GUIManager::HandleClick, this);
        m_eventMgr->AddCallback(StateType(0), "Mouse_Left_Release", &GUIManager::HandleRelease, this);
        m_eventMgr->AddCallback(StateType(0), "Mouse_Right", &GUIManager::HandleClick, this);
        m_eventMgr->AddCallback(StateType(0), "Mouse_Right_Release", &GUIManager::HandleRelease, this);
        m_eventMgr->AddCallback(StateType(0), "Text_Entered", &GUIManager::HandleTextEntered, this);
        m_eventMgr->AddCallback(StateType(0), "Key_LeftArrow", &GUIManager::HandleArrowKeys, this);
        m_eventMgr->AddCallback(StateType(0), "Key_RightArrow", &GUIManager::HandleArrowKeys, this);
    }

    GUIManager::~GUIManager()
    {
        m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left");
        m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left_Release");
        m_eventMgr->RemoveCallback(StateType(0), "Text_Entered");
        m_eventMgr->RemoveCallback(StateType(0), "Key_LeftArrow");
        m_eventMgr->RemoveCallback(StateType(0), "Key_RightArrow");
    }

    GUIInterface* GUIManager::GetInterface(const StateType& t_state, const std::string& t_name)
    {
        auto s = m_interfaces.find(t_state);

        if(s == m_interfaces.end())
        {
            return nullptr;
        }

        auto i = std::find_if(s->second.begin(), s->second.end(), [&t_name](GUIInterfaceType& t_pair) { return t_pair.first == t_name; });
        
        return (i != s->second.end() ? i->second.get() : nullptr);
    }

    GUIInterface* GUIManager::GetInterface(const std::string& t_name)
    {
        return GetInterface(m_currentState, t_name);
    }

    bool GUIManager::RemoveInterface(const StateType& t_state, const std::string& t_name)
    {
        auto s = m_interfaces.find(t_state);
        
        if(s == m_interfaces.end())
        {
            return false;
        }
        
        auto i = std::remove_if(s->second.begin(), s->second.end(),  [&t_name](GUIInterfaceType& t_pair)
        {
            return t_pair.first == t_name;
        });

        if(i == s->second.end())
        {
            return false;
        }

        s->second.erase(i);
        return true;
    }

    bool GUIManager::RemoveInterface(const std::string& t_name)
    {
        return RemoveInterface(m_currentState, t_name);
    }

    bool GUIManager::BringToFront(const GUIInterface* t_interface)
    {
        return BringToFront(m_currentState, t_interface->GetName());
    }

    bool GUIManager::BringToFront(const StateType& t_state, const std::string& t_name)
    {
        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return false;
        }

        auto i = std::find_if(state->second.begin(), state->second.end(), [&t_name](GUIInterfaceType& t_pair) { return t_pair.first == t_name; });
        
        if(i == state->second.end())
        {
            return false;
        }

        m_toFront.emplace_back(t_state, i->second.get());
        return true;
    }

    void GUIManager::ChangeState(const StateType& t_state)
    {
        if(m_currentState == t_state)
        {
            return;
        }

        HandleRelease(nullptr);
        SetState(t_state);
    }

    void GUIManager::RemoveState(const StateType& t_state)
    {
        m_events.erase(t_state);
        m_interfaces.erase(t_state);
    }

    SharedContext* GUIManager::GetContext() const
    {
        return m_context;
    }

    void GUIManager::DefocusAllInterfaces()
    {
        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return;
        }

        for(auto &itr : state->second)
        {
            itr.second->Defocus();
        }
    }

    void GUIManager::HandleClick(EventDetails* t_details)
    {
        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return;
        }

        sf::Vector2i mousePos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
        for(auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr)
        {
            auto i = itr->second.get();
            if(!i->IsInside(sf::Vector2f(mousePos)))
            {
                continue;
            }

            if(!i->IsActive())
            {
                continue;
            }

            if(t_details->m_keyCode == static_cast<int>(MouseButtonType::Left))
            {
                DefocusAllInterfaces();
                i->Focus();
                i->OnClick(sf::Vector2f(mousePos));

                if(i->IsBeingMoved())
                {
                    i->BeginMoving();
                }
            }
            
            t_details->m_hasBeenProcessed = true;
            return;
        }

        state->second.back().second->Defocus();
    }

    void GUIManager::HandleRelease(EventDetails* t_details)
    {
        if(t_details && t_details->m_keyCode != static_cast<int>(MouseButtonType::Left))
        {
            return;
        }

        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return;
        }

        for(auto &itr : state->second)
        {
            auto& i = itr.second;
            if(!i->IsActive())
            {
                continue;
            }

            if(i->GetState() == GUIElementState::Clicked)
            {
                i->OnRelease();
            }

            if(i->IsBeingMoved())
            {
                i->StopMoving();
            }
        }
    }

    void GUIManager::HandleTextEntered(EventDetails* t_details)
    {
        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return;
        }

        for(auto &itr : state->second)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            if(!itr.second->IsFocused())
            {
                continue;
            }

            itr.second->OnTextEntered(t_details->m_textEntered);

            return;
        }
    }

    void GUIManager::HandleArrowKeys(EventDetails* t_details)
    {
        auto state = m_interfaces.find(m_currentState);
        
        if(state == m_interfaces.end())
        {
            return;
        }

        for(auto &itr : state->second)
        {
            if(!itr.second->IsActive())
            {
                continue;
            }

            if(!itr.second->IsFocused())
            {
                continue;
            }

            itr.second->OnArrowKey(t_details->m_name);

            return;
        }
    }

    void GUIManager::AddEvent(GUIEvent t_event)
    {
        m_events[m_currentState].push_back(t_event);
    }

    bool GUIManager::PollEvent(GUIEvent& t_event)
    {
        if(m_events[m_currentState].empty())
        {
            return false;
        }

        t_event = m_events[m_currentState].back();
        m_events[m_currentState].pop_back();

        return true;
    }

    void GUIManager::Update(float t_dT)
    {
        sf::Vector2i mousePos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
        HandleMovesToFront();
        auto state = m_interfaces.find(m_currentState);

        if(state == m_interfaces.end())
        {
            return;
        }

        bool HoverCaptured = false;

        for(auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr)
        {
            auto& i = itr->second;

            if(!i->IsActive())
            {
                continue;
            }

            i->Update(t_dT);

            if(i->IsBeingMoved())
            {
                continue;
            }

            if(i->IsInside(sf::Vector2f(mousePos)) && !HoverCaptured)
            {
                if(i->GetState() == GUIElementState::Neutral)
                {
                    i->OnHover(sf::Vector2f(mousePos));
                }

                HoverCaptured = true;
                continue;
            }
            else if(i->GetState() == GUIElementState::Focused)
            {
                i->OnLeave();
            }
        }
    }

    void GUIManager::Render(sf::RenderWindow* t_wind)
    {
        auto state = m_interfaces.find(m_currentState);

        if(state == m_interfaces.end())
        {
            return;
        }

        sf::View CurrentView = t_wind->getView();
        t_wind->setView(t_wind->getDefaultView());

        for(auto &itr : state->second)
        {
            auto& i = itr.second;
            if(!i->IsActive())
            {
                continue;
            }

            if(i->NeedsRedraw())
            {
                i->Redraw();
            }

            if(i->NeedsContentRedraw())
            {
                i->RedrawContent();
            }

            if(i->NeedsControlRedraw())
            {
                i->RedrawControls();
            }

            i->Draw(t_wind);
        }

        t_wind->setView(CurrentView);
    }

    void GUIManager::HandleMovesToFront()
    {
        for(auto& itr : m_toFront)
        {
            auto state = m_interfaces.find(m_currentState);

            if(state == m_interfaces.end())
            {
                continue;
            }

            auto& list = state->second;
            auto i = std::find_if(list.begin(), list.end(), [&itr](const GUIInterfaceType& t_pair) { return itr.second == t_pair.second.get(); });
            
            if(i == list.end())
            {
                continue;
            }

            auto name = i->first;
            auto ptr = std::move(i->second);
            list.erase(i);
            list.emplace_back(name, std::move(ptr));
        }
    }

    GUIElement* GUIManager::CreateElement(const GUIElementType& t_id, GUIInterface* t_owner)
    {
        if(t_id == GUIElementType::Window)
        {
            return new GUIInterface("", this);
        }

        auto f = m_factory.find(t_id);

        return (f != m_factory.end() ? f->second(t_owner) : nullptr);
    }

    bool GUIManager::AddInterface(const StateType& t_state, const std::string& t_name)
    {
        auto s = m_interfaces.emplace(t_state, GUIInterfaces()).first;
        auto i = std::find_if(s->second.begin(), s->second.end(), [&t_name](GUIInterfaceType& t_pair) { return t_pair.first == t_name; });
        
        if(i != s->second.end())
        {
            std::cout << "Interface with the name " << t_name << " already exists!" << std::endl; return false;
        }

        std::unique_ptr<GUIInterface> temp = std::make_unique<GUIInterface>(t_name, this);
        s->second.emplace_back(t_name, std::move(temp));
       
        return true;
    }

    bool GUIManager::AddInterface(const std::string& t_name)
    {
        return AddInterface(m_currentState, t_name);
    }

    bool GUIManager::LoadInterface(const StateType& t_state, const std::string& t_interface, const std::string& t_name)
    {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + "res/GUI_Interfaces/" + t_interface);
        std::string InterfaceName;

        if(!file.is_open())
        {
            std::cout << "! Failed to load: " << t_interface << std::endl;
            return false;
        }

        std::string line;

        while(std::getline(file, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string key;
            keystream >> key;

            if(key == "Interface")
            {
                std::string style;
                keystream >> InterfaceName >> style;

                if(!AddInterface(t_state, t_name))
                {
                    std::cout << "Failed adding interface: " << t_name << std::endl;
                    return false;
                }

                auto i = GetInterface(t_state, t_name);
                i->SetFileName(InterfaceName);
                keystream >> *i;

                if(!LoadStyle(style, i))
                {
                    std::cout << "Failed loading style file: " << style << " for interface " << t_name << std::endl;
                }

                i->SetContentSize(i->GetSize());
                i->Setup();
            }
            else if(key == "Element")
            {
                if(InterfaceName.empty())
                {
                    std::cout << "Error: 'Element' outside or before declaration of 'Interface'!" << std::endl;
                    continue;
                }

                std::string type;
                std::string name;
                sf::Vector2f position;
                std::string style;
                std::string layer;
                keystream >> type >> name >> position.x >> position.y >> style >> layer;
                GUIElementType eType = StringToType(type);

                if(eType == GUIElementType::None)
                {
                    std::cout << "Unknown element('" << name << "') type: '" << type << "'" << std::endl;
                    continue;
                }

                auto i = GetInterface(t_state, t_name);

                if(!i)
                {
                    continue;
                }

                if(!i->AddElement(eType, name))
                {
                    continue;
                }

                auto e = i->GetElement(name);
                keystream >> *e;
                e->SetPosition(position);
                e->SetControl((layer == "Control"));

                if(!LoadStyle(style, e))
                {
                    std::cout << "Failed loading style file: " << style << " for element " << name << std::endl;
                }

                e->Setup();
            }
        }

        file.close();

        return true;
    }

    bool GUIManager::LoadInterface(const std::string& t_interface, const std::string& t_name)
    {
        return LoadInterface(m_currentState, t_interface, t_name);
    }

    bool GUIManager::LoadInterface(GUIInterface* t_interface, const std::string& t_fileName)
    {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + "res/GUI_Interfaces/" + t_fileName);
        std::string InterfaceName;

        if(!file.is_open())
        {
            std::cout << "! Failed to load: " << t_interface << std::endl;
            return false;
        }

        std::string line;

        while(std::getline(file, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string key;
            keystream >> key;

            if(key == "Interface")
            {
                std::string style;
                keystream >> InterfaceName >> style;
                t_interface->SetFileName(InterfaceName);
                keystream >> *t_interface;

                if(!LoadStyle(style, t_interface))
                {
                    std::cout << "Failed loading style file: " << style << " for interface " << t_fileName << std::endl;
                }

                t_interface->SetContentSize(t_interface->GetSize());
            }
            else if(key == "Element")
            {
                if(InterfaceName.empty())
                {
                    std::cout << "Error: 'Element' outside or before declaration of 'Interface'!" << std::endl;
                    continue;
                }

                std::string type;
                std::string name;
                sf::Vector2f position;
                std::string style;
                std::string layer;
                keystream >> type >> name >> position.x >> position.y >> style >> layer;
                GUIElementType eType = StringToType(type);

                if(eType == GUIElementType::None)
                {
                    std::cout << "Unknown element('" << name << "') type: '" << type << "'" << std::endl;
                    continue;
                }

                if(!t_interface->AddElement(eType, name))
                {
                    continue;
                }

                auto e = t_interface->GetElement(name);
                keystream >> *e;
                e->SetPosition(position);
                e->SetControl((layer == "Control"));

                if(!LoadStyle(style, e))
                {
                    std::cout << "Failed loading style file: " << style << " for element " << name << std::endl;
                    continue;
                }
            }
        }

        file.close();

        return true;
    }

    bool GUIManager::LoadStyle(const std::string& t_file, GUIElement* t_element)
    {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + "res/GUI_Styles/" + t_file);

        std::string currentState;
        GUIStyle ParentStyle;
        GUIStyle TemporaryStyle;

        if(!file.is_open())
        {
            std::cout << "! Failed to load: " << t_file << std::endl;
            return false;
        }

        ParentStyle.m_styleName = t_file;
        TemporaryStyle.m_styleName = t_file;
        std::string line;

        while(std::getline(file, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;

            if(type.empty())
            {
                continue;
            }

            if(type == "State")
            {
                if(!currentState.empty())
                {
                    std::cout << "Error: 'State' keyword found inside another state!" << std::endl;
                    continue;
                }

                keystream >> currentState;
            }
            else if(type == "/State")
            {
                if(currentState.empty())
                {
                    std::cout << "Error: '/State' keyword found prior to 'State'!" << std::endl;
                    continue;
                }

                GUIElementState state = GUIElementState::Neutral;

                if(currentState == "Hover")
                {
                    state = GUIElementState::Focused;
                }
                else if(currentState == "Clicked")
                {
                    state = GUIElementState::Clicked;
                }

                if(state == GUIElementState::Neutral)
                {
                    ParentStyle = TemporaryStyle;
                    t_element->UpdateStyle(GUIElementState::Neutral, TemporaryStyle);
                    t_element->UpdateStyle(GUIElementState::Focused, TemporaryStyle);
                    t_element->UpdateStyle(GUIElementState::Clicked, TemporaryStyle);
                }
                else
                {
                    t_element->UpdateStyle(state, TemporaryStyle);
                }

                TemporaryStyle = ParentStyle;
                currentState.clear();
            }
            else
            {
                // Handling style information.
                if(currentState.empty())
                {
                    std::cout << "Error: '" << type << "' keyword found outside of a state!" << std::endl;
                    continue;
                }

                if(type == "Size")
                {
                    keystream >> TemporaryStyle.m_size.x >> TemporaryStyle.m_size.y;
                }
                else if(type == "BgColor")
                {
                    int r, g, b, a = 0;
                    keystream >> r >> g >> b >> a;
                    TemporaryStyle.m_backgroundColor = sf::Color(r, g, b, a);
                }
                else if(type == "BgImage")
                {
                    keystream >> TemporaryStyle.m_backgroundImage;
                }
                else if(type == "BgImageColor")
                {
                    int r, g, b, a = 0;
                    keystream >> r >> g >> b >> a;
                    TemporaryStyle.m_backgroundImageColor = sf::Color(r, g, b, a);
                }
                else if(type == "TextColor")
                {
                    int r, g, b, a = 0;
                    keystream >> r >> g >> b >> a;
                    TemporaryStyle.m_textColor = sf::Color(r, g, b, a);
                }
                else if(type == "TextSize")
                {
                    keystream >> TemporaryStyle.m_textSize;
                }
                else if(type == "TextOriginCenter")
                {
                    TemporaryStyle.m_textCenterOrigin = true;
                }
                else if(type == "StretchToFit")
                {
                    TemporaryStyle.m_stretchToFit = true;
                }
                else if(type == "Font")
                {
                    keystream >> TemporaryStyle.m_textFont;
                }
                else if(type == "TextPadding")
                {
                    keystream >> TemporaryStyle.m_textPadding.x >> TemporaryStyle.m_textPadding.y;
                }
                else if(type == "ElementColor")
                {
                    int r, g, b, a = 0;
                    keystream >> r >> g >> b >> a;
                    TemporaryStyle.m_elementColor = sf::Color(r, g, b, a);
                }
                else if(type == "Glyph")
                {
                    keystream >> TemporaryStyle.m_glyph;
                }
                else if(type == "GlyphPadding")
                {
                    keystream >> TemporaryStyle.m_glyphPadding.x >> TemporaryStyle.m_glyphPadding.y;
                }
                else
                {
                    std::cout << "Error: style tag '" << type << "' is unknown!" << std::endl;
                }
            }
        }

        file.close();

        return true;
    }

    GUIElementType GUIManager::StringToType(const std::string& t_string) const
    {
        auto t = m_elemTypes.find(t_string);
        return (t != m_elemTypes.end() ? t->second : GUIElementType::None);
    }
}
