#pragma once

#include <sstream>

namespace ragnarok
{
    class Emitter;

    class BaseGenerator
    {
    public:
        virtual ~BaseGenerator() = default;

        virtual void Generate(Emitter* t_emitter, ParticleContainer* t_particles, size_t t_from, size_t t_to) = 0;

        friend std::stringstream& operator >> (std::stringstream& t_stream, BaseGenerator& b)
        {
            b.ReadIn(t_stream);
            return t_stream;
        }

        virtual void ReadIn(std::stringstream& t_stream)
        {}
    };
}