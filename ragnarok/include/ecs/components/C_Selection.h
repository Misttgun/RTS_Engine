#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    /**
    * \brief Health component
    */
    class C_Selection : public C_Base
    {
    public:
        C_Selection() : C_Base(Component::Selection), m_showSelection(true)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_size;
        }

        float GetSize() const
        {
            return m_size;
        }

        void SetSize(const float t_size)
        {
            m_size = t_size;
        }

        void SetSelection(bool t_show)
        {
            m_showSelection = t_show;
        }

        bool ShowSelection() const
        {
            return m_showSelection;
        }

    private:
        float m_size;
        bool m_showSelection;
    };
}