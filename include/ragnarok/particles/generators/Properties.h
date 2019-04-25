#pragma once

#include "../BaseGenerator.h"

namespace ragnarok
{
    class PropGenerator : public BaseGenerator
    {
    public:
        PropGenerator(const bool t_gravity = true, const bool t_additive = false) : m_gravity(t_gravity), m_additive(t_additive)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& gravity = t_particles->m_gravity;
            for (auto i = t_from; i <= t_to; ++i)
            {
                gravity[i] = m_gravity;
            }

            auto& additive = t_particles->m_addBlend;
            for (auto i = t_from; i <= t_to; ++i)
            {
                additive[i] = m_additive;
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            int gravity = 1;
            int additive = 0;
            t_stream >> gravity >> additive;
            m_gravity = (gravity != 0);
            m_additive = (additive != 0);
        }

    private:
        bool m_gravity;
        bool m_additive;
    };
}