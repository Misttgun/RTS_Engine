#include "../../include/states/StateManager.h"
#include "../../include/sounds/SoundManager.h"

namespace ragnarok
{
    SoundManager::SoundManager(AudioManager* t_audioMgr)
        : m_lastID(0), m_audioManager(t_audioMgr), m_elapsed(0.f), m_numSounds(0)
    {}

    SoundManager::~SoundManager()
    {
        Cleanup();
    }

    void SoundManager::ChangeState(const StateType& t_state)
    {
        PauseAll(m_currentState);
        UnpauseAll(t_state);
        SetState(t_state);

        if(m_music.find(m_currentState) != m_music.end())
        {
            return;
        }

        SoundInfo info("");
        m_music.emplace(m_currentState, std::make_pair(info, nullptr)); // ?
    }

    void SoundManager::RemoveState(const StateType& t_state)
    {
        auto& StateSounds = m_audio.find(t_state)->second;
        for(auto &itr : StateSounds)
        {
            RecycleSound(itr.first, std::move(itr.second.second), itr.second.first.m_name);
        }

        m_audio.erase(t_state);
        auto music = m_music.find(t_state);

        if(music == m_music.end())
        {
            return;
        }

        if(music->second.second)
        {
            --m_numSounds;
        }

        m_music.erase(t_state);
    }

    void SoundManager::Cleanup()
    {
        for(auto &state : m_audio)
        {
            for(auto &sound : state.second)
            {
                m_audioManager->ReleaseResource(sound.second.first.m_name);
            }
        }

        m_audio.clear();

        for(auto &recycled : m_recycled)
        {
            m_audioManager->ReleaseResource(recycled.first.second);
        }

        m_recycled.clear();
        m_music.clear();

        m_properties.clear();
        m_numSounds = 0;
        m_lastID = 0;
    }

    void SoundManager::Update(float t_dT)
    {
        m_elapsed += t_dT;

        if(m_elapsed < 0.33f)
        {
            return;
        }

        // Run once every third of a second.
        m_elapsed = 0;
        auto& container = m_audio[m_currentState];

        for(auto itr = container.begin(); itr != container.end();)
        {
            if(!itr->second.second->getStatus())
            {
                RecycleSound(itr->first, std::move(itr->second.second), itr->second.first.m_name);
                itr = container.erase(itr); // Remove sound.
                continue;
            }

            ++itr;
        }

        auto music = m_music.find(m_currentState);
        if(music == m_music.end())
        {
            return;
        }

        if(!music->second.second)
        {
            return;
        }

        if(music->second.second->getStatus())
        {
            return;
        }

        music->second.second.release();
        --m_numSounds;
    }

    SoundID SoundManager::Play(const std::string& t_sound, const sf::Vector3f& t_position, bool t_loop, bool t_relative)
    {
        auto props = GetSoundProperties(t_sound);

        if(!props)
        {
            return -1;
        } // Failed to load sound properties.

        SoundID id;
        std::unique_ptr<sf::Sound> sound;

        if(!CreateSound(id, props->m_audioName, sound))
        {
            return -1;
        }

        // Sound created successfully.
        SetUpSound(sound.get(), props, t_loop, t_relative);
        sound->setPosition(t_position);
        sound->play();
        SoundInfo info(props->m_audioName);
        m_audio[m_currentState].emplace(id, std::make_pair(info, std::move(sound)));

        return id;
    }

    bool SoundManager::Play(const SoundID& t_id)
    {
        auto& container = m_audio[m_currentState];
        auto sound = container.find(t_id);

        if(sound == container.end())
        {
            return false;
        }

        sound->second.second->play();
        sound->second.first.m_manualPaused = false;

        return true;
    }

    bool SoundManager::Stop(const SoundID& t_id)
    {
        auto& container = m_audio[m_currentState];
        auto sound = container.find(t_id);

        if(sound == container.end())
        {
            return false;
        }

        sound->second.second->stop();
        sound->second.first.m_manualPaused = true;

        return true;
    }

    bool SoundManager::Pause(const SoundID& t_id)
    {
        auto& container = m_audio[m_currentState];
        auto sound = container.find(t_id);

        if(sound == container.end())
        {
            return false;
        }

        sound->second.second->pause();
        sound->second.first.m_manualPaused = true;

        return true;
    }

    bool SoundManager::PlayMusic(const std::string& t_musicId, float t_volume, bool t_loop)
    {
        auto s = m_music.find(m_currentState);

        if(s == m_music.end())
        {
            return false;
        }

        std::string path = m_audioManager->GetPath(t_musicId);
        if(path.empty())
        {
            return false;
        }

        auto& music = s->second.second;
        if(!music)
        {
            music = std::make_unique<sf::Music>();
            ++m_numSounds;
        }

        if(!music->openFromFile(Utils::GetWorkingDirectory() + path))
        {
            music.release();
            --m_numSounds;
            std::cerr << "[SoundManager] Failed to load music from file: " << t_musicId << std::endl;
            return false;
        }

        music->setLoop(t_loop);
        music->setVolume(t_volume);
        music->setRelativeToListener(true); // Always relative.
        music->play();
        s->second.first.m_name = t_musicId;

        return true;
    }

    bool SoundManager::PlayMusic(const StateType& t_state)
    {
        auto music = m_music.find(m_currentState);

        if(music == m_music.end())
        {
            return false;
        }

        if(!music->second.second)
        {
            return false;
        }

        music->second.second->play();
        music->second.first.m_manualPaused = false;

        return true;
    }

    bool SoundManager::StopMusic(const StateType& t_state)
    {
        auto music = m_music.find(m_currentState);

        if(music == m_music.end())
        {
            return false;
        }

        if(!music->second.second)
        {
            return false;
        }

        music->second.second->stop();
        music->second.second.release();
        --m_numSounds;

        return true;
    }

    bool SoundManager::PauseMusic(const StateType& t_state)
    {
        auto music = m_music.find(m_currentState);

        if(music == m_music.end())
        {
            return false;
        }

        if(!music->second.second)
        {
            return false;
        }

        music->second.second->pause();
        music->second.first.m_manualPaused = true;

        return true;
    }

    bool SoundManager::SetPosition(const SoundID& t_id, const sf::Vector3f& t_pos)
    {
        auto& container = m_audio[m_currentState];
        auto sound = container.find(t_id);

        if(sound == container.end())
        {
            return false;
        }

        sound->second.second->setPosition(t_pos);

        return true;
    }

    bool SoundManager::IsPlaying(const SoundID& t_id) const
    {
        const auto& container = m_audio.find(m_currentState);

        if(container == m_audio.end())
        {
            return false;
        }

        const auto& sound = container->second.find(t_id);

        return(sound != container->second.end() ? (sound->second.second->getStatus() != sf::SoundSource::Status::Stopped) : false);
    }

    SoundProps* SoundManager::GetSoundProperties(const std::string& t_soundName)
    {
        auto properties = m_properties.find(t_soundName);

        if(properties == m_properties.end())
        {
            if(!LoadProperties(t_soundName))
            {
                return nullptr;
            }

            properties = m_properties.find(t_soundName);
        }

        return &properties->second;
    }

    bool SoundManager::LoadProperties(const std::string& t_name)
    {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + "res/Sounds/" + t_name + ".sound");

        if(!file.is_open())
        {
            std::cerr << "Failed to load sound: " << t_name << std::endl;
            return false;
        }

        SoundProps props("");
        std::string line;

        while(std::getline(file, line))
        {
            if(line[0] == '|')
            {
                continue;
            }

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;

            if(type == "Audio")
            {
                keystream >> props.m_audioName;
            }
            else if(type == "Volume")
            {
                keystream >> props.m_volume;
            }
            else if(type == "Pitch")
            {
                keystream >> props.m_pitch;
            }
            else if(type == "Distance")
            {
                keystream >> props.m_minDistance;
            }
            else if(type == "Attenuation")
            {
                keystream >> props.m_attenuation;
            }
            else
            {
                // ?
            }
        }

        file.close();

        if(props.m_audioName.empty())
        {
            return false;
        }

        m_properties.emplace(t_name, props);

        return true;
    }

    void SoundManager::PauseAll(const StateType& t_state)
    {
        auto& container = m_audio[t_state];

        for(auto itr = container.begin(); itr != container.end();)
        {
            if(!itr->second.second->getStatus())
            {
                RecycleSound(itr->first, std::move(itr->second.second), itr->second.first.m_name);
                itr = container.erase(itr);
                continue;
            }

            itr->second.second->pause();
            ++itr;
        }

        auto music = m_music.find(t_state);
        if(music == m_music.end())
        {
            return;
        }

        if(!music->second.second)
        {
            return;
        }

        music->second.second->pause();
    }

    void SoundManager::UnpauseAll(const StateType& t_state)
    {
        auto& container = m_audio[t_state];

        for(auto &itr : container)
        {
            if(itr.second.first.m_manualPaused)
            {
                continue;
            }

            itr.second.second->play();
        }

        auto music = m_music.find(t_state);

        if(music == m_music.end())
        {
            return;
        }

        if(!music->second.second || music->second.first.m_manualPaused)
        {
            return;
        }

        music->second.second->play();
    }

    bool SoundManager::CreateSound(SoundID& t_id, const std::string& t_audioName, std::unique_ptr<sf::Sound>& t_sound)
    {
        if(!m_recycled.empty() && (m_numSounds >= Max_Sounds || m_recycled.size() >= Sound_Cache))
        {
            auto itr = m_recycled.begin();
            while(itr != m_recycled.end())
            {
                if(itr->first.second == t_audioName)
                {
                    break;
                }

                ++itr;
            }

            if(itr == m_recycled.end())
            {
                // If a sound with the same name hasn't been found!
                auto element = m_recycled.begin();
                t_id = element->first.first;
                m_audioManager->ReleaseResource(element->first.second);
                m_audioManager->RequireResource(t_audioName);
                t_sound = std::move(element->second);
                t_sound->setBuffer(*m_audioManager->GetResource(t_audioName));
                m_recycled.erase(element);
            }
            else
            {
                t_id = itr->first.first;
                t_sound = std::move(itr->second);
                m_recycled.erase(itr);
            }

            return true;
        }

        if(m_numSounds < Max_Sounds)
        {
            if(m_audioManager->RequireResource(t_audioName))
            {
                t_sound = std::make_unique<sf::Sound>();
                t_id = m_lastID;
                ++m_lastID;
                ++m_numSounds;
                t_sound->setBuffer(*m_audioManager->GetResource(t_audioName));
                
                return true;
            }
        }

        std::cerr << "[SoundManager] Failed to create sound." << std::endl;

        return false;
    }

    void SoundManager::SetUpSound(sf::Sound* t_snd, const SoundProps* t_props, bool t_loop, bool t_relative)
    {
        t_snd->setVolume(t_props->m_volume);
        t_snd->setPitch(t_props->m_pitch);
        t_snd->setMinDistance(t_props->m_minDistance);
        t_snd->setAttenuation(t_props->m_attenuation);
        t_snd->setLoop(t_loop);
        t_snd->setRelativeToListener(t_relative);
    }

    void SoundManager::RecycleSound(const SoundID& t_id, std::unique_ptr<sf::Sound> t_snd, const std::string& t_name)
    {
        m_recycled.emplace_back(std::make_pair(t_id, t_name), std::move(t_snd));
    }
}
