#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

namespace SExE
{
    using MessageType = unsigned int;
    
    struct TwoFloats
    {
        float m_x; float m_y;
    };

    struct Message
    {
        Message(const MessageType& t_type) : m_type(t_type)
        {}

        MessageType m_type;
        int m_sender;
        int m_receiver;

        union
        {
            TwoFloats m_2f;
            bool m_bool;
            int m_int;
        };
    };
}

#endif // !MESSAGE_H