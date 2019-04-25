#pragma once

#include "../core/S_Base.h"
#include "../components/C_SoundEmitter.h"

namespace ragnarok
{
    /**
     * \brief Sound system
     */
    class S_Sound : public S_Base
    {
        public:
        S_Sound(SystemManager* t_systemMgr);
        ~S_Sound() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        void SetUp(AudioManager* t_audioManager, SoundManager* t_soundManager);

        private:
        /**
         * \brief Position a sound in space
         * \param t_entityPos the entity position
         * \param t_elevation the entity elevation
         * \return the sound position
         */
        sf::Vector3f MakeSoundPosition(const sf::Vector2f& t_entityPos, unsigned int t_elevation) const;
        void EmitSound(const EntityId& t_entity, const EntitySound& t_sound, bool t_useId, bool t_relative, int t_checkFrame = -1) const;
        AudioManager* m_audioManager;
        SoundManager* m_soundManager;
    };
}