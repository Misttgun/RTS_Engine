#pragma once

#include <string>
#include <functional>

namespace ragnarok
{
    class GUIManager;
    class GUIInterface;
    class StateManager;
    struct EventDetails;
    enum class StateType;

    /* Class responsible for displaying directory information.
    Useful for loading and saving maps.*/
    class GUIFileManager
    {
        public:
        GUIFileManager(const std::string& t_name, GUIManager* t_guiMgr, StateManager* t_stateMgr);
        ~GUIFileManager();

        // Changes the current directory
        void SetDirectory(std::string t_dir);

        // Handle backward-traversal logic
        void ParentDirCallback(EventDetails* t_details);
        // Logic for when we click on an entrie
        void HandleEntries(EventDetails* t_details);
        // Handle the button press action
        void ActionButton(EventDetails* t_details);
        void CloseButton(EventDetails* t_details);

        void Hide() const;
        void Show();
        void LoadMode();
        void SaveMode();

        bool IsInSaveMode() const;

        template<class T>
        void SetActionCallback(void(T::*t_method)(const std::string&), T* t_instance)
        {
            // Same with event manager, add a fuction as call back
            m_actionCallback = [t_instance, t_method](const std::string& t_str) -> void { (t_instance->*t_method)(t_str); };
        }

        private:
        // List all the files and directories in the current directory
        void ListFiles();

        GUIInterface* m_interface;
        std::string m_name;
        std::string m_dir;

        std::string m_folderEntry;
        std::string m_fileEntry;

        GUIManager* m_guiManager;
        StateManager* m_stateMgr;
        StateType m_currentState;
        std::function<void(std::string)> m_actionCallback;
        bool m_saveMode;
    };
}