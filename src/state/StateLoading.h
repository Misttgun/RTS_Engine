#pragma once
#ifndef STATELOADING_H
#define STATELOADING_H

#include "BaseState.h"
#include "EventManager.h"
#include "FileLoader.h"

namespace SExE
{
    using LoaderContainer = std::vector<FileLoader*>;

    class StateLoading : public BaseState
    {
        public:
        StateLoading(StateManager* t_stateManager);
        ~StateLoading();

        void onCreate();
        void onDestroy();

        void activate();
        void deactivate();

        void addLoader(FileLoader* t_loader);
        bool hasWork() const;

        void update(const sf::Time& t_time);
        void draw();

        void setManualContinue(bool t_continue);
        void proceed(EventDetails* t_details);

        private:
        void updateText(const std::string& t_text, float t_percentage);
        float calculatePercentage();

        LoaderContainer m_loaders;
        sf::Text m_text;
        sf::RectangleShape m_rect;
        unsigned short m_percentage;
        size_t m_originalWork;
        bool m_manualContinue;
    };
}

#endif // !STATELOADING_H