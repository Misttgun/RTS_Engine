#pragma once
#include <array>
#include "../core/C_Base.h"
#include "../../sounds/SoundManager.h"

namespace ragnarok
{
    enum class EntitySound
    {
        None = -1, Footstep, Attack, Hurt, Death, COUNT
    };

    /**
     * \brief Represent sound parmaters such as name, number of soundframes
     */
    struct SoundParameters
    {
        // Maximum possible number of frames that can have sounds attached to them
        static const int Max_SoundFrames = 5;

        SoundParameters()
        {
            for (int i = 0; i < Max_SoundFrames; ++i)
            {
                m_frames[i] = -1;
            }
        }

        std::string m_sound;
        // Sound frame is the glue between sounds and sprite sheets. 
        // It defines during which animation frames the sound is emitted.
        std::array<int, Max_SoundFrames> m_frames;
    };

    /**
     * \brief Sound emitter component
     */
    class C_SoundEmitter : public C_Base
    {
    public:
        C_SoundEmitter() : C_Base(Component::SoundEmitter), m_soundID(-1)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            // Set up delimiter information
            std::string main_delimiter = ":";
            std::string frame_delimiter = ",";

            // Iterate for each possible entity sound
            for (size_t i = 0; i < static_cast<size_t>(EntitySound::COUNT); ++i)
            {
                // Read in the content of the next segment
                std::string chunk;
                t_stream >> chunk;

                if (chunk.empty())
                {
                    break;
                }

                // Split the chunk between frames and sound
                const std::string sound = chunk.substr(0, chunk.find(main_delimiter));
                std::string frames = chunk.substr(chunk.find(main_delimiter) + main_delimiter.length());

                // Set the sound name
                m_params[i].m_sound = sound;

                unsigned int frameNum = 0;
                while (frameNum < SoundParameters::Max_SoundFrames)
                {
                    const size_t pos = frames.find(frame_delimiter);
                    int frame;

                    // If there is a frame delimiter
                    if (pos != std::string::npos)
                    {
                        // Store the frame value and erase frame and delimiter from string
                        frame = stoi(frames.substr(0, pos));
                        frames.erase(0, pos + frame_delimiter.length());
                    }
                    else
                    {
                        // Store the frame value
                        frame = stoi(frames);

                        // Set the frame value and get out of the loop
                        m_params[i].m_frames[frameNum] = frame;
                        break;
                    }

                    // // Set the frame value
                    m_params[i].m_frames[frameNum] = frame;
                    ++frameNum;
                }
            }
        }

        SoundID GetSoundID() const
        {
            return m_soundID;
        }

        void SetSoundID(const SoundID& t_id)
        {
            m_soundID = t_id;
        }

        /**
         * \brief Get the name of the sound to play
         * \param t_snd sound
         * \return the name of the sound
         */
        const std::string& GetSound(const EntitySound& t_snd) const
        {
            static std::string empty;
            return(static_cast<size_t>(t_snd) < static_cast<size_t>(EntitySound::COUNT) ? m_params[static_cast<int>(t_snd)].m_sound : empty);
        }

        /**
         * \brief Tell if the current animation frame should emit a sound
         * \param t_snd the sound
         * \param t_frame the current frame
         * \return true if a sound should be played
         */
        bool IsSoundFrame(const EntitySound& t_snd, int t_frame) const
        {
            if (static_cast<size_t>(t_snd) >= static_cast<size_t>(EntitySound::COUNT))
            {
                return false;
            }

            for (int i = 0; i < SoundParameters::Max_SoundFrames; ++i)
            {
                if (m_params[static_cast<int>(t_snd)].m_frames[i] == -1)
                {
                    return false;
                }

                if (m_params[static_cast<int>(t_snd)].m_frames[i] == t_frame)
                {
                    return true;
                }
            }

            return false;
        }

        SoundParameters* GetParameters()
        {
            return &m_params[0];
        }

    private:
        std::array<SoundParameters, static_cast<size_t>(EntitySound::COUNT)> m_params;
        SoundID m_soundID;
    };
}