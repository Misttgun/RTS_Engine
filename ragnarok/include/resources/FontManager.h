#pragma once

#include <SFML/Graphics/Font.hpp>
#include "ResourceManager.h"

namespace ragnarok
{
    class FontManager : public ResourceManager<FontManager, sf::Font>
    {
        public:
        FontManager() : ResourceManager("res/fonts.cfg")
        {}

        bool Load(sf::Font* t_resource, const std::string& t_path)
        {
            return t_resource->loadFromFile(Utils::GetWorkingDirectory() + t_path);
        }
    };
}