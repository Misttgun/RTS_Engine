#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "EventManager.h"

#include <string>
#include <SFML/Graphics.hpp>

namespace Engine 
{
	class Window
	{
	public:
		Window();
		Window(const std::string& t_title, const sf::Vector2u& t_size);
		~Window();

		void beginDraw(); //Clear the window
		void endDraw(); // Display the changes

		void update();

		bool isDone() const { return m_isDone; }
		bool isFullScreen() const { return  m_isFullscreen; }
		bool isFocused() const { return m_isFocused; }

		void toggleFullscreen(EventDetails* t_details);
		void close(EventDetails* t_details = nullptr);

		void draw(sf::Drawable& t_drawable);

		sf::RenderWindow* getRenderWindow() { return &m_window; };
		EventManager* getEventManager() { return &m_eventManager; }
		sf::Vector2u getWindowSize() const { return m_windowSize; }

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
