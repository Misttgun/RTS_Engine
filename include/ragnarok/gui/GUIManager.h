#pragma once

#include <unordered_map>
#include <functional>
#include <fstream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "GUIInterface.h"
#include "GUIEvent.h"
#include "elements/GUIDropDownMenu.h"
#include "../events/EventManager.h"
#include "../states/StateDependent.h"

namespace ragnarok
{
    using GUIInterfaceType = std::pair<std::string, std::unique_ptr<GUIInterface>>;
    using GUIInterfaces = std::vector<GUIInterfaceType>;
    using GUIContainer = std::unordered_map<StateType, GUIInterfaces>;
    using GUIEvents = std::unordered_map<StateType, std::vector<GUIEvent>>;
    using GUIFactory = std::unordered_map<GUIElementType, std::function<GUIElement*(GUIInterface*)>>;
    using GUIElemTypes = std::unordered_map <std::string, GUIElementType>;

    using GUIBringToFront = std::vector<std::pair<StateType, GUIInterface*>>;

    struct SharedContext;

    class GUIManager : public StateDependent
    {
        friend class GUIInterface;

        public:
        GUIManager(EventManager* t_evMgr, SharedContext* t_shared);
        ~GUIManager();

        bool AddInterface(const StateType& t_state, const std::string& t_name);
        bool AddInterface(const std::string& t_name);
        GUIInterface* GetInterface(const StateType& t_state, const std::string& t_name);
        GUIInterface* GetInterface(const std::string& t_name);
        bool RemoveInterface(const StateType& t_state, const std::string& t_name);
        bool RemoveInterface(const std::string& t_name);

        bool BringToFront(const GUIInterface* t_interface);
        bool BringToFront(const StateType& t_state, const std::string& t_name);

        bool LoadInterface(const StateType& t_state, const std::string& t_interface, const std::string& t_name);
        bool LoadInterface(const std::string& t_interface, const std::string& t_name);
        bool LoadInterface(GUIInterface* t_interface, const std::string& t_fileName);

        void ChangeState(const StateType& t_state);
        void RemoveState(const StateType& t_state);

        SharedContext* GetContext() const;

        void DefocusAllInterfaces();

        void HandleClick(EventDetails* t_details);
        void HandleRelease(EventDetails* t_details);
        void HandleTextEntered(EventDetails* t_details);
        void HandleArrowKeys(EventDetails* t_details);

        void AddEvent(GUIEvent t_event);
        bool PollEvent(GUIEvent& t_event);

        void Update(float t_dT);
        void Render(sf::RenderWindow* t_wind);

        template<class T>
        void RegisterElement(const GUIElementType& t_id)
        {
            m_factory[t_id] = [](GUIInterface* t_owner) -> GUIElement* { return new T("", t_owner); };
        }

        bool LoadStyle(const std::string& t_file, GUIElement* t_element);

        private:
        void HandleMovesToFront();
        GUIElement* CreateElement(const GUIElementType& t_id, GUIInterface* t_owner);
        GUIElementType StringToType(const std::string& t_string) const;

        GUIContainer m_interfaces;
        GUIBringToFront m_toFront;
        GUIEvents m_events;
        SharedContext* m_context;
        EventManager* m_eventMgr;
        GUIFactory m_factory;
        GUIElemTypes m_elemTypes;
    };
}