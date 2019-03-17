#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    /**
     * \brief Controller component
     */
    class C_Controller : public C_Base
    {
        public:
        C_Controller() : C_Base(Component::Controller)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {}
    };
}