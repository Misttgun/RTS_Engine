#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <SFML/Audio.hpp>
#include "SoundProps.h"
#include "../resources/AudioManager.h"
#include "../utilities/Utilities.h"
#include "../states/StateDependent.h"

namespace ragnarok
{
	    using SoundID = int;

    enum class StateType;

    struct SoundInfo
    {
        SoundInfo(const std::string& t_name) : m_name(t_name), m_manualPaused(false)
        {}

        std::string m_name;
        bool m_manualPaused;
    };

    using SoundProperties = std::unordered_map<std::string, SoundProps>;
    using SoundContainer = std::unordered_map<SoundID, std::pair<SoundInfo, std::unique_ptr<sf::Sound>>>;
    using Sounds = std::unordered_map<StateType, SoundContainer>;
    using RecycledSounds = std::vector<std::pair<std::pair<SoundID, std::string>, std::unique_ptr<sf::Sound>>>;
    using MusicContainer = std::unordered_map<StateType, std::pair<SoundInfo, std::unique_ptr<sf::Music>>>;

	/**
	 * \brief Class for using and managing sounds.
	 */
    class SoundManager : public StateDependent
    {
        public:
        SoundManager(AudioManager* t_audioMgr);
        ~SoundManager();

        void ChangeState(const StateType& t_state);
        void RemoveState(const StateType& t_state);

        void Update(float t_dT);

		/**
		 * \brief Play a soundProps from its name
		 * \param t_sound name of the sound props
		 * \param t_position location of the sound
		 * \param t_loop bool stating whether the sound loops or not
		 * \param t_relative ????????????????????????
		 */
        SoundID Play(const std::string& t_sound, const sf::Vector3f& t_position, bool t_loop = false, bool t_relative = false);
		/**
		 * \brief Play a sound from its id
		 * \param t_id id of the sound
		 */
        bool Play(const SoundID& t_id);
        bool Stop(const SoundID& t_id);
        bool Pause(const SoundID& t_id);

        bool PlayMusic(const std::string& t_musicId, float t_volume = 100.f, bool t_loop = false);
        bool PlayMusic(const StateType& t_state);
        bool StopMusic(const StateType& t_state);
        bool PauseMusic(const StateType& t_state);

		/**
		 * \brief Setup the triD location of a sound
		 * \param t_id id of the sound
		 * \param t_pos location where the sound must come from
		 */
        bool SetPosition(const SoundID& t_id, const sf::Vector3f& t_pos);
        bool IsPlaying(const SoundID& t_id) const;
        SoundProps* GetSoundProperties(const std::string& t_soundName);

        static const int Max_Sounds = 150;
        static const int Sound_Cache = 75;

        private:
        bool LoadProperties(const std::string& t_file);
        void PauseAll(const StateType& t_state);
        void UnpauseAll(const StateType& t_state);

		/**
		 * \brief Check if the sounds doesn't already exist, if not, create it
		 * \param t_id id of the sound
		 * \param t_audioName name of the sound
		 * \param t_sound pointer to the empty (for now) sound
		 */
        bool CreateSound(SoundID& t_id, const std::string& t_audioName, std::unique_ptr<sf::Sound>& t_sound);
        void SetUpSound(sf::Sound* t_snd, const SoundProps* t_props, bool t_loop = false, bool t_relative = false);
		/**
		 * \brief Recycle and unused sound
		 * \param t_id id of the sound
		 * \param t_name name of the sound
		 * \param t_snd pointer to the sound
		 */
        void RecycleSound(const SoundID& t_id, std::unique_ptr<sf::Sound> t_snd, const std::string& t_name);

        void Cleanup();

        Sounds m_audio;
        MusicContainer m_music;
        RecycledSounds m_recycled;
        SoundProperties m_properties;

        SoundID m_lastID;
        unsigned int m_numSounds;
        float m_elapsed;

        AudioManager* m_audioManager;
    };
}