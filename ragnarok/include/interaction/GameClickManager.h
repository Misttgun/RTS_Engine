//
// Created by florian on 08/05/19.
//

#ifndef RTS_ENGINE_GAMECLICKMANAGER_H
#define RTS_ENGINE_GAMECLICKMANAGER_H


#include <string>
#include <functional>
#include "../events/EventDetails.h"
#include "../events/EventManager.h"
#include "../states/StateManager.h"

namespace ragnarok
{
    using GCMCallbacks = std::unordered_map<std::string,
            std::function<void(int, sf::Vector2f)>>;

    class GameClickManager {
    public:
        explicit GameClickManager(StateManager *t_stateManager);
        void setupHooks(EventManager *t_eventManager);
        void handleEvent(EventDetails *t_event);

        template<class T>
        bool addCallback(const std::string &t_name,
                         void(T::*t_func)(int, sf::Vector2f), T *t_instance) {
            auto callback = std::bind(t_func, t_instance,
                                      std::placeholders::_1,
                                      std::placeholders::_2);

            return m_callbacks.emplace(t_name, callback).second;
        }

        bool removeCallback(const std::string &t_name) {
            const auto itr = m_callbacks.find(t_name);
            if (itr == m_callbacks.end()) {
                return false;
            }

            m_callbacks.erase(t_name);

            return true;
        }

    private:
        void selectEntity(int t_position);

        int m_selectedEntity = -1;
        GCMCallbacks m_callbacks;
        StateManager *m_stateManager;

        void actOnEntity(int t_entityID, sf::Vector2f t_position);
    };
}

#endif //RTS_ENGINE_GAMECLICKMANAGER_H
