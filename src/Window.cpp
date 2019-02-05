#include "Window.h"

namespace SExE
{
    // - constructor / destructor section
    Window::Window()
    {
        setup("Window", sf::Vector2u(640, 480));
    }

    Window::Window(const Window& w)
    {
        setup(sf::String(w.m_windowTitle), sf::Vector2u(w.m_windowSize.x, w.m_windowSize.y));
        m_eventManager = w.m_eventManager;
        m_isDone = w.m_isDone;
        m_isFullscreen = w.m_isFullscreen;
        m_isFocused = w.m_isFocused;
    }

    Window::Window(const std::string & t_title, const sf::Vector2u & t_size)
    {
        setup(t_title, t_size);
    }

    Window::~Window()
    {
        destroy();
    }

    // - drawing section

    void Window::beginDraw()
    {
        m_window.clear(sf::Color::Black);
    }

    void Window::endDraw()
    {
        m_window.display();
    }

    void Window::draw(const sf::Drawable & t_drawable)
    {
        m_window.draw(t_drawable);
    }

    // - window special event section
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

    // - input section
    void Window::handleInput()
    {
        sf::Event event {};
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::LostFocus)
            {
                m_isFocused = false;
                m_eventManager.setFocus(false);
            }

            else if (event.type == sf::Event::GainedFocus)
            {
                m_isFocused = true;
                m_eventManager.setFocus(true);
            }

            else if (event.type == sf::Event::Closed)
            {
                close();
            }

            m_eventManager.handleEvent(event);
        }

        m_eventManager.update();
    }

    // getter / setter section


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

    // - internal method section

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