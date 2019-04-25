#pragma once

#include <cstdint>

namespace ragnarok
{
    using Bitset = uint32_t;


    /**
     * \brief Data structure representing the makeup of an entity
     */
    class Bitmask
    {
        public:
        Bitmask() : bits(0)
        {}

        Bitmask(const Bitset& t_bits) : bits(t_bits)
        {}

        Bitset GetMask() const
        {
            return bits;
        }

        void SetMask(const Bitset& t_value)
        {
            bits = t_value;
        }

        bool Matches(const Bitmask& t_bits, const Bitset& t_relevant = 0) const
        {
            return (t_relevant ? ((t_bits.GetMask() & t_relevant) == (bits & t_relevant)) : (t_bits.GetMask() == bits));
        }

        bool GetBit(unsigned int t_pos) const
        {
            return ((bits&(1 << t_pos)) != 0);
        }

        void TurnOnBit(unsigned int t_pos)
        {
            bits |= 1 << t_pos;
        }

        void TurnOnBits(const Bitset& t_bits)
        {
            bits |= t_bits;
        }

        void ClearBit(unsigned int t_pos)
        {
            bits &= ~(1 << t_pos);
        }

        void ToggleBit(unsigned int t_pos)
        {
            bits ^= 1 << t_pos;
        }

        void Clear()
        {
            bits = 0;
        }

        private:
        Bitset bits;
    };
}