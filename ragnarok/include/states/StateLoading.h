#pragma once

#include "../states/BaseState.h"
#include "../events/EventManager.h"
#include "../threads/FileLoader.h"

namespace ragnarok
{
    using LoaderContainer = std::vector<FileLoader*>;

    /**
     * \brief Focus on graphical progress representation and loading logic.
     */
    class StateLoading : public BaseState
    {
        public:
        StateLoading(StateManager* t_stateManager);
        ~StateLoading() = default;

        void OnCreate() override;
        void OnDestroy() override;

        void Activate() override;
        void Deactivate() override;

        void AddLoader(FileLoader* t_loader);
        bool HasWork() const;

        void Update(const sf::Time& t_time) override;
        void Draw() override;

        void SetManualContinue(bool t_continue);
        void Proceed(EventDetails* t_details);

        private:
        void UpdateText(const std::string& t_text, float t_percentage);
        float CalculatePercentage();

        LoaderContainer m_loaders;
        sf::Text m_text;
        sf::RectangleShape m_rect;
        size_t m_originalWork;
        bool m_manualContinue;
    };
}