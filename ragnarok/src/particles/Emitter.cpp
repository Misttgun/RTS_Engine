#include "../../include/particles/Emitter.h"
#include "../../include/particles/ParticleSystem.h"

namespace ragnarok
{

    Emitter::Emitter(const sf::Vector3f& t_position, const int t_maxParticles) 
    : m_emitRate(0), m_maxParticles(t_maxParticles), m_position(t_position), m_accumulator(0.f), m_system(nullptr)
    {}

    void Emitter::Update(const float t_dT, ParticleContainer* t_particles)
    {
        if (m_generators.empty())
        {
            return;
        }

        const auto generatorList = m_system->GetGenerators(m_generators);
        if (!generatorList)
        {
            return;
        }

        m_accumulator += t_dT * m_emitRate;
        if (m_accumulator < 1.f)
        {
            return;
        }

        auto num_particles = static_cast<int>(m_accumulator);
        m_accumulator -= num_particles;
        if (m_maxParticles != -1)
        {
            if (num_particles > m_maxParticles)
            {
                num_particles = m_maxParticles; 
                m_maxParticles = 0;
            }
            else
            {
                m_maxParticles -= num_particles;
            }
        }

        const size_t from = t_particles->m_countAlive;
        const size_t to = (t_particles->m_countAlive + num_particles > t_particles->Max_Particles
                     ? t_particles->Max_Particles - 1 : t_particles->m_countAlive + num_particles - 1);

        for (auto& generator : *generatorList)
        {
            generator->Generate(this, t_particles, from, to);
        }

        for (auto i = from; i <= to; ++i)
        {
            t_particles->Enable(i);
        }

        if (!m_maxParticles)
        {
            m_system->RemoveEmitter(this);
        }
    }

    void Emitter::SetPosition(const sf::Vector3f& t_position)
    {
        m_position = t_position;
    }

    sf::Vector3f Emitter::GetPosition() const
    {
        return m_position;
    }

    void Emitter::SetEmitRate(size_t t_nPerSecond)
    {
        m_emitRate = t_nPerSecond;
    }

    size_t Emitter::GetEmitRate() const
    {
        return m_emitRate;
    }

    void Emitter::SetGenerators(const std::string& t_generators)
    {
        m_generators = t_generators;
    }

    void Emitter::SetParticleSystem(ParticleSystem* t_system)
    {
        m_system = t_system;
    }

    std::string Emitter::GetGenerators()const
    {
        return m_generators;
    }

    ParticleSystem* Emitter::GetParticleSystem() const
    {
        return m_system;
    }
}
