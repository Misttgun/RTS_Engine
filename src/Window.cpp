#include "Window.h"

namespace SExE
{
    Window::Window()
    {
        setup("Window", sf::Vector2u(640, 480));
    }

    Window::Window(const std::string & t_title, const sf::Vector2u & t_size)
    {
        setup(t_title, t_size);
    }

    Window::~Window()
    {
        destroy();
    }

    void Window::beginDraw()
    {
        m_window.clear(sf::Color::Black);
    }

    void Window::endDraw()
    {
        m_window.display();
    }

    void Window::update()
    {
        sf::Event event {};
        while(m_window.pollEvent(event))
        {
            if(event.type == sf::Event::LostFocus)
            {
                m_isFocused = false;
                m_eventManager.setFocus(false);
            }
            else if(event.type == sf::Event::GainedFocus)
            {
                m_isFocused = true;
                m_eventManager.setFocus(true);
            }

            m_eventManager.handleEvent(event);
        }

        m_eventManager.update();
    }

    bool Window::isDone() const
    {
        return m_isDone;
    }

    bool Window::isFullScreen() const
    {
        return  m_isFullscreen;
    }

    bool Window::isFocused() const
    {
        return m_isFocused;
    }

    void Window::toggleFullscreen(EventDetails * t_details)
    {
        m_isFullscreen = !m_isFullscreen;
        destroy();
        create();
    }

    void Window::close(EventDetails * t_details)
    {
        m_isDone = true;
    }

    void Window::draw(sf::Drawable & t_drawable)
    {
        m_window.draw(t_drawable);
    }

    sf::RenderWindow* Window::getRenderWindow()
    {
        return &m_window;
    };

    EventManager* Window::getEventManager()
    {
        return &m_eventManager;
    }

    sf::Vector2u Window::getWindowSize() const
    {
        return m_windowSize;
    }

    void Window::setup(const std::string & t_title, const sf::Vector2u & t_size)
    {
        m_windowTitle = t_title;
        m_windowSize = t_size;
        m_isFullscreen = false;
        m_isDone = false;
        m_isFocused = false;

        m_eventManager.addCallback("Fullscreen_toggle", &Window::toggleFullscreen, this);
        m_eventManager.addCallback("Window_close", &Window::close, this);

        create();
    }

    void Window::destroy()
    {
        m_window.close();
    }

    void Window::create()
    {
        const auto style = m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default;
        const auto mode = sf::VideoMode(m_windowSize.x, m_windowSize.y, 32);
        m_window.create(mode, m_windowTitle, style);
    }
}