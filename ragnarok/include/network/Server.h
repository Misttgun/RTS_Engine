//
// Created by florian on 30/06/19.
//

#ifndef RTS_ENGINE_SERVER_H
#define RTS_ENGINE_SERVER_H

#include <SFML/Network.hpp>
#include <queue>
#include "../threads/FileLoader.h"
#include "../SharedContext.h"
#include "../../../game/include/UnitManager.h"

namespace ragnarok
{
    static const std::size_t PACKET_SIZE = 16; // Example : "COMM 000 000 000"
    static const std::vector<std::string> ENTITY_TYPES{"Peasant", "Soldier", "Archer"};

    class Server : public FileLoader
    {
    public:
        Server(SharedContext *t_context, UnitManager *t_unitManager);
        sf::Socket::Status connect();
        sf::Socket::Status connect(const sf::IpAddress&, unsigned short);
        bool processIncomingData();
        void enqueueAction(const std::string &command, sf::Uint16 data1, sf::Uint16 data2, sf::Uint16 data3);
        ~Server();

        bool isConnected() const;

    protected:
        bool ProcessLine(std::stringstream &) override;

    private:
        sf::TcpSocket m_socket;
        sf::IpAddress m_remoteAddress;
        sf::Uint16 m_remotePort;
        SharedContext *m_context;
        UnitManager *m_unitManager;
        bool m_connected;

        std::queue<std::string> m_sendQueue;

    };
}

#endif //RTS_ENGINE_SERVER_H
