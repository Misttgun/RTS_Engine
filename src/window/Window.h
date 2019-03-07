#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "EventManager.h"

namespace SExE
{
    class Window
    {
        public:
        Window(const std::string& t_title = "Window", const sf::Vector2u& t_size = {640,480}, bool t_useShaders = true);
        ~Window();

        void beginDraw();
        void endDraw();

        void update();

        bool isDone() const;
        bool isFullScreen() const;
        bool isFocused() const;

        void toggleFullscreen(EventDetails* t_details);
        void close(EventDetails* t_details = nullptr);

        sf::RenderWindow* getRenderWindow();
        Renderer* getRenderer();
        EventManager* getEventManager();
        sf::Vector2u getWindowSize() const;
        sf::FloatRect getViewSpace()const;

        private:
        void setup(const std::string& t_title, const sf::Vector2u& t_size);
        void create();

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

#endif // !WINDOW_H
