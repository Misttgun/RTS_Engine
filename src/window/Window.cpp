#include "Window.h"

namespace SExE
{
    Window::Window(const std::string & t_title, const sf::Vector2u & t_size, bool t_useShaders) : m_renderer(this, t_useShaders)
    {
        setup(t_title, t_size);
    }

    Window::~Window()
    {
        m_window.close();
    }

    void Window::setup(const std::string & t_title, const sf::Vector2u & t_size)
    {
        m_windowTitle = t_title;
        m_windowSize = t_size;
        m_isFullscreen = false;
        m_isDone = false;
        m_isFocused = false;
        m_shadersLoaded = false;

        m_eventManager.addCallback(StateType(0), "Fullscreen_toggle", &Window::toggleFullscreen, this);
        m_eventManager.addCallback(StateType(0), "Window_close", &Window::close, this);

        create();
    }

    void Window::create()
    {
        sf::Uint32 style = sf::Style::Default;
        
        if(m_isFullscreen)
        {
            style = sf::Style::Fullscreen;
        }

        sf::ContextSettings settings;
        settings.depthBits = 24;
        settings.stencilBits = 8;
        settings.antialiasingLevel = 0;
        settings.majorVersion = 4;
        settings.minorVersion = 5;

        m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y, 32), m_windowTitle, style, settings);

        if(!m_shadersLoaded)
        {
            m_renderer.loadShaders(); 
            m_shadersLoaded = true;
        }
    }

    void Window::beginDraw()
    {
        m_window.clear(sf::Color::Black);
        m_renderer.beginDrawing();
    }

    void Window::endDraw()
    {
        m_window.display();
        m_renderer.endDrawing();
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
        m_window.close();
        create();
    }

    void Window::close(EventDetails * t_details)
    {
        m_isDone = true;
    }

    sf::RenderWindow* Window::getRenderWindow()
    {
        return &m_window;
    };

    Renderer* Window::getRenderer()
    {
        return &m_renderer;
    }

    EventManager* Window::getEventManager()
    {
        return &m_eventManager;
    }

    sf::Vector2u Window::getWindowSize() const
    {
        return m_windowSize;
    }

    sf::FloatRect Window::getViewSpace() const
    {
        sf::Vector2f viewCenter(m_window.getView().getCenter());
        sf::Vector2f viewSize(m_window.getView().getSize());
        sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
        sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
        return viewSpace;
    }
}