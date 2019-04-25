#pragma once

#include <string>

namespace ragnarok
{
    using Frame = unsigned int;

    class SpriteSheet;

    class AnimBase
    {
		/**
		 * \brief Class for setting up, using and manipulating animation from spritesheet
		 */
        friend class SpriteSheet;
        public:
        AnimBase();
        virtual ~AnimBase();

        void SetSpriteSheet(SpriteSheet* t_sheet);

        bool SetFrame(Frame t_frame);
        void SetStartFrame(Frame t_frame);
        void SetEndFrame(Frame t_frame);
        void SetFrameRow(unsigned int t_row);
        void SetActionStart(Frame t_frame);
        void SetActionEnd(Frame t_frame);
        void SetFrameTime(float t_time);
        void SetLooping(bool t_loop);
        void SetName(const std::string& t_name);

        SpriteSheet* GetSpriteSheet();

        Frame GetFrame() const;
        Frame GetStartFrame() const;
        Frame GetEndFrame() const;
        unsigned int GetFrameRow() const;
        int GetActionStart() const;
        int GetActionEnd() const;
        float GetFrameTime() const;
        float GetElapsedTime() const;
        bool IsLooping() const;
        bool IsPlaying() const;
		/**
		 * \brief Check if an animation is occurring
		 */
        bool IsInAction() const;
        bool CheckMoved();
        std::string GetName() const;

        void Play();
        void Pause();
        void Stop();
        void Reset();

		/**
		 * \brief If an animation is still playing, update the sprite via AnimDirectional
		 * \param t_dT time spend since last update
		 */
        virtual void Update(float t_dT);

        friend std::stringstream& operator >>(std::stringstream& t_stream, AnimBase& a)
        {
            a.ReadIn(t_stream);
            return t_stream;
        }

        protected:
        virtual void FrameStep() = 0;
        virtual void CropSprite() = 0;
        virtual void ReadIn(std::stringstream& t_stream) = 0;

        Frame m_frameCurrent;
        Frame m_frameStart;
        Frame m_frameEnd;
        Frame m_frameRow;
        int m_frameActionStart;
        int m_frameActionEnd;
        float m_frameTime;
        float m_elapsedTime;
        bool m_loop;
        bool m_playing;
        bool m_hasMoved;

        std::string m_name;

        SpriteSheet* m_spriteSheet;
    };

}