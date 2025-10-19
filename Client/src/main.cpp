#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <utility>   // std::pair
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "TileWorld.h"
#include "GameObject.h"
#include "Projectile.h"
#include "Player.h"
#include "Enemy.h"
#include "Network.h"
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <enet/enet.h>
#include <optional>
#include <mutex>
#include <queue>
#include "Game.h"
#include "ResourceManager.h"
#include "PacketBuilder.h"
#include <algorithm>
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD  // if you're using GLAD
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

static GLFWwindow* window;
static int windowWidth = 800;
static int windowHeight = 640;

Shader* shader;
Renderer* renderer;
TileWorld* tileWorld;

bool isHost = false;
bool startGame = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

// Generate a random edge position
std::pair<int, int> getRandomEdgePosition()
{
    int edge = rand() % 4; // 0=top, 1=bottom, 2=left, 3=right
    int x = 0, y = 0;

    switch (edge)
    {
    case 0: // Top
        x = rand() % windowWidth;
        y = 0;
        break;
    case 1: // Bottom
        x = rand() % windowWidth;
        y = windowHeight - 1;
        break;
    case 2: // Left
        x = 0;
        y = rand() % windowHeight;
        break;
    case 3: // Right
        x = windowWidth - 1;
        y = rand() % windowHeight;
        break;
    }

    return { x, y };
}


void initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "Pixel Simulator", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(0);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
}

void processInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if(Game::player != nullptr)
            Game::player->m_velocity.y = -125.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.y = 125.0f;
    }
    else 
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.y = 0.0f;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.x = -125.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (Game::player != nullptr)
            Game::player->m_velocity.x = 125.0f;
    }
    else
    {
        if (Game::player != nullptr)
    		Game::player->m_velocity.x = 0.0f;
    }
    
    if (Game::player != nullptr) 
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            Game::player->shoot();
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (Game::player != nullptr)
    {
        float dx = xpos - (Game::player->m_pos.x);
        float dy = ypos - (Game::player->m_pos.y);
        float angleRadians = atan2(dy, dx);
        Game::player->m_rotation = angleRadians;
    }
}

glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t)
{
    return a + (b - a) * t;
}

void ImGuiInit()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

int main(int argc, char* argv[])
{   
    if (argc == 2 && std::strcmp(argv[1], "host") == 0)
        isHost = true;

    initWindow();
    
    shader = new Shader("src\\vertexShader.txt","src\\fragmentShader.txt");
	renderer = new Renderer(shader, windowWidth, windowHeight);

    Game::init();
    ResourceManager::init();
    
	tileWorld = new TileWorld({ Texture2D("resources\\whiteTile.png"), Texture2D("resources\\grayTile.png") }, windowWidth, windowHeight);

    //Network::init(isHost);

    ImGuiInit();

    bool spawnEnemies = true;
    char ipAddress[64] = "";

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //width, height are the second and third values
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, -1.0f, 1.0f);
        shader->setMatrix4("projection", glm::value_ptr(projection)); //create method for this in the renderer probably.

        tileWorld->draw(*renderer);

		//Renderer::draw(ResourceManager::textures["background"], *Game::player);



        if (Network::startGame)
        {
            Game::doDeltaTime(Game::getTime());
            Network::clientTime += Game::deltaTime; //update clientTime for network interpolation stuffs.
            processInput(window);

            // Handle incoming network updates
            Network::handleServerUpdates();

            Game::updateGameObjects();
            renderer->drawFrame();

            Network::sendServerUpdate();
            Game::cleanupInactiveObjects();
        }


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow(); // Show demo window! :)

        ImGui::Begin("Menu");

        if (ImGui::Button("Host")) 
        {
            Network::init(true);
            //startGame = true;
        }

        if (ImGui::Button("Join"))
        {
            printf("IP Address: %s\n", ipAddress);
            Network::init(false);
            //startGame = true;
        }
        
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float labelWidth = ImGui::CalcTextSize("IP Address").x + ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::SetNextItemWidth(availableWidth - labelWidth);
        ImGui::InputText("IP Address", ipAddress, IM_ARRAYSIZE(ipAddress));

        if (ImGui::Checkbox("Spawn Enemies", &Game::spawnEnemies))
        {
            CommandPacket commandPkt = { Network::clientID, CommandType::SPAWNENEMY };
            Network::pb->write(commandPkt);
        }
        

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

//Messing around with limiting fps
/*float endFrameTime = glfwGetTime();
float frameDuration = endFrameTime - currentFrameTime;

if (frameDuration < (1.0f / 165.0f))
{
    glfwWaitEventsTimeout((1.0f / 165.0f) - frameDuration);
}*/