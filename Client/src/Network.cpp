#include "Network.h"
#include <iostream>
#include <optional>
#include <thread>
#include <vector>
#include "Game.h"
#include "ResourceManager.h"
#include "Enemy.h"
#include "Projectile.h"
#include "ObjectPacket.h"
#include "ClientPacket.h"

uint8_t Network::clientID = 0;
ENetHost* Network::client = nullptr;
ENetPeer* Network::peer = nullptr;
ENetAddress Network::address = {};
ENetEvent Network::event = {};
std::optional<std::thread> Network::networkThread;
Network* Network::instance = nullptr;

Network::Network(bool isHost)
    :isHost(isHost)
{

}

Network& Network::getInstance()
{
    if (!instance)
    {
        throw std::runtime_error("Network not initialized! Call Network::init() first.");
    }
    return *instance;
}

bool Network::isInitialized() {
    return instance != nullptr;
}

std::string Network::getLocalIP()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    addrinfo hints = {};
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* info = nullptr;
    getaddrinfo(hostname, NULL, &hints, &info);

    std::string ip = "127.0.0.1";
    for (addrinfo* ptr = info; ptr != nullptr; ptr = ptr->ai_next) {
        sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;

        char str[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &sockaddr_ipv4->sin_addr, str, INET_ADDRSTRLEN);

        ip = str;
        if (ip != "127.0.0.1") break; // pick first non-localhost
    }

    freeaddrinfo(info);
    WSACleanup();
    return ip;
}

int Network::init(bool isHost)
{
    if (isInitialized())
    {
        throw std::runtime_error("Network already initialized!");
    }
    instance = new Network(isHost);

    if (isHost) 
    {
        networkHost();
    }

    // Placeholder for network client logic
    std::cout << "Network client started." << std::endl;
    // Implement network client logic here

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet!\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    client = enet_host_create(NULL, 1, 2, 0, 0);

    if (client == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client.\n");
        return EXIT_FAILURE;
    }

    //enet_address_set_host(&address, "127.0.0.1");
    //enet_address_set_host(&address, "localhost");
	enet_address_set_host(&address, ip.c_str());
    address.port = 6969;

    peer = enet_host_connect(client, &address, 1, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection\n");
        return EXIT_FAILURE;
    }

    networkThread.emplace(networkClient);
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
	// Handle incoming packets here
	// For example, you can parse the packet data and update game state accordingly
	//std::cout << "Received packet of size: " << packet->dataLength << std::endl;

    if (packet->dataLength < 1) return;
    std::lock_guard<std::mutex> lock(Game::pendingMutex);
    
    size_t offset = 0;
    uint32_t tickID = readUInt32(packet->data, offset);
    
    Snapshot snapshot{ tickID, {} };
    latestServerTick = snapshot.tickID;

    //std::cout << "Received packet with tick ID: " << static_cast<int>(snapshot.tickID) << std::endl;
    uint8_t size = packet->data[offset];
    offset += sizeof(uint8_t);

    for (int i = 0; i < size; i++)
    {
        MessageType messageType;
        std::memcpy(&messageType, packet->data + offset, sizeof(MessageType));
        offset += sizeof(MessageType);

        switch (messageType)
        {
            case MessageType::CONNECT: {
                std::cout << "Received CONNECT message." << std::endl;
                // Handle connect logic here
                ClientPacket data;
				std::memcpy(&data, packet->data + offset, sizeof(ClientPacket));
				offset += sizeof(ClientPacket);
                if(clientID == 0)
                    clientID = data.clientID; //TODO eventually send usernames or other data and create a map of each clientID with information relevant about that player?
                break;
            }
            case MessageType::OBJECT: { //spawn object
                ObjectPacket data;
                std::memcpy(&data, packet->data + offset, sizeof(ObjectPacket));
				offset += sizeof(ObjectPacket);
                
				snapshot.entities.push_back(data);
                break;
            }

        }
    }

	snapshotBuffer.push_back(snapshot);
	if (snapshotBuffer.size() > 50) //keep last 50 snapshots only.
		snapshotBuffer.pop_front();
}

void Network::sendServerUpdate()
{
    packetTimer -= Game::deltaTime;
    if (packetTimer <= 0.0f)
    {
        packetTimer = 1.0f / 60.0f;

        for (GameObject* go : Game::gameObjects)
        {
			if (go->objectType == ObjectType::PROJECTILE && go->updated) //client only sends up its player and projectiles it created.
            {
                std::cout << "Sending Projectile from client" << std::endl;
                ObjectPacket dataToSend{ go->clientID, go->networkID, go->objectType, go->m_pos.x, go->m_pos.y, go->m_velocity.x, go->m_velocity.y, go->m_rotation, go->health };
				pb->write(dataToSend);
            }
            
            go->updated = false;
        }

        if (Game::player) 
        {
            ObjectPacket dataToSend{ Game::player->clientID, Game::player->networkID, Game::player->objectType, Game::player->m_pos.x, Game::player->m_pos.y, Game::player->m_velocity.x, Game::player->m_velocity.y, Game::player->m_rotation, Game::player->health };
            pb->write(dataToSend);
        }

        sendPacket(Network::peer, pb->build());
    }
}

void Network::handleServerUpdates()
{
    // Handle incoming network updates

    float renderTick = getCurrentTick();
    
    //std::cout << "Render Tick: " << renderTick << std::endl;

	Snapshot prev;
	Snapshot next;

    if (getSnapshotsForInterpolation(prev, next, renderTick))
    {
		float alpha = computeAlpha(renderTick, prev.tickID, next.tickID);
        
        //create new entities that dont exist yet.
        for (ObjectPacket entity : next.entities)
        {
            bool exists = false;
            for (GameObject* go : Game::gameObjects)
            {
                if (go->networkID == entity.objectID && go->clientID == entity.clientID) // && entity.objectType != ObjectType::PROJECTILE && go->objectType != ObjectType::PROJECTILE
                {
                    exists = true;
                    break;
				}
            }

            if (!exists)
            {
                switch (entity.objectType)
                {
                case ObjectType::ENEMY:
                {
                    Enemy* enemy = new Enemy(glm::vec2(entity.x, entity.y), glm::vec2(32.0f, 32.0f));
                    enemy->m_scale = 1.5f;
                    enemy->m_rotation = entity.rotation;
                    enemy->networkID = entity.objectID;
                    enemy->previousPos = glm::vec2(entity.x, entity.y);
                    enemy->targetPos = glm::vec2(entity.x, entity.y);
                    enemy->health = entity.health;
                    enemy->updated = false;
                    break;
                }
                case ObjectType::PLAYER:
                {
                    Player* player = new Player(glm::vec2(entity.x, entity.y), glm::vec2(32.0f, 32.0f));
                    player->m_scale = 1.5f;
                    player->m_rotation = entity.rotation;
                    player->networkID = entity.objectID;
                    player->clientID = entity.clientID;
                    player->updated = true;

                    if (Game::player == nullptr && entity.clientID == Network::clientID)
                        Game::player = player;

                    break;
                }
                case ObjectType::PROJECTILE:
                {
                    if (entity.clientID != Network::clientID)
                    {
                        Projectile* projectile = new Projectile(glm::vec2(entity.x, entity.y), glm::vec2(32.0f, 32.0f), glm::vec3(1.0f), entity.rotation, 1.5f, glm::vec2(cos(entity.rotation) * 250.0f, sin(entity.rotation) * 250.0f));
                        projectile->updated = false;
                        projectile->networkID = entity.objectID;
                        projectile->clientID = entity.clientID;
                        projectile->previousPos = glm::vec2(entity.x, entity.y);
                        projectile->targetPos = glm::vec2(entity.x, entity.y);
                        projectile->t = 0.0f;
                        projectile->health = entity.health;
                    }
                    break;

                }
                }
            }
        }

        //update prev and target positions.
		for (GameObject* go : Game::gameObjects)
		{
            bool exists = false;
            for (ObjectPacket entity : prev.entities)
            {
				if (go->networkID == entity.objectID && go->clientID == entity.clientID)
				{

                    if (go->objectType == ObjectType::PROJECTILE)
                    {
                        go->synced = true;
                    }

					glm::vec2 prevPos = glm::vec2(entity.x, entity.y);
					go->previousPos = prevPos;
                    go->t = alpha;
                    if(go->networkID != Game::player->networkID)
                        go->m_rotation = entity.rotation;
                    exists = true;
					break;
				}
            }

            for (ObjectPacket entity : next.entities)
            {
                if (go->networkID == entity.objectID && go->clientID == entity.clientID)
                {

                    if (go->objectType == ObjectType::PROJECTILE)
                    {
                        go->synced = true;
                    }

                    glm::vec2 targetPos = glm::vec2(entity.x, entity.y);
                    go->targetPos = targetPos;
                    go->t = alpha;
                    if (go->networkID != Game::player->networkID)
                        go->m_rotation = entity.rotation;
                    exists = true;
                    break;
                }
            }
		
			if (!exists && go->objectType == ObjectType::ENEMY) 
            {
                go->m_active = false; //object no longer exists on server so mark it inactive.
            }
            else if (!exists && go->objectType == ObjectType::PROJECTILE && go->synced)
            {
				go->m_active = false; //object no longer exists on server so mark it inactive.
            }
				

        }
    }
}

float Network::getCurrentTick()
{
	float rawTick = clientTime / tickInterval;
    return (latestServerTick - interpolationDelayInTicks) + (rawTick - std::floor(rawTick));
}

bool Network::getSnapshotsForInterpolation(Snapshot& outPrev, Snapshot& outNext, float renderTick)
{
    std::lock_guard<std::mutex> lock(Game::pendingMutex);
	if (snapshotBuffer.size() < 2)
		return false;

	for (size_t i = 0; i < snapshotBuffer.size(); ++i)
	{
		if (snapshotBuffer[i].tickID >= renderTick)
		{
            if (i == 0) return false;
		    outNext = snapshotBuffer[i];
			outPrev = snapshotBuffer[i - 1];
			
            return true;
		}
	}
    return false;

}

float Network::computeAlpha(float renderTick, uint32_t tickPrev, uint32_t tickNext) {
    if (tickPrev == tickNext) return 0.0f;
    return (renderTick - tickPrev) / float(tickNext - tickPrev);
}

void Network::networkClient()
{

    while (true)
    {

        while (enet_host_service(client, &event, 0) > 0)
        {

            switch (event.type)
            {
			case ENET_EVENT_TYPE_CONNECT:
				puts("Connection to server succeeded.");
                startGame = true;
			break;
            case ENET_EVENT_TYPE_RECEIVE:

                handlePacket(event.packet);

                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnection succeeded.");
                event.peer->data = NULL;
                break;
            }
        }

    }

    enet_peer_reset(peer);
    std::cout << "Connection to server failed." << std::endl;
    //return EXIT_FAILURE;
    return;
}

std::vector<wchar_t> convertToWide(const char* cstr) {
    if (cstr == nullptr) {
        return std::vector<wchar_t>();
    }

    size_t convertedChars = 0;
    // Determine the required buffer size.
    errno_t err = mbstowcs_s(&convertedChars, nullptr, 0, cstr, _TRUNCATE);
    if (err != 0 || convertedChars == 0) {
        return std::vector<wchar_t>();
    }

    // Create a vector to hold the wide string. This handles memory allocation and deallocation automatically.
    std::vector<wchar_t> wideStr(convertedChars);

    // Perform the conversion.
    err = mbstowcs_s(&convertedChars, wideStr.data(), wideStr.size(), cstr, _TRUNCATE);
    if (err != 0) {
        return std::vector<wchar_t>();
    }

    return wideStr;
}

//TODO this function will call the server exe and start it up.
void Network::networkHost() 
{
    STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);

    //build out wide char arguments
	//std::cout << getLocalIP() << std::endl;
    //ip = getLocalIP();
    ip = "10.0.0.178";
    std::vector<wchar_t> serverPath = convertToWide("C:\\Users\\samue\\Documents\\C++ Game Projects\\Zombies++\\x64\\Debug\\Server.exe");
	std::vector<wchar_t> args = convertToWide(ip.c_str());
    std::vector<wchar_t> commandLine = serverPath;

	commandLine.pop_back(); //remove null terminator
	commandLine.push_back(L' '); //add space between exe path and args)
    commandLine.insert(commandLine.end(), args.begin(), args.end());
    
    //create job to assign process to (allows the process to get killed when this program gets killed)
    HANDLE hJob = CreateJobObject(NULL, NULL);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = { 0 };
	jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jobInfo, sizeof(jobInfo));

    std::wcout << "Command Line : " << commandLine.data() << std::endl;

    if (CreateProcess(serverPath.data(), commandLine.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        std::cout << "Server launched successfully!" << std::endl;

		AssignProcessToJobObject(hJob, pi.hProcess);
		ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else
    {
        std::cout << "Failed to start server. Error: " << GetLastError() << std::endl;
    }
}