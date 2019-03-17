#include <ragnarok/utilities/Utilities.h>
#include <iostream>
#include "ragnarok/gui/specialized/FileManager.h"
#include "ragnarok/gui/GUIManager.h"
#include "ragnarok/states/StateManager.h"
#include "ragnarok/utilities/Helpers.h"
#include "ragnarok/events/EventManager.h"

namespace ragnarok
{
    GUIFileManager::GUIFileManager(std::string t_name, GUIManager* t_guiMgr, StateManager* t_stateMgr)
        : m_guiManager(t_guiMgr), m_stateMgr(t_stateMgr), m_name(t_name), m_saveMode(false)
    {
        m_guiManager->LoadInterface("FileManager.interface", t_name);
        m_interface = m_guiManager->GetInterface(t_name);
        m_currentState = m_stateMgr->GetCurrentStateType();
        m_folderEntry = m_interface->GetElement("FolderEntry")->GetStyleName();
        m_fileEntry = m_interface->GetElement("FileEntry")->GetStyleName();
        m_interface->RemoveElement("FolderEntry");
        m_interface->RemoveElement("FileEntry");
        m_interface->SetContentRectSize({300, 260});
        m_interface->SetContentOffset({0.f, 16.f});
        m_interface->PositionCenterScreen();

        auto e = m_stateMgr->GetContext()->m_eventManager;

        e->AddCallback<GUIFileManager>("FileManager_Parent", &GUIFileManager::ParentDirCallback, this);
        e->AddCallback<GUIFileManager>("FileManager_Entries", &GUIFileManager::HandleEntries, this);
        e->AddCallback<GUIFileManager>("FileManager_ActionButton", &GUIFileManager::ActionButton, this);
        e->AddCallback<GUIFileManager>("FileManager_Close", &GUIFileManager::CloseButton, this);

        SetDirectory(Utils::GetWorkingDirectory());
    }

    GUIFileManager::~GUIFileManager()
    {
        m_guiManager->RemoveInterface(m_currentState, m_name);
        auto events = m_stateMgr->GetContext()->m_eventManager;
        events->RemoveCallback(m_currentState, "FileManager_Parent");
        events->RemoveCallback(m_currentState, "FileManager_Entries");
        events->RemoveCallback(m_currentState, "FileManager_ActionButton");
        events->RemoveCallback(m_currentState, "FileManager_Close");
    }

    void GUIFileManager::HandleEntries(EventDetails* t_details)
    {
        if(t_details->m_guiElement.find("FEntry_") != std::string::npos)
        {
            std::string path = m_dir + m_interface->GetElement(t_details->m_guiElement)->GetText() + "\\";
            SetDirectory(path);
            m_interface->UpdateScrollVertical(0);
        }
        else if(t_details->m_guiElement.find("Entry_") != std::string::npos)
        {
            m_interface->GetElement("FileName")->SetText(m_interface->GetElement(t_details->m_guiElement)->GetText());
        }
    }

    void GUIFileManager::ActionButton(EventDetails* t_details)
    {
        if(m_actionCallback == nullptr)
        {
            std::cout << "Action callback for file manager was not bound!" << std::endl; return;
        }

        auto filename = m_interface->GetElement("FileName")->GetText();
        m_actionCallback(m_dir + filename);
    }

    void GUIFileManager::CloseButton(EventDetails* t_details)
    {
        Hide();
    }

    void GUIFileManager::SetDirectory(std::string t_dir)
    {
        m_dir = t_dir;
        std::replace(m_dir.begin(), m_dir.end(), '\\', '/');
        m_interface->RemoveElementsContaining("Entry_");
        ListFiles();
    }

    void GUIFileManager::ListFiles()
    {
        m_interface->GetElement("Directory")->SetText(m_dir);
        auto list = Utils::GetFileList(m_dir, "*.*", true);
        Utils::SortFileList(list);
        auto ParentDir = m_interface->GetElement("ParentDir");
        float x = ParentDir->GetPosition().x;
        float y = ParentDir->GetPosition().y + ParentDir->GetSize().y + 1.f;
        size_t i = 0;

        for(auto& file : list)
        {
            if(file.first == "." || file.first == "..")
            {
                continue;
            }

            std::string entry = (file.second ? "FEntry_" : "Entry_");
            m_interface->AddElement(GUIElementType::Label, entry + std::to_string(i));
            auto element = m_interface->GetElement(entry + std::to_string(i));
            element->SetText(file.first);
            element->SetPosition({x, y});
            m_guiManager->LoadStyle((file.second ? m_folderEntry : m_fileEntry), element);
            y += ParentDir->GetSize().y + 4.f;
            ++i;
        }
    }

    void GUIFileManager::ParentDirCallback(EventDetails* t_details)
    {
        auto i = m_dir.find_last_of("/", m_dir.length() - 2);

        if(i != std::string::npos)
        {
            std::string dir = m_dir.substr(0U, i + 1);
            SetDirectory(dir);
        }
    }

    void GUIFileManager::Hide()
    {
        m_interface->SetActive(false);
    }

    void GUIFileManager::Show()
    {
        m_interface->SetActive(true);
        m_interface->PositionCenterScreen();
        ListFiles();
        m_interface->Focus();
    }

    bool GUIFileManager::IsInSaveMode() const
    {
        return m_saveMode;
    }

    void GUIFileManager::LoadMode()
    {
        m_interface->GetElement("ActionButton")->SetText("Load"); m_saveMode = false;
    }

    void GUIFileManager::SaveMode()
    {
        m_interface->GetElement("ActionButton")->SetText("Save"); m_saveMode = true;
    }
}