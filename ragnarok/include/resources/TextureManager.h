#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "ResourceManager.h"

namespace ragnarok
{
    class TextureManager : public ResourceManager<TextureManager, sf::Texture>
    {
        public:
        TextureManager() : ResourceManager("res/textures.cfg")
        {}

        bool Load(sf::Texture* t_resource, const std::string& t_path)
        {
            return t_resource->loadFromFile(Utils::GetWorkingDirectory() + t_path);
        }
    };
}