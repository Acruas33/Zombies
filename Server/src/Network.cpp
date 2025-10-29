#include "Network.h"
#include "PacketBuilder.h"
#include <iostream>
#include "Game.h"
#include "Enemy.h"
#include "Projectile.h"

int Network::init(char* ip)
{
    if (isInitialized())
    {
		throw std::runtime_error("Network already initialized!");
    }
    instance = new Network();

    std::cout << "Network host started." << std::endl;
    // Implement network hosting logic here
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet!\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    //address.host = ENET_HOST_ANY;
    //address.host = ip;
    enet_address_set_host(&address, ip);
	//enet_address_set_host(&address, "10.0.0.178");
    address.port = 6969;

    std::cout << "ENet initialized successfully.\n";

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet server host!\n");
        return EXIT_FAILURE;
    }

    networkThread.emplace(networkHost);
}

bool Network::isInitialized()
{
    return instance != nullptr;
}

void Network::sendPacket(ENetPeer* peer, ENetPacket* packet)
{
    enet_peer_send(peer, 0, packet);
}

uint32_t Network::readUInt32(const uint8_t* data, size_t& offset)
{
    uint32_t value;
    std::memcpy(&value, data + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    return value;
}


void Network::handlePacket(ENetPacket* packet)
{
    if (packet->dataLength < 1) return;
    size_t offset = 0;
    uint32_t tickID = readUInt32(packet->data, offset);

    uint8_t size = packet->data[offset];
	offset += sizeof(uint8_t);
    
    std::lock_guard<std::mutex> lock(Game::pendingMutex);
    for (int i = 0; i < size; i++) 
    {
        MessageType messageType;
		std::memcpy(&messageType, packet->data + offset, sizeof(MessageType));
		offset += sizeof(MessageType);

        switch (messageType)
        {
            case MessageType::CONNECT: 
            {
                //This is never actually used I dont think because clients dont send up a packet for connecting its handled in the handleConnection function.
                std::cout << "Received CONNECT message." << std::endl;
                ClientPacket data;
                std::memcpy(&data, packet->data + offset, sizeof(ClientPacket));
                offset += sizeof(ClientPacket);
                if (data.clientID == 0)
                {
                    data.clientID = clientCount; //TODO eventually send usernames or other data and create a map of each clientID with information relevant about that player?
                }
                break;
            }
            case MessageType::OBJECT: 
            { //spawn object
                ObjectPacket data;
                std::memcpy(&data, packet->data + offset, sizeof(ObjectPacket));
                offset += sizeof(ObjectPacket);
                if (data.objectID == -1) {
					std::cout << "Receiving Projectile Packet from client: " << data.clientID << std::endl;
                }
                Game::pendingUpdates.push(data);
                break;
            }
            case MessageType::CHAT: 
            {
                CommandPacket data;
				std::memcpy(&data, packet->data + offset, sizeof(CommandPacket));
				offset += sizeof(CommandPacket);
                Game::pendingUpdates.push(data);
                break;
            }
        }
    }

}

void Network::broadcastPacket(ENetPacket* packet)
{
	enet_host_broadcast(server, 0, packet);
}

void Network::handleConnection(ENetEvent event)
{
	ClientPacket clientData{ clientCount };
	ObjectPacket data{ clientCount, ++Network::networkID, ObjectType::PLAYER, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, 100.0f };

    std::lock_guard<std::mutex> lock(Game::pendingMutex);
    Game::pendingUpdates.push(data);

    PacketBuilder* pb = new PacketBuilder();
	pb->write(clientData);
    //pb->write(data);
	ENetPacket* packet = pb->build();

    sendPacket(event.peer, packet);
}

void Network::sendServerUpdate()
{
    if (Network::packetTimer <= 0.0)
    {
        Network::packetTimer = 1.0 / 60.0; // Reset packet timer

        for (GameObject* go : Game::gameObjects)
        {
            if (go->updated)
            {
                ObjectPacket pkt = { go->clientID, go->networkID, go->objectType, go->m_pos.x, go->m_pos.y, go->m_velocity.x, go->m_velocity.y, go->m_rotation, go->health }; // client ID is like the owner of the object basically 0 for server
                Network::pb->write(pkt);
                go->updated = false;
            }
        }

        Network::broadcastPacket(Network::pb->build());
    }
}

void Network::handleClientUpdates()
{
    std::lock_guard<std::mutex> lock(Game::pendingMutex);

    // Handle incoming network updates
    while (!Game::pendingUpdates.empty())
    {
        PacketVariant packet = std::move(Game::pendingUpdates.front());
        Game::pendingUpdates.pop();

        std::visit([](auto& p)
        {
            using T = std::decay_t<decltype(p)>;

            if constexpr (std::is_same_v<T, ObjectPacket>) //create/spawn/update object Pretty sure we wont be using this right now on the server at all but may eventually so yea.
            {
                bool exists = false;
                for (GameObject* go : Game::gameObjects)
                {
                    if (go->networkID == p.objectID && p.objectType != ObjectType::PROJECTILE)
                    {
                        //update
                        glm::vec2 packetPos = glm::vec2(p.x, p.y);
                        go->m_pos = packetPos;
                        go->m_rotation = p.rotation;
                        exists = true;
                        break;
                    }
                }

                if (!exists)
                {
                    switch (p.objectType)
                    {
                    case ObjectType::PLAYER:
                    {
                        Player* player = new Player(glm::vec2(p.x, p.y), glm::vec2(32.0f, 32.0f));
                        player->m_scale = 1.5f;
                        player->m_rotation = p.rotation;
                        player->networkID = p.objectID;
                        player->clientID = p.clientID;
                        player->m_velocity.x = p.dx;
                        player->m_velocity.y = p.dy;
                        player->health = p.health;
                        player->updated = true;
                        break;
                    }
                    case ObjectType::PROJECTILE:
                    {
                        Projectile* projectile = new Projectile(glm::vec2(p.x, p.y), glm::vec2(32.0f, 32.0f), glm::vec3(1.0f), p.rotation, 1.5f, glm::vec2(p.dx, p.dy));
                        projectile->updated = true;
                        projectile->networkID = p.objectID;
                        projectile->clientID = p.clientID;
                        projectile->health = p.health;
                        //std::cout << "Projectile created on the Server with clientID: " << p.clientID << " networkID: " << p.objectID << std::endl;
                        break;
                    }
                    }
                }

            }
            else if constexpr (std::is_same_v<T, CommandPacket>)
            {
                if (p.commandType == CommandType::SPAWNENEMY) 
                {
					std::cout << "spawn toggled" << std::endl;
                    Game::spawnEnemies = !Game::spawnEnemies;
                }
                else if (p.commandType == CommandType::STARTGAME) 
                {
                    std::cout << "starting game" << std::endl;
                    //handleConnection();
                }
            }

        }, packet);
    }
}

void Network::networkHost()
{
    while (true)
    {
        while (enet_host_service(server, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT: {

                printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);

                clientCount++;

				handleConnection(event);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:

                handlePacket(event.packet);

                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "A client disconnected." << std::endl;
                event.peer->data = NULL;
                break;
            default:
                break;
            }
        }
    }

    enet_host_destroy(server);
    printf("ENet server host destroyed.\n");
}