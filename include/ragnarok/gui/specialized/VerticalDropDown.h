#pragma once

#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "../../states/StateTypes.h"

namespace ragnarok
{
    class EventManager;
    class GUIElement;
    class GUIInterface;
    class GUIManager;
    struct EventDetails;

    using DropDownEntries = std::vector<std::string>;

    class GUIVerticalDropDown
    {
        public:
        GUIVerticalDropDown(EventManager* t_eventManager, GUIManager* t_guiManager, GUIElement* t_element = nullptr);
        ~GUIVerticalDropDown();

        void Setup(const StateType& t_state, const std::string& t_name, const std::string& t_fileName);

        void SetPosition(const sf::Vector2f& t_position);
        void Show();
        void BringToFront();
        void Hide();

        std::string GetSelected() const;
        void ResetSelected();

        void AddEntry(const std::string& t_entry);
        void RemoveEntry(const std::string& t_entry);
        void PurgeEntries();

        void OnClick(EventDetails* t_details);
        void Redraw();

        private:
        GUIInterface * m_interface;
        GUIElement* m_element;
        std::string m_name;
        std::string m_entryStyle;
        StateType m_state;

        DropDownEntries m_entries;

        std::string m_callbackNameBase;

        std::string m_selection;

        GUIManager* m_guiManager;
        EventManager* m_eventManager;
    };
}