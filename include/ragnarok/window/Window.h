#pragma once

#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "../events/EventManager.h"

namespace ragnarok
{
    /**
     * \brief Class wich takes care of all window related fonctionnality 
    (events processing, drawing, updating the screen, etc)
     */
    class Window
    {
        public:
        Window(const std::string& t_title = "Window", const sf::Vector2u& t_size = {640,480}, bool t_useShaders = false);
        ~Window();

        void BeginDraw();
        void EndDraw();

        void Update();

        bool IsDone() const;
        bool IsFullscreen() const;
        bool IsFocused() const;

        void ToggleFullscreen(EventDetails* t_details);
        void Close(EventDetails* t_details = nullptr);

        sf::RenderWindow* GetRenderWindow();
        Renderer* GetRenderer();
        EventManager* GetEventManager();
        sf::Vector2u GetWindowSize()const;
 
        /**
         * \brief Get the current view space
         * \return the rect representing the current view
         */
        sf::FloatRect GetViewSpace()const;

        private:
        void Setup(const std::string& t_title, const sf::Vector2u& t_size);
        void Create();

        sf::RenderWindow m_window;
        Renderer m_renderer;
        EventManager m_eventManager;
        sf::Vector2u m_windowSize;
        std::string m_windowTitle;
        bool m_isDone;
        bool m_isFullscreen;
        bool m_isFocused;

        bool m_shadersLoaded;
    };
}