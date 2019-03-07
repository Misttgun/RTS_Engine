#include "Renderer.h"
#include "Window.h"
#include "Utilities.h"

namespace SExE
{
    Renderer::Renderer(Window* t_window, bool t_useShaders)
        : m_window(t_window), m_useShaders(t_useShaders), m_drawing(false), m_addBlend(false), m_drawCalls(0), m_currentShader(nullptr)
    {}

    void Renderer::additiveBlend(bool t_flag)
    {
        m_addBlend = t_flag;
    }

    bool Renderer::useShader(const std::string& t_name)
    {
        if(!m_useShaders)
        {
            return false;
        }
        m_currentShader = getShader(t_name);
        return (m_currentShader != nullptr);
    }

    void Renderer::disableShader()
    {
        m_currentShader = nullptr;
    }

    sf::Shader* Renderer::getShader(const std::string& t_name)
    {
        if(!m_useShaders)
        {
            return nullptr;
        }
        auto shader = m_shaders.find(t_name);
        if(shader == m_shaders.end())
        {
            return nullptr;
        }
        return shader->second.get();
    }

    void Renderer::beginDrawing()
    {
        m_drawing = true;
        m_drawCalls = 0;
    }

    bool Renderer::isDrawing() const
    {
        return m_drawing;
    }

    void Renderer::endDrawing()
    {
        m_drawing = false;
    }

    void Renderer::draw(const sf::Shape& t_shape, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            if(!m_window->getViewSpace().intersects(t_shape.getGlobalBounds()))
            {
                return;
            }
        }

        draw((const sf::Drawable&)t_shape, t_target);
    }

    void Renderer::draw(const sf::Sprite& t_sprite, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            if(!m_window->getViewSpace().intersects(t_sprite.getGlobalBounds()))
            {
                return;
            }
        }

        draw((const sf::Drawable&)t_sprite, t_target);
    }

    void Renderer::draw(const sf::Drawable& t_drawable, sf::RenderTarget* t_target)
    {
        if(!t_target)
        {
            t_target = m_window->getRenderWindow();
        }

        t_target->draw(t_drawable, (m_addBlend ? sf::BlendAdd : m_currentShader && m_useShaders ? m_currentShader : sf::RenderStates::Default));
        ++m_drawCalls;
    }

    void Renderer::loadShaders()
    {
        if(!m_useShaders)
        {
            return;
        }
        auto directory = Utils::getWorkingDirectory() + "media/Shaders/";
        auto v_shaders = Utils::getFileList(directory, "*.vert", false);
        auto f_shaders = Utils::getFileList(directory, "*.frag", false);

        for(auto& shader : v_shaders)
        {
            auto& file = shader.first;
            auto name = file.substr(0, file.find(".vert"));
            auto fragShader = std::find_if(f_shaders.begin(), f_shaders.end(), [&name](std::pair<std::string, bool>& l_pair)
            {
                return l_pair.first == name + ".frag";
            });

            auto shaderItr = m_shaders.emplace(name, std::move(std::make_unique<sf::Shader>()));
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

        for(auto& shader : f_shaders)
        {
            auto& file = shader.first;
            auto name = file.substr(0, file.find(".frag"));
            auto shaderItr = m_shaders.emplace(name, std::move(std::make_unique<sf::Shader>()));
            auto& shader = shaderItr.first->second;
            shader->loadFromFile(directory + name + ".frag", sf::Shader::Fragment);
        }
    }
}