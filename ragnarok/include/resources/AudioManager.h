#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include "ResourceManager.h"

namespace ragnarok
{
    class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>
    {
        public:
        AudioManager() : ResourceManager("res/audio.cfg")
        {}

        bool Load(sf::SoundBuffer* t_resource, const std::string& t_path)
        {
            return t_resource->loadFromFile(Utils::GetWorkingDirectory() + t_path);
        }
    };
}