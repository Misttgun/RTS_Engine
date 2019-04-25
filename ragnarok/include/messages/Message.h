#pragma once

namespace ragnarok
{
    using MessageType = unsigned int;

    struct TwoFloats
    {
        float m_x; float m_y;
    };


    /**
     * \brief Represent a message sent to an observer.
     */
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