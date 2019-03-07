#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <unordered_map>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace SExE
{
    class Window;

    using ShaderList = std::unordered_map<std::string, std::unique_ptr<sf::Shader>>;

    class Renderer
    {
        friend Window;
        
        public:
        Renderer(Window* t_window, bool t_useShaders = true);

        void additiveBlend(bool t_flag);
        bool useShader(const std::string& t_name);
        void disableShader();
        sf::Shader* getShader(const std::string& t_name);

        void beginDrawing();
        bool isDrawing()const;
        void draw(const sf::Shape& t_shape, sf::RenderTarget* t_target = nullptr);
        void draw(const sf::Sprite& t_sprite, sf::RenderTarget* t_target = nullptr);
        void draw(const sf::Drawable& t_drawable, sf::RenderTarget* t_target = nullptr);
        void endDrawing();
        
        private:
        void loadShaders();

        Window* m_window;
        ShaderList m_shaders;
        sf::Shader* m_currentShader;
        bool m_addBlend;
        bool m_drawing;
        bool m_useShaders;
        unsigned int m_drawCalls;
    };
}

#endif // !RENDERER_H
