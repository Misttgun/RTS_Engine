#include "../../include/states/StateManager.h"
#include "../../include/states/StateDependent.h"
#include "../../include/states/StateLoading.h"
#include "../../include/window/Window.h"
#include "../../include/gui/GUIManager.h"
#include "../../include/sounds/SoundManager.h"

namespace ragnarok
{
    StateManager::StateManager(SharedContext* t_shared) : m_shared(t_shared)
    {
        RegisterState<StateLoading>(StateType::Loading);
        CreateState(StateType::Loading);
        m_loading = dynamic_cast<StateLoading*>(m_states.back().second.get());
    }

    StateManager::~StateManager()
    {
        for(auto &itr : m_states)
        {
            itr.second->OnDestroy();
        }
    }

    void StateManager::Update(const sf::Time& t_time)
    {
        if(m_states.empty())
        {
            return;
        }

        // If state is transcendent, we update previous state
        if(m_states.back().second->IsTranscendent() && m_states.size() > 1)
        {
            auto itr = m_states.end();

            while(itr != m_states.begin())
            {
                if(itr != m_states.end())
                {
                    if(!itr->second->IsTranscendent())
                    {
                        break;
                    }
                }

                --itr;
            }

            for(; itr != m_states.end(); ++itr)
            {
                itr->second->Update(t_time);
            }
        }
        else
        {
            m_states.back().second->Update(t_time);
        }
    }

    void StateManager::Draw()
    {
        if(m_states.empty())
        {
            return;
        }

        // If state is transparent, we render previous state
        if(m_states.back().second->IsTransparent() && m_states.size() > 1)
        {
            auto itr = m_states.end();
            while(itr != m_states.begin())
            {
                if(itr != m_states.end())
                {
                    if(!itr->second->IsTransparent())
                    {
                        break;
                    }
                }

                --itr;
            }

            for(; itr != m_states.end(); ++itr)
            {
                m_shared->m_wind->GetRenderWindow()->setView(itr->second->GetView());
                itr->second->Draw();
            }
        }
        else
        {
            m_shared->m_wind->GetRenderWindow()->setView(m_states.back().second->GetView());
            m_states.back().second->Draw();
        }
    }

    SharedContext* StateManager::GetContext() const
    {
        return m_shared;
    }

    bool StateManager::HasState(const StateType& t_type) const
    {
        for (const auto& m_state : m_states)
        {
            if(m_state.first == t_type)
            {
                const auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), t_type);
                return removed == m_toRemove.end();
            }
        }

        return false;
    }

    StateType StateManager::GetCurrentStateType() const
    {
        if(m_states.empty())
        {
            return static_cast<StateType>(0);
        }

        return m_states.back().first;
    }

    StateType StateManager::GetNextToLast() const
    {
        if(m_states.empty())
        {
            return static_cast<StateType>(0);
        }

        if(m_states.size() == 1)
        {
            return m_states.back().first;
        }

        return (m_states.end() - 2)->first;
    }

    void StateManager::ProcessRequests()
    {
        while(m_toRemove.begin() != m_toRemove.end())
        {
            RemoveState(*m_toRemove.begin());
            m_toRemove.erase(m_toRemove.begin());
        }
    }

    void StateManager::SetGlobalState(const StateType& t_type)
    {
        for(auto& dependent : m_dependents)
        {
            dependent->ChangeState(t_type);
        }
    }

    void StateManager::SwitchTo(const StateType& t_type)
    {
        if(!m_states.empty() && m_states.back().first == t_type)
        {
            return;
        }

        SetGlobalState(t_type);

        for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
        {
            if(itr->first != t_type)
            {
                continue;
            }

            m_states.back().second->Deactivate();
            StateType tmp_type = itr->first;
            std::unique_ptr<BaseState> tmp_state = std::move(itr->second);
            m_states.erase(itr);
            tmp_state->Activate();
            auto& view = tmp_state->GetView();
            m_states.emplace_back(tmp_type, std::move(tmp_state));
            m_shared->m_wind->GetRenderWindow()->setView(view);

            if(t_type == StateType::Loading)
            {
                return;
            }

            if(m_loading->HasWork())
            {
                SwitchTo(StateType::Loading);
            }

            return;
        }

        // State with t_type wasn't found.
        if(!m_states.empty())
        {
            m_states.back().second->Deactivate();
        }

        CreateState(t_type);
        m_states.back().second->Activate();
        m_shared->m_wind->GetRenderWindow()->setView(m_states.back().second->GetView());

        if(t_type == StateType::Loading)
        {
            return;
        }

        if(m_loading->HasWork())
        {
            SwitchTo(StateType::Loading);
        }
    }

    void StateManager::Remove(const StateType& t_type)
    {
        for (auto& m_state : m_states)
        {
            if(m_state.first != t_type)
            {
                continue;
            }

            m_state.second->OnDestroy();
            m_toRemove.push_back(t_type);

            for(auto& dependent : m_dependents)
            {
                dependent->RemoveState(t_type);
            }
        }
    }

    void StateManager::CreateState(const StateType& t_type)
    {
        const auto newState = m_stateFactory.find(t_type);

        if(newState == m_stateFactory.end())
        {
            return;
        }

        std::unique_ptr<BaseState> state(newState->second());
        state->m_view = m_shared->m_wind->GetRenderWindow()->getDefaultView();
        m_states.emplace_back(t_type, std::move(state));
        m_states.back().second->OnCreate();
    }

    void StateManager::RemoveState(const StateType& t_type)
    {
        for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
        {
            if(itr->first != t_type)
            {
                continue;
            }

            m_states.erase(itr);

            return;
        }
    }

    void StateManager::AddDependent(StateDependent* t_dependent)
    {
        m_dependents.emplace_back(t_dependent);
    }

    void StateManager::RemoveDependent(StateDependent* t_dependent)
    {
        m_dependents.erase(std::find_if(m_dependents.begin(), m_dependents.end(), [t_dependent](StateDependent* t_itr) { return t_itr == t_dependent; }));
    }
}
