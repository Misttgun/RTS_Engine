#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    /**
     * \brief Sound listener component
     */
    class C_SoundListener : public C_Base
    {
        public:
        C_SoundListener() : C_Base(Component::SoundListener)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {}
    };
}