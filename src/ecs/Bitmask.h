#pragma once
#ifndef BITMASK_H
#define BITMASK_H

#include <stdint.h>

namespace SExE
{
    using Bitset = uint32_t;

    class Bitmask
    {
        public:
        Bitmask() : bits(0)
        {}

        Bitmask(const Bitset& t_bits) : bits(t_bits)
        {}

        Bitset getMask() const
        {
            return bits;
        }

        void setMask(const Bitset& t_value)
        {
            bits = t_value;
        }

        bool matches(const Bitmask& t_bits, const Bitset& t_relevant = 0) const
        {
            return(t_relevant ? ((t_bits.getMask() & t_relevant) == (bits & t_relevant)) : (t_bits.getMask() == bits));
        }

        bool getBit(unsigned int t_pos) const
        {
            return ((bits&(1 << t_pos)) != 0);
        }

        void turnOnBit(unsigned int t_pos)
        {
            bits |= 1 << t_pos;
        }

        void turnOnBits(const Bitset& t_bits)
        {
            bits |= t_bits;
        }

        void clearBit(unsigned int t_pos)
        {
            bits &= ~(1 << t_pos);
        }

        void toggleBit(unsigned int t_pos)
        {
            bits ^= 1 << t_pos;
        }

        void clear()
        {
            bits = 0;
        }

        private:
        Bitset bits;
    };
}

#endif // !BITMASK_H
