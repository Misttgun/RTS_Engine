#pragma once

#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    class TextureGenerator : public BaseGenerator
    {
    public:
        TextureGenerator() = default;

        TextureGenerator(const std::string& t_texture) : m_texture(t_texture)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            if (m_texture.empty())
            {
                return;
            }

            TextureManager* manager = t_emitter->GetParticleSystem()->GetTextureManager();
            if (!manager->RequireResource(m_texture))
            {
                return;
            } // +1 for checking and a quick pointer.

            auto& textures = t_particles->m_texture;
            auto& drawables = t_particles->m_drawable;
            const auto resource = manager->GetResource(m_texture);
            const auto size = resource->getSize();

            for (auto i = t_from; i <= t_to; ++i)
            {
                textures[i] = m_texture;
                manager->RequireResource(m_texture);
                drawables[i].setTexture(resource);
                drawables[i].setTextureRect(sf::IntRect(0, 0, size.x, size.y));
            }

            manager->ReleaseResource(m_texture); // Release the initial +1.
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_texture;
        }

    private:
        std::string m_texture;
    };
}