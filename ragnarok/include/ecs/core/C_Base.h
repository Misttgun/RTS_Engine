#pragma once

#include <sstream>
#include "../ECSTypes.h"

namespace ragnarok
{
    /**
     * \brief Base class for all the components.
     */
    class C_Base
    {
        public:
        C_Base(const Component& t_type) : m_type(t_type)
        {}

        virtual ~C_Base() = default;

        Component GetType() const
        {
            return m_type;
        }

        friend std::stringstream& operator >>(std::stringstream& t_stream, C_Base& b)
        {
            b.ReadIn(t_stream);
            return t_stream;
        }

        /**
         * \brief Quick way to read component data in from file
         * \param t_stream file from which to read
         */
        virtual void ReadIn(std::stringstream& t_stream) = 0;

        protected:
        Component m_type;
    };
}