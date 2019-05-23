#pragma once

#include "../core/C_Base.h"

namespace ragnarok
{
    /**
     * \brief Ressource component
     */
    class C_Ressource : public C_Base
    {
    public:
        C_Ressource() : C_Base(Component::Ressource)
        {}

        void ReadIn(std::stringstream& t_stream) override
        {}
    };
}