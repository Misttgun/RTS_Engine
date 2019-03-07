#include "Window.h"
#include "StateManager.h"
#include "StateDependent.h"
#include "StateLoading.h"

namespace SExE
{
    StateManager::StateManager(SharedContext* t_shared) : m_shared(t_shared)
    {
        registerState<StateLoading>(StateType::Loading);
        createState(StateType::Loading);
        m_loading = static_cast<StateLoading*>(m_states.back().second.get());
    }

    StateManager::~StateManager()
    {
        for(auto &itr : m_states)
        {
            itr.second->onDestroy();
        }
    }

    void StateManager::update(const sf::Time& t_time)
    {
        if(m_states.empty())
        {
            return;
        }

        if(m_states.back().second->isTranscendent() && m_states.size() > 1)
        {
            auto itr = m_states.end();
            while(itr != m_states.begin())
            {
                if(itr != m_states.end())
                {
                    if(!itr->second->isTranscendent())
                    {
                        break;
                    }
                }
                --itr;
            }
            for(; itr != m_states.end(); ++itr)
            {
                itr->second->update(t_time);
            }
        }
        else
        {
            m_states.back().second->update(t_time);
        }
    }

    void StateManager::draw()
    {
        if(m_states.empty())
        {
            return;
        }

        if(m_states.back().second->isTransparent() && m_states.size() > 1)
        {
            auto itr = m_states.end();
            while(itr != m_states.begin())
            {
                if(itr != m_states.end())
                {
                    if(!itr->second->isTransparent())
                    {
                        break;
                    }
                }
                --itr;
            }
            for(; itr != m_states.end(); ++itr)
            {
                m_shared->m_wind->getRenderWindow()->setView(itr->second->getView());
                itr->second->draw();
            }
        }
        else
        {
            m_shared->m_wind->getRenderWindow()->setView(m_states.back().second->getView());
            m_states.back().second->draw();
        }
    }

    SharedContext* StateManager::getContext()
    {
        return m_shared;
    }

    bool StateManager::hasState(const StateType& t_type) const
    {
        for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
        {
            if(itr->first == t_type)
            {
                auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), t_type);
                if(removed == m_toRemove.end())
                {
                    return true;
                }

                return false;
            }
        }
        return false;
    }

    StateType StateManager::getCurrentStateType() const
    {
        if(m_states.empty())
        {
            return static_cast<StateType>(0);
        }

        return m_states.back().first;
    }

    StateType StateManager::getNextToLast() const
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

    void StateManager::processRequests()
    {
        while(m_toRemove.begin() != m_toRemove.end())
        {
            removeState(*m_toRemove.begin());
            m_toRemove.erase(m_toRemove.begin());
        }
    }

    void StateManager::setGlobalState(const StateType& t_type)
    {
        for(auto& dependent : m_dependents)
        {
            dependent->changeState(t_type);
        }
    }

    void StateManager::switchTo(const StateType& t_type)
    {
        if(!m_states.empty() && m_states.back().first == t_type)
        {
            return;
        }

        setGlobalState(t_type);
        for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
        {
            if(itr->first != t_type)
            {
                continue;
            }

            m_states.back().second->deactivate();
            StateType tmp_type = itr->first;
            std::unique_ptr<BaseState> tmp_state = std::move(itr->second);
            m_states.erase(itr);
            tmp_state->activate();
            auto& view = tmp_state->getView();
            m_states.emplace_back(tmp_type, std::move(tmp_state));
            m_shared->m_wind->getRenderWindow()->setView(view);

            if(t_type == StateType::Loading)
            {
                return;
            }

            if(m_loading->hasWork())
            {
                switchTo(StateType::Loading);
            }

            return;
        }


        if(!m_states.empty())
        {
            m_states.back().second->deactivate();
        }

        createState(t_type);
        m_states.back().second->activate();
        m_shared->m_wind->getRenderWindow()->setView(m_states.back().second->getView());

        if(t_type == StateType::Loading)
        {
            return;
        }

        if(m_loading->hasWork())
        {
            switchTo(StateType::Loading);
        }
    }

    void StateManager::remove(const StateType& t_type)
    {
        for(auto itr = m_states.begin(); itr != m_states.end(); ++itr)
        {
            if(itr->first != t_type)
            {
                continue;
            }

            itr->second->onDestroy();
            m_toRemove.push_back(t_type);

            for(auto& dependent : m_dependents)
            {
                dependent->removeState(t_type);
            }
        }
    }

    void StateManager::createState(const StateType& t_type)
    {
        auto newState = m_stateFactory.find(t_type);

        if(newState == m_stateFactory.end())
        {
            return;
        }

        std::unique_ptr<BaseState> state(newState->second());
        state->m_view = m_shared->m_wind->getRenderWindow()->getDefaultView();
        m_states.emplace_back(t_type, std::move(state));
        m_states.back().second->onCreate();
    }

    void StateManager::removeState(const StateType& t_type)
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

    void StateManager::addDependent(StateDependent* t_dependent)
    {
        m_dependents.emplace_back(t_dependent);
    }

    void StateManager::removeDependent(StateDependent* t_dependent)
    {
        m_dependents.erase(std::find_if(m_dependents.begin(), m_dependents.end(), [t_dependent](StateDependent* l_itr)
        {
            return l_itr == t_dependent;
        }));
    }
}