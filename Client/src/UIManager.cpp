#include "UIManager.h"
#include "GLFW/glfw3.h"

void UIManager::init(GLFWwindow* window)
{
    UIManager::window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

	mainMenuData.logo = Texture2D("resources\\logoCrop.png");
};

void UIManager::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (mainMenuData.isVisible)
        renderMainMenu();

    if (settingsMenuData.isVisible)
        renderSettingsMenu();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
};

void UIManager::renderMainMenu()
{
    ImVec2 windowSize(300, 100);
    ImVec2 center = getCenterPosition(windowSize);

	ImGui::SetNextWindowPos(center, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGui::Begin("Menu");

    if (ImGui::Button("Host"))
    {
        if (!Network::isInitialized())
        {
            Network::init(true);
            mainMenuData.isVisible = false;
        }
    }

	ImGui::SameLine();

    if (ImGui::Button("Join"))
    {
        if (!Network::isInitialized())
        {
            printf("IP Address: %s\n", mainMenuData.ipAddress);
            Network::ip = mainMenuData.ipAddress;
            Network::init(false);
            mainMenuData.isVisible = false;
        }

    }

    float availableWidth = ImGui::GetContentRegionAvail().x;
    float labelWidth = ImGui::CalcTextSize("IP Address").x + ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::SetNextItemWidth(availableWidth - labelWidth);
    ImGui::InputText("IP Address", mainMenuData.ipAddress, IM_ARRAYSIZE(mainMenuData.ipAddress));

    ImGui::End();

	// Render logo
	Renderer::DrawSprite(mainMenuData.logo, glm::vec3(center.x + (windowSize.x / 2) - (300.0f / 2), center.y - 179.0f - 10.0f, 0.0f), glm::vec2(300.0f, 179.0f));
};

void UIManager::toggleMainMenu()
{
    mainMenuData.isVisible = !mainMenuData.isVisible;
};

void UIManager::renderSettingsMenu()
{
    ImGui::Begin("Settings");

    if (ImGui::Checkbox("Spawn Enemies", &settingsMenuData.spawnEnemies))
    {
        Game::spawnEnemies = settingsMenuData.spawnEnemies;
        CommandPacket commandPkt = { Network::clientID, CommandType::SPAWNENEMY };
        Network::pb->write(commandPkt);
    }

    if (ImGui::Button("Quit"))
    {
		glfwSetWindowShouldClose(window, true); //TODO : Graceful shutdown
    }

    ImGui::End();
};

void UIManager::toggleSettingsMenu()
{
    settingsMenuData.isVisible = !settingsMenuData.isVisible;
};

ImVec2 UIManager::getCenterPosition(ImVec2 windowSize)
{
    ImGuiIO& io = ImGui::GetIO();
    
    ImVec2 center = ImVec2(
        (io.DisplaySize.x - windowSize.x) * 0.5f,
        (io.DisplaySize.y - windowSize.y) * 0.5f
    );

    return center;
};