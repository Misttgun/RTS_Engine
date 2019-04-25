#pragma once

#include "../BaseGenerator.h"
#include "../Emitter.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    enum class LineAxis
    {
        x, y, z
    };

    class LinePosition : public BaseGenerator
    {
    public:
        LinePosition() : m_axis(LineAxis::x), m_deviation(0.f)
        {}

        LinePosition(const LineAxis t_axis, const float t_deviation) : m_axis(t_axis), m_deviation(t_deviation)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, const size_t t_from, const size_t t_to) override
        {
            auto& positions = t_particles->m_position;
            auto center = t_emitter->GetPosition();
            auto& rand = *t_emitter->GetParticleSystem()->GetRand();

            for (auto i = t_from; i <= t_to; ++i)
            {
                if (m_axis == LineAxis::x)
                {
                    center.x = rand(center.x - m_deviation, center.x + m_deviation);
                }
                else if (m_axis == LineAxis::y)
                {
                    center.y = rand(center.y - m_deviation, center.y + m_deviation);
                }
                else
                {
                    center.z = rand(center.z - m_deviation, center.z + m_deviation);
                }

                positions[i] = center;
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            std::string axis;
            t_stream >> axis >> m_deviation;

            if (axis == "x")
            {
                m_axis = LineAxis::x;
            }
            else if (axis == "y")
            {
                m_axis = LineAxis::y;
            }
            else if (axis == "z")
            {
                m_axis = LineAxis::z;
            }
            else
            {
                std::cout << "Faulty axis: " << axis << std::endl;
            }
        }

    private:
        LineAxis m_axis;
        float m_deviation;
    };
}