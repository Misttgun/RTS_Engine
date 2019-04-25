#pragma once

namespace ragnarok
{
    class BaseUpdater
    {
    public:
        virtual ~BaseUpdater() = default;

        virtual void Update(float t__d_t, ParticleContainer* t_particles) = 0;
    };
}