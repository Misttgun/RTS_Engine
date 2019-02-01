#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "EventManager.h"

#include <string>
#include <SFML/Graphics.hpp>

namespace SExE
{
    class Window
    {
        public:
        Window();
        Window(const std::string& t_title, const sf::Vector2u& t_size);
        ~Window();

        void beginDraw();
        void endDraw();

        void update();

        bool isDone() const;
        bool isFullScreen() const;
        bool isFocused() const;

        void toggleFullscreen(EventDetails* t_details);
        void close(EventDetails* t_details = nullptr);

        void draw(sf::Drawable& t_drawable);

        sf::RenderWindow* getRenderWindow();
        EventManager* getEventManager();
        sf::Vector2u getWindowSize() const;

        private:
        void setup(const std::string& t_title, const sf::Vector2u& t_size);
        void destroy();
        void create();

        sf::RenderWindow m_window;
        EventManager m_eventManager;
        sf::Vector2u m_windowSize;
        std::string m_windowTitle;
        bool m_isDone;
        bool m_isFullscreen;
        bool m_isFocused;
    };
}

#endif // !WINDOW_H
