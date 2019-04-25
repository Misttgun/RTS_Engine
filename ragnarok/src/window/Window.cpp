#include "../../include/window/Window.h"

namespace ragnarok
{
    Window::Window(const std::string& t_title, const sf::Vector2u& t_size, bool t_useShaders) : m_renderer(this, t_useShaders)
    {
        Setup(t_title, t_size);
    }

    Window::~Window()
    {
        m_window.close();
    }

    void Window::Setup(const std::string& t_title, const sf::Vector2u& t_size)
    {
        m_windowTitle = t_title;
        m_windowSize = t_size;
        m_isFullscreen = false;
        m_isDone = false;
        m_isFocused = true;
        m_shadersLoaded = false;

        //Add all the window callbacks with StateType(0) because they are not state dependent
        m_eventManager.AddCallback(StateType(0), "Fullscreen_toggle", &Window::ToggleFullscreen, this);
        m_eventManager.AddCallback(StateType(0), "Window_close", &Window::Close, this);

        Create();
    }

    void Window::Create()
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
            m_renderer.LoadShaders(); m_shadersLoaded = true;
        }
    }

    void Window::BeginDraw()
    {
        m_window.clear(sf::Color::Black); 
        m_renderer.BeginDrawing();
    }

    void Window::EndDraw()
    {
        m_window.display(); 
        m_renderer.EndDrawing();
    }

    bool Window::IsDone() const
    {
        return m_isDone;
    }

    bool Window::IsFullscreen() const
    {
        return m_isFullscreen;
    }

    bool Window::IsFocused() const
    {
        return m_isFocused;
    }

    sf::RenderWindow* Window::GetRenderWindow()
    {
        return &m_window;
    }

    Renderer* Window::GetRenderer()
    {
        return &m_renderer;
    }

    EventManager* Window::GetEventManager()
    {
        return &m_eventManager;
    }

    sf::Vector2u Window::GetWindowSize() const
    {
        return m_windowSize;
    }

    sf::FloatRect Window::GetViewSpace() const
    {
        const sf::Vector2f viewCenter(m_window.getView().getCenter());
        const sf::Vector2f viewSize(m_window.getView().getSize());
        const sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
        const sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize); // Top left corner + rectangle size

        return viewSpace;
    }

    void Window::ToggleFullscreen(EventDetails* t_details)
    {
        m_isFullscreen = !m_isFullscreen;
        m_window.close();
        Create();
    }

    void Window::Close(EventDetails* t_details)
    {
        m_isDone = true;
    }

    void Window::Update()
    {
        sf::Event event{};

        while(m_window.pollEvent(event))
        {
            if(event.type == sf::Event::LostFocus)
            {
                m_isFocused = false; 
                m_eventManager.SetFocus(false);
            }
            else if(event.type == sf::Event::GainedFocus)
            {
                m_isFocused = true; 
                m_eventManager.SetFocus(true);
            }

            m_eventManager.HandleEvent(event);
        }

        m_eventManager.Update();
    }
}
