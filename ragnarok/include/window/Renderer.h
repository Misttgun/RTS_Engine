#pragma once

#include <unordered_map>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace ragnarok
{
    class Window;

    using ShaderList = std::unordered_map<std::string, std::unique_ptr<sf::Shader>>;

    /* Class which takes care of rendering to the window.
    Very useful for using shader and changing shaders at runtime.*/
    class Renderer
    {
        friend Window;

        public:
        Renderer(Window* t_window, bool t_useShaders = true);

        void AdditiveBlend(bool t_flag);
        bool UseShader(const std::string& t_name);
        void DisableShader();
        sf::Shader* GetShader(const std::string& t_name);

        void BeginDrawing();
        bool IsDrawing()const;
        void Draw(const sf::Shape& t_shape, sf::RenderTarget* t_target = nullptr);
        void Draw(const sf::Sprite& t_sprite, sf::RenderTarget* t_target = nullptr);
        void Draw(const sf::Drawable& t_drawable, sf::RenderTarget* t_target = nullptr);
        void EndDrawing();

        private:
        void LoadShaders();

        Window* m_window;
        ShaderList m_shaders;
        sf::Shader* m_currentShader;
        bool m_addBlend;
        bool m_drawing;
        bool m_useShaders;
        unsigned int m_drawCalls; // Useful for debugging
    };
}