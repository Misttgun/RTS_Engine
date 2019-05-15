#include <iostream>
#include "../../include/gui/GUIManager.h"
#include "../../include/gui/elements/GUILabel.h"
#include "../../include/gui/elements/GUIScrollbar.h"
#include "../../include/gui/elements/GUITextField.h"
#include "../../include/gui/elements/GUICheckBox.h"
#include "../../include/gui/elements/GUISprite.h"
#include "../../include/SharedContext.h"
#include "../../include/window/Window.h"
#include "../../include/utilities/Utilities.h"

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

    /**
     * Tries getting interface with passed name for passed state
     * @param t_state The state for which the interface was registered
     * @param t_name The name of the interface to search for
     * @return The matching interface if found, otherwise nullptr
     */
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

    /**
     * Tries removing interface with passed name for passed state
     * @param t_state The state for which the interface was registered
     * @param t_name The name of the interface to remove
     * @return True if a matching interface was removed, false otherwise
     */
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

    /**
     * Finds an interface by name and puts it to the front for passed state
     * @param t_state The state for which to put the interface to the front
     * @param t_name The name of the interface to put to the front
     * @return True on success, false if no interface with passed name was found
     */
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

    /**
     * Calls HandleRelease and then sets state
     *
     * If passed state is equal to current one, do noting. Else, calls
     * HandleRelease before setting the new state.
     * @param t_state
     */
    void GUIManager::ChangeState(const StateType& t_state)
    {
        if(m_currentState == t_state)
        {
            return;
        }

        HandleRelease(nullptr);
        SetState(t_state);
    }

    /**
     * Removes all events and interfaces for passed state
     * @param t_state The state for which to empty event and interface lists
     */
    void GUIManager::RemoveState(const StateType& t_state)
    {
        m_events.erase(t_state);
        m_interfaces.erase(t_state);
    }

    SharedContext* GUIManager::GetContext() const
    {
        return m_context;
    }

    /**
     * Calls Defocus on all current state's interfaces
     */
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

    /**
     * Focuses clicked interface and calls its click handler
     *
     * Tries to find an active interface with the mouse inside of it, focuses it
     * and then calls its click handler method (and BeginMoving, if being moved
     * after that). Then, sets event's processed flag and returns.
     * If no such interface was found, defocuses last interface.
     * @param t_details The event details
     */
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

    /**
     * Calls clicked interface's release handler
     *
     * Tries to find an active interface with state "Clicked" and calls its
     * release handler method (and StopMoving, if being moved after that).
     * @param t_details The event details
     */
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

    /**
     * Calls focused interface's text entered handler
     *
     * Tries to find an active and focused interface and calls its text entered
     * handler method with the event's entered text.
     * @param t_details The event details
     */
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

    /**
     * Calls focused interface's arrow key handler
     *
     * Tries to find an active and focused interface and calls its arrow key
     * handler method with the event's arrow key.
     * @param t_details The event details
     */
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

    /**
     * Adds passed event to current state's event list
     * @param t_event The event to add to the event list
     */
    void GUIManager::AddEvent(GUIEvent t_event)
    {
        m_events[m_currentState].push_back(t_event);
    }

    /**
     * Pops an event from current state's event list
     * @param t_event The pointer for storing the popped event
     * @return True if an event was popped, false if the event list was empty
     */
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

    /**
     * Updates current state's active interfaces, calls hover / leave handler
     *
     * First calls HandleMovesToFront, then updates current state's active
     * interfaces and for each interface, if the cursor is inside, calls its
     * hover handler method as needed, else calls its leave handler method as
     * needed.
     * @param t_dT The delta time since last update
     */
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

    /**
     * Draws all current state's active interfaces to passed render window
     *
     * For each current state's active interface, calls its redraw methods as
     * needed and then calls its draw method with passed render window.
     * @param t_wind The render window to use for rendering
     */
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

    /**
     * Instanciates a new element of passed type with passed owner
     * @param t_id The type of the element to create
     * @param t_owner The owner of the element to create
     * @return The newly created GUI element
     */
    GUIElement* GUIManager::CreateElement(const GUIElementType& t_id, GUIInterface* t_owner)
    {
        if(t_id == GUIElementType::Window)
        {
            return new GUIInterface("", this);
        }

        auto f = m_factory.find(t_id);

        return (f != m_factory.end() ? f->second(t_owner) : nullptr);
    }

    /**
     * Tries adding a new interface with passed name for passed state
     *
     * First ensures that a list of interfaces exists for passed state, then
     * tries to find an interface with passed name in that list. If no such
     * interface exists, instantiates and inserts a new GUI interface into the
     * list.
     * @param t_state The state for which to add a new interface
     * @param t_name The name of the new interface
     * @return True if a new interface was created, false otherwise
     */
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

    /**
     * Tries adding a new interface with passed name for current state
     * @param t_name The name of the new interface
     * @return True if a new interface was created, false otherwise
     */
    bool GUIManager::AddInterface(const std::string& t_name)
    {
        return AddInterface(m_currentState, t_name);
    }

    /**
     * Loads an interface from an interface file
     * @param t_state The state for which the interface should be added
     * @param t_interface The name of the file containing interface information
     * @param t_name The name to give to the newly loaded interface
     * @return True on success, false on failure
     */
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

    /**
     * Loads an interface from an interface file, for current state
     * @param t_interface The name of the file containing interface information
     * @param t_name The name to give to the newly loaded interface
     * @return True on success, false on failure
     */
    bool GUIManager::LoadInterface(const std::string& t_interface, const std::string& t_name)
    {
        return LoadInterface(m_currentState, t_interface, t_name);
    }

    /**
     * Loads an interface from an interface file, without adding it to any list
     * @param t_interface A pointer to store the newly loaded interface
     * @param t_fileName The name of the file containing interface information
     * @return True on success, false on failure
     */
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
                    std::cerr << "Couldn't add element " << name << " to interface " << t_interface->GetName();
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

    /**
     * Loads a style from style file with passed file name, for passed element
     * @param t_file The name of the style file from which to load the style
     * @param t_element The element on which to apply the loaded style
     * @return True on success, false on failure
     */
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
