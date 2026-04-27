#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <optional>
#include <algorithm>
#include "game_logic.h"
#include "game.pb.h"

// Configuration du serveur
const std::uint16_t SERVER_PORT = 50000;
const float TICK_RATE_SECONDS = 1.0f / 30.0f; // 30 FPS

struct ConnectedClient
{
    sf::IpAddress address;
    std::uint16_t port;
    Side side;
};

int main()
{
    sf::UdpSocket socket;
    if (socket.bind(SERVER_PORT) != sf::Socket::Status::Done)
    {
        std::cerr << "Critical Error: Could not bind port " << SERVER_PORT << std::endl;
        return 1;
    }
    socket.setBlocking(false);

    std::cout << "--- Royal Arena Battle Server ---" << std::endl;
    std::cout << "Listening on port " << SERVER_PORT << std::endl;

    Battle battle(false); // false = isServer
    sf::Clock clock;
    std::vector<ConnectedClient> clients;

    while (true)
    {
        float dt = clock.restart().asSeconds();
        battle.update(dt);

        // 1. Réception et traitement des paquets
        sf::Packet packet;
        std::optional<sf::IpAddress> remoteIp;
        std::uint16_t remotePort = 0;

        while (socket.receive(packet, remoteIp, remotePort) == sf::Socket::Status::Done)
        {
            if (!remoteIp)
                continue;

            // Identification du client
            auto it = std::find_if(clients.begin(), clients.end(), [&](const ConnectedClient &c)
                                   { return c.address == *remoteIp && c.port == remotePort; });

            Side senderSide;
            if (it == clients.end())
            {
                // Nouveau joueur ? (Max 2 joueurs)
                if (clients.size() < 2)
                {
                    senderSide = (clients.empty()) ? Side::PLAYER : Side::ENEMY;
                    clients.push_back({*remoteIp, remotePort, senderSide});
                    std::cout << "[Connect] " << remoteIp->toString() << ":" << remotePort
                              << " assigned to " << (senderSide == Side::PLAYER ? "PLAYER 1" : "PLAYER 2") << std::endl;
                }
                else
                {
                    continue; // Serveur plein
                }
            }
            else
            {
                senderSide = it->side;
            }

            // Traitement des données
            std::string data;
            packet >> data;

            // Essayer de parser une SpawnRequest
            game::SpawnRequest spawnReq;
            if (spawnReq.ParseFromString(data))
            {
                sf::Vector2f pos(spawnReq.position().x(), spawnReq.position().y());
                battle.handleServerSpawn(senderSide, pos, spawnReq.card_index());
            }
        }

        // 2. Diffusion de l'état du jeu (Broadcast)
        if (!clients.empty())
        {
            std::string stateData = battle.serialize();
            sf::Packet statePacket;
            statePacket << stateData;

            for (const auto &client : clients)
            {
                socket.send(statePacket, client.address, client.port);
            }
        }

        // 3. Contrôle de la charge CPU (Tick rate)
        sf::sleep(sf::seconds(TICK_RATE_SECONDS));
    }

    return 0;
}
