#include "Window.h"

Engine::Window::Window()
{
	setup("Window", sf::Vector2u(640, 480));
}

Engine::Window::Window(const std::string & t_title, const sf::Vector2u & t_size)
{
	setup(t_title, t_size);
}

Engine::Window::~Window()
{
	destroy();
}

void Engine::Window::beginDraw()
{
	m_window.clear(sf::Color::Black);
}

void Engine::Window::endDraw()
{
	m_window.display();
}

void Engine::Window::update()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed) {
			m_isDone = true;
		}
		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
			toggleFullScreen();
		}
	}
}

bool Engine::Window::isDone() const
{
	return m_isDone;
}

bool Engine::Window::isFullScreen() const
{
	return m_isFullscreen;
}

sf::Vector2u Engine::Window::getWindowSize() const
{
	return m_windowSize;
}

void Engine::Window::toggleFullScreen()
{
	m_isFullscreen = !m_isFullscreen;
	destroy();
	create();
}

void Engine::Window::draw(sf::Drawable & t_drawable)
{
	m_window.draw(t_drawable);
}

void Engine::Window::setup(const std::string & t_title, const sf::Vector2u & t_size)
{
	m_windowTitle = t_title;
	m_windowSize = t_size;
	m_isFullscreen = false;
	m_isDone = false;
	create();
}

void Engine::Window::destroy()
{
	m_window.close();
}

void Engine::Window::create()
{
	auto style = m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default;
	auto mode = sf::VideoMode(m_windowSize.x, m_windowSize.y, 32);
	m_window.create(mode, m_windowTitle, style);
}
