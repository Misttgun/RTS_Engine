#include "ragnarok/window/Renderer.h"
#include "ragnarok/window/Window.h"
#include "ragnarok/utilities/Utilities.h"

namespace ragnarok
{
    Renderer::Renderer(Window* t_window, bool t_useShaders) 
    : m_window(t_window), m_currentShader(nullptr), m_addBlend(false), m_drawing(false)
    , m_useShaders(t_useShaders), m_drawCalls(0) 
    {}

    void Renderer::AdditiveBlend(bool t_flag)
    {
        m_addBlend = t_flag;
    }

    bool Renderer::UseShader(const std::string& t_name)
    {
        if(!m_useShaders)
        {
            return false;
        }

        m_currentShader = GetShader(t_name);

        return (m_currentShader != nullptr);
    }

    void Renderer::DisableShader()
    {
        m_currentShader = nullptr;
    }

    sf::Shader* Renderer::GetShader(const std::string& t_name)
    {
        if(!m_useShaders)
        {
            return nullptr;
        }

        const auto shader = m_shaders.find(t_name);

        if(shader == m_shaders.end())
        {
            return nullptr;
        }

        return shader->second.get();
    }

    void Renderer::BeginDrawing()
    {
        m_drawing = true; 
        m_drawCalls = 0;
    }

    bool Renderer::IsDrawing() const
    {
        return m_drawing;
    }

    void Renderer::EndDrawing()
    {
        m_drawing = false;
    }

    void Renderer::Draw(const sf::Shape& t_shape, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            if(!m_window->GetViewSpace().intersects(t_shape.getGlobalBounds()))
            {
                return;
            }
        }

        Draw(static_cast<const sf::Drawable&>(t_shape), t_target);
    }

    void Renderer::Draw(const sf::Sprite& t_sprite, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            if(!m_window->GetViewSpace().intersects(t_sprite.getGlobalBounds()))
            {
                return;
            }
        }

        Draw(static_cast<const sf::Drawable&>(t_sprite), t_target);
    }

    void Renderer::Draw(const sf::Drawable& t_drawable, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            t_target = m_window->GetRenderWindow();
        }

        t_target->draw(t_drawable, (m_addBlend ? sf::BlendAdd : m_currentShader
                       && m_useShaders ? m_currentShader : sf::RenderStates::Default));

        ++m_drawCalls;
    }

    void Renderer::LoadShaders()
    {
        if(!m_useShaders)
        {
            return;
        }

        const auto directory = Utils::GetWorkingDirectory() + "res/Shaders/";
        auto v_shaders = Utils::GetFileList(directory, "*.vert", false);
        auto f_shaders = Utils::GetFileList(directory, "*.frag", false);

        for(auto& v_shader : v_shaders)
        {
            auto& file = v_shader.first;
            auto name = file.substr(0, file.find(".vert"));
            auto fragShader = std::find_if(f_shaders.begin(), f_shaders.end(),
                                           [&name](std::pair<std::string, bool>& t_pair) { return t_pair.first == name + ".frag"; });

            auto shaderItr = m_shaders.emplace(name, std::make_unique<sf::Shader>());
            auto& shader = shaderItr.first->second;

            if(fragShader != f_shaders.end())
            {
                shader->loadFromFile(directory + name + ".vert", directory + name + ".frag");
                f_shaders.erase(fragShader);
            }
            else
            {
                shader->loadFromFile(directory + name + ".vert", sf::Shader::Vertex);
            }
        }

        for(auto& f_shader : f_shaders)
        {
            auto& file = f_shader.first;
            auto name = file.substr(0, file.find(".frag"));
            auto shaderItr = m_shaders.emplace(name, std::make_unique<sf::Shader>());
            auto& shader = shaderItr.first->second;
            shader->loadFromFile(directory + name + ".frag", sf::Shader::Fragment);
        }
    }
}