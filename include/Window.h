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
            // - constructor / destructor section
            Window();
            Window(const Window& w);
            Window(const std::string& t_title, const sf::Vector2u& t_size);
            ~Window();

            // - drawing section
            void beginDraw();
            void endDraw();
            void draw(const sf::Drawable& t_drawable);

            // - window special event section
            void toggleFullscreen(EventDetails* t_details);
            void close(EventDetails* t_details = nullptr);

            // - input section
            void handleInput();

            // - propagation section
            bool pollEvent(sf::Event& e) { return m_window.pollEvent(e); }

            // - getter / setters section
            bool isDone() const;
            bool isFullScreen() const;
            bool isFocused() const;
            sf::RenderWindow* getRenderWindow();
            EventManager* getEventManager();
            sf::Vector2u getWindowSize() const;

        private:
            // - internal method section
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
