//
// Created by florian on 30/06/19.
//

#include "../../include/network/Server.h"
#include "../../include/network/Command.h"
#include "../../include/ecs/core/EntityManager.h"
#include "../../include/ecs/components/C_Position.h"
#include "../../include/map/Map.h"
#include <sstream>
#include <iomanip>

ragnarok::Server::Server(SharedContext *t_context, UnitManager *t_unitManager) :
        m_connected(false),
        m_remoteAddress("127.0.0.1"),
        m_remotePort(4640),
        m_context(t_context),
        m_unitManager(t_unitManager)
{}

sf::Socket::Status ragnarok::Server::connect() {
    return connect(m_remoteAddress, m_remotePort);
}

sf::Socket::Status ragnarok::Server::connect(const sf::IpAddress &remoteAddress, unsigned short remotePort) {
    sf::Socket::Status status = m_socket.connect(remoteAddress, remotePort, sf::seconds(60));
    if (status == sf::Socket::Done)
    {
        m_connected = true;
        m_socket.setBlocking(false);
    }

    return status;
}

ragnarok::Server::~Server() {
    m_socket.disconnect();
}

bool ragnarok::Server::processIncomingData() {
    char data[PACKET_SIZE];
    std::size_t received;

    sf::Socket::Status status = m_socket.receive(data, PACKET_SIZE, received);

    if (status == sf::Socket::NotReady)
    {
        return true;
    }
    else if (status == sf::Socket::Done)
    {
        std::istringstream iss(data);
        std::string command;

        if (!(iss >> command))
        {
            return false;
        }

        if (command == "COMM")
        {
            std::size_t sent;

            if (m_sendQueue.empty())
            {
                m_socket.send("NOOP 000 000 000", 16, sent);
            }
            else
            {
                std::string toSend = m_sendQueue.front();
                m_sendQueue.pop();
                m_socket.send(toSend.c_str(), 16, sent);
            }

            return true;
        }
        else if (command == "MOVE")
        {
            sf::Uint16 entityID;
            sf::Uint16 destinationX;
            sf::Uint16 destinationY;
            if (!(iss >> entityID >> destinationX >> destinationY))
            {
                return false;
            }

            m_unitManager->setDestination(entityID, destinationX, destinationY);

            return true;
        }
        else if (command == "ATTQ")
        {
            sf::Uint16 entityID;
            sf::Uint16 attackedEntityID;
            if (!(iss >> entityID >> attackedEntityID))
            {
                return false;
            }

            return true;
        }
        else if (command == "CREA")
        {
            sf::Uint16 entityType;
            sf::Uint16 positionX;
            sf::Uint16 positionY;
            if (!(iss >> entityType >> positionX >> positionY))
            {
                return false;
            }

            const std::string entityTypeName = ENTITY_TYPES[entityType];
            const int entityId = m_context->m_entityManager->AddEntity(entityTypeName);
            const auto pos = m_context->m_entityManager->GetComponent<ragnarok::C_Position>(entityId, ragnarok::Component::Position);

            auto t_pos = m_context->m_gameMap->GetDiscreteMap()->setSquare(positionX, positionY, entityId);

            pos->SetPosition(t_pos.x, t_pos.y);
            pos->SetOccupiedMapPosition({positionX, positionY});

            m_unitManager->AddUnit(entityId, t_pos.x, t_pos.y);

            return true;
        }
        else if (command == "NOOP")
        {
            return true;
        }
    }

    return false;
}

void ragnarok::Server::enqueueAction(const std::string& command, sf::Uint16 data1, sf::Uint16 data2, sf::Uint16 data3) {
    std::ostringstream oss;

    oss << std::left << std::setfill('0') << std::setw(3) << command << ' ' << data1 << ' ' << data2 << ' ' << data3;
    m_sendQueue.push(oss.str());
}

bool ragnarok::Server::ProcessLine(std::stringstream &t_stream) {
    std::string type;
    if (!(t_stream >> type))
    {
        return false;
    }

    if (type == "ADDRESS")
    {
        if (!(t_stream >> m_remoteAddress))
        {
            return false;
        }
    }
    else if (type == "PORT")
    {
        if(!(t_stream >> m_remotePort))
        {
            return false;
        }
    }

    return true;
}

bool ragnarok::Server::isConnected() const {
    return m_connected;
}
