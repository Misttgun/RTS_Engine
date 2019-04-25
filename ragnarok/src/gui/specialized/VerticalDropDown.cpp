#include "../../../include/gui/specialized/VerticalDropDown.h"
#include "../../../include/gui/GUIManager.h"

namespace ragnarok
{
    GUIVerticalDropDown::GUIVerticalDropDown(EventManager* t_eventManager, GUIManager* t_guiManager, GUIElement* t_element)
        : m_eventManager(t_eventManager), m_guiManager(t_guiManager), m_interface(nullptr), m_element(t_element)
    {}

    GUIVerticalDropDown::~GUIVerticalDropDown()
    {
        m_eventManager->RemoveCallback(m_state, m_callbackNameBase + "Click");
        m_eventManager->RemoveCallback(m_state, m_callbackNameBase + "Leave");

        if(m_interface)
        {
            m_guiManager->RemoveInterface(m_state, m_name);
        }
    }

    void GUIVerticalDropDown::Setup(const StateType& t_state, const std::string& t_name, const std::string& t_fileName)
    {
        if(m_interface)
        {
            return;
        }

        m_state = t_state; m_name = t_name;

        if(!m_guiManager->LoadInterface(m_state, t_fileName, m_name))
        {
            return;
        }

        m_interface = m_guiManager->GetInterface(m_state, m_name);
        m_interface->SetActive(false);
        m_entryStyle = m_interface->GetElement("Entry_")->GetStyleName();

        m_callbackNameBase = "_callback" + m_name;

        std::unique_ptr<Binding> bind;
        GUIEvent event;
        event.m_interface = m_name;
        event.m_element = "*";
        bind = std::make_unique<Binding>(m_callbackNameBase + "Click");
        event.m_type = GUIEventType::Click;
        EventInfo info(event);
        bind->BindEvent(EventType::GUI_Click, info);
        m_eventManager->AddBinding(std::move(bind));
        m_eventManager->AddCallback(m_state, m_callbackNameBase + "Click", &GUIVerticalDropDown::OnClick, this);
    }

    void GUIVerticalDropDown::SetPosition(const sf::Vector2f& t_position)
    {
        if(!m_interface)
        {
            return;
        }

        m_interface->SetPosition(t_position);
    }

    void GUIVerticalDropDown::Show()
    {
        if(!m_interface)
        {
            return;
        }

        m_interface->SetActive(true);
        m_interface->Focus();
    }

    void GUIVerticalDropDown::BringToFront()
    {
        if(!m_interface)
        {
            return;
        }

        m_guiManager->BringToFront(m_interface);
    }

    void GUIVerticalDropDown::Hide()
    {
        if(!m_interface)
        {
            return;
        }

        m_interface->SetActive(false);

        if(!m_element)
        {
            return;
        }

        m_element->SetState(GUIElementState::Neutral);
    }

    std::string GUIVerticalDropDown::GetSelected() const
    {
        return m_selection;
    }

    void GUIVerticalDropDown::ResetSelected()
    {
        m_selection = "";
        if(!m_element)
        {
            return;
        }

        m_element->SetText("Select...");
    }

    void GUIVerticalDropDown::AddEntry(const std::string& t_entry)
    {
        m_entries.emplace_back(t_entry);
    }

    void GUIVerticalDropDown::RemoveEntry(const std::string& t_entry)
    {
        m_entries.erase(std::find_if(m_entries.begin(), m_entries.end(), [&t_entry](const std::string& t_str) { return t_entry == t_str; }));
    }

    void GUIVerticalDropDown::PurgeEntries()
    {
        m_entries.clear();
    }

    void GUIVerticalDropDown::OnClick(EventDetails* t_details)
    {
        if(t_details->m_guiElement == "")
        {
            return;
        }

        m_selection = m_interface->GetElement(t_details->m_guiElement)->GetText();
        Hide();

        if(!m_element)
        {
            return;
        }

        m_element->SetText(m_selection);
        m_element->SetRedraw(true);
    }

    void GUIVerticalDropDown::Redraw()
    {
        if(!m_interface)
        {
            return;
        }

        m_interface->RemoveElementsContaining("Entry_");
        size_t id = 0;
        auto position = sf::Vector2f(0, 0);

        for(auto& entry : m_entries)
        {
            m_interface->AddElement(GUIElementType::Label, "Entry_" + std::to_string(id));
            auto element = m_interface->GetElement("Entry_" + std::to_string(id));
            element->SetText(entry);
            element->SetPosition(position);
            m_guiManager->LoadStyle(m_entryStyle, element);
            position.y += element->GetSize().y + 1.f;
            ++id;
        }
    }
}
