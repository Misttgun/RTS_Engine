#pragma once

#include <random>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>

namespace ragnarok
{
    class RandomGenerator
    {
        public:
        RandomGenerator() : m_engine(m_device())
        {}

        int Generate(int t_min, int t_max)
        {
            sf::Lock lock(m_mutex);
            
            if(t_min > t_max)
            {
                std::swap(t_min, t_max);
            }

            if(t_min != m_intDistribution.min() || t_max != m_intDistribution.max())
            {
                m_intDistribution = std::uniform_int_distribution<int>(t_min, t_max);
            }

            return m_intDistribution(m_engine);
        }

        float Generate(float t_min, float t_max)
        {
            sf::Lock lock(m_mutex);
            
            if(t_min > t_max)
            {
                std::swap(t_min, t_max);
            }

            if(t_min != m_floatDistribution.min() || t_max != m_floatDistribution.max())
            {
                m_floatDistribution = std::uniform_real_distribution<float>(t_min, t_max);
            }

            return m_floatDistribution(m_engine);
        }

        float operator()(float t_min, float t_max)
        {
            return Generate(t_min, t_max);
        }

        int operator()(int t_min, int t_max)
        {
            return Generate(t_min, t_max);
        }

        private:
        std::random_device m_device;
        std::mt19937 m_engine;
        std::uniform_int_distribution<int> m_intDistribution;
        std::uniform_real_distribution<float> m_floatDistribution;
        sf::Mutex m_mutex;
    };
}