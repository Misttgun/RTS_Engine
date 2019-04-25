#include "../../include/sprites/AnimBase.h"
#include "../../include/sprites/SpriteSheet.h"

namespace ragnarok
{
    AnimBase::AnimBase() : m_frameCurrent(0), m_frameStart(0), m_frameEnd(0), m_frameRow(0),
        m_frameTime(0), m_elapsedTime(0), m_frameActionStart(-1), m_frameActionEnd(-1), m_loop(false),
        m_playing(false), m_hasMoved(false)
    {}

    AnimBase::~AnimBase()
    {}

    void AnimBase::SetSpriteSheet(SpriteSheet* t_sheet)
    {
        m_spriteSheet = t_sheet;
    }

    bool AnimBase::SetFrame(Frame t_frame)
    {
        if ((t_frame >= m_frameStart && t_frame <= m_frameEnd) || (t_frame >= m_frameEnd && t_frame <= m_frameStart))
        {
            m_frameCurrent = t_frame;
            m_hasMoved = true;
            return true;
        }

        return false;
    }

    void AnimBase::SetStartFrame(Frame t_frame)
    {
        m_frameStart = t_frame;
    }

    void AnimBase::SetEndFrame(Frame t_frame)
    {
        m_frameEnd = t_frame;
    }

    void AnimBase::SetFrameRow(unsigned int t_row)
    {
        m_frameRow = t_row;
    }

    void AnimBase::SetActionStart(Frame t_frame)
    {
        m_frameActionStart = t_frame;
    }

    void AnimBase::SetActionEnd(Frame t_frame)
    {
        m_frameActionEnd = t_frame;
    }

    void AnimBase::SetFrameTime(float t_time)
    {
        m_frameTime = t_time;
    }

    void AnimBase::SetLooping(bool t_loop)
    {
        m_loop = t_loop;
    }

    void AnimBase::SetName(const std::string& t_name)
    {
        m_name = t_name;
    }

    SpriteSheet* AnimBase::GetSpriteSheet()
    {
        return m_spriteSheet;
    }

    Frame AnimBase::GetFrame() const
    {
        return m_frameCurrent;
    }

    Frame AnimBase::GetStartFrame() const
    {
        return m_frameStart;
    }

    Frame AnimBase::GetEndFrame() const
    {
        return m_frameEnd;
    }

    unsigned int AnimBase::GetFrameRow() const
    {
        return m_frameRow;
    }

    int AnimBase::GetActionStart() const
    {
        return m_frameActionStart;
    }

    int AnimBase::GetActionEnd() const
    {
        return m_frameActionEnd;
    }

    float AnimBase::GetFrameTime() const
    {
        return m_frameTime;
    }

    float AnimBase::GetElapsedTime() const
    {
        return m_elapsedTime;
    }

    std::string AnimBase::GetName() const
    {
        return m_name;
    }

    bool AnimBase::IsLooping() const
    {
        return m_loop;
    }

    bool AnimBase::IsPlaying() const
    {
        return m_playing;
    }

    bool AnimBase::IsInAction() const
    {
        if (m_frameActionStart == -1 || m_frameActionEnd == -1)
        {
            return true;
        }

        return (m_frameCurrent >= static_cast<Frame>(m_frameActionStart) && m_frameCurrent <= static_cast<Frame>(m_frameActionEnd));
    }

    bool AnimBase::CheckMoved()
    {
        bool result = m_hasMoved;
        m_hasMoved = false;
        return result;
    }

    void AnimBase::Play()
    {
        m_playing = true;
    }

    void AnimBase::Pause()
    {
        m_playing = false;
    }

    void AnimBase::Stop()
    {
        m_playing = false; Reset();
    }

    void AnimBase::Reset()
    {
        m_frameCurrent = m_frameStart;
        m_elapsedTime = 0.0f;
        CropSprite();
    }

    void AnimBase::Update(float t_dT)
    {
        if (!m_playing)
        {
            return;
        }

        m_elapsedTime += t_dT;

        if (m_elapsedTime < m_frameTime)
        {
            return;
        }

        FrameStep();
        CropSprite();
        m_elapsedTime = 0;
    }
}
