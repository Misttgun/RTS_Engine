#pragma once

#include "../BaseGenerator.h"
#include "../Emitter.h"
#include "../ParticleSystem.h"

namespace ragnarok
{
    class AreaPosition : public BaseGenerator
    {
    public:
        AreaPosition() = default;

        AreaPosition(const sf::Vector3f& t_deviation) : m_deviation(t_deviation)
        {}

        void Generate(Emitter* t_emitter, ParticleContainer* t_particles, size_t t_from, size_t t_to) override
        {
            auto& positions = t_particles->m_position;
            const auto center = t_emitter->GetPosition();
            const auto rangeFrom = sf::Vector3f(center.x - m_deviation.x, center.y - m_deviation.y, center.z - m_deviation.z);
            const auto rangeTo = sf::Vector3f(center.x + m_deviation.x, center.y + m_deviation.y, center.z + m_deviation.z);
            auto& rand = *t_emitter->GetParticleSystem()->GetRand();

            for (auto i = t_from; i <= t_to; ++i)
            {
                positions[i] = sf::Vector3f(rand(rangeFrom.x, rangeTo.x),rand(rangeFrom.y, rangeTo.y),rand(rangeFrom.z, rangeTo.z));
            }
        }

        void ReadIn(std::stringstream& t_stream) override
        {
            t_stream >> m_deviation.x >> m_deviation.y >> m_deviation.z;
        }

    private:
        sf::Vector3f m_deviation;
    };
}