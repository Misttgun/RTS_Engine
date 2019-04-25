#pragma once

#include <string>
#include <SFML/System/Vector3.hpp>

namespace ragnarok
{
    class ParticleSystem;
    class ParticleContainer;

    class Emitter
    {
    public:
        Emitter(const sf::Vector3f& t_position, int t_maxParticles = -1);
        void Update(float t_dT, ParticleContainer* t_particles);
        void SetPosition(const sf::Vector3f& t_position);
        sf::Vector3f GetPosition() const;
        size_t GetEmitRate() const;

        void SetEmitRate(size_t t_nPerSecond);
        void SetParticleSystem(ParticleSystem* t_system);
        void SetGenerators(const std::string& t_generators);
        std::string GetGenerators() const;
        ParticleSystem* GetParticleSystem() const;

    private:
        std::string m_generators;
        size_t m_emitRate;
        int m_maxParticles;
        sf::Vector3f m_position;
        float m_accumulator;
        ParticleSystem* m_system;
    };
}