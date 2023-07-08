#include "ImGuiWrapper.h"

#include "ImGui.h"
#include "Graphics.h"

ImGuiWrapper::ImGuiWrapper()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
}

ImGuiWrapper::~ImGuiWrapper()
{
}

void ImGuiWrapper::Initialise(HWND& hWnd, Graphics& pGraphics)
{
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(
        pGraphics.GetDevice().Get(),
        pGraphics.GetContext().Get());
}

void ImGuiWrapper::Update(StatsParams statsParams)
{
    m_iZoidAmount = statsParams.iZoidAmount;

    m_fSeperation = statsParams.fSeperation;
    m_fAlignment = statsParams.fAlignment;
    m_fCohesion = statsParams.fCohesion;
    m_fVelocity = statsParams.fVelocity;

}

void ImGuiWrapper::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("_.- Boid Options -._");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Boids: %d", m_iZoidAmount);

    ImGui::NewLine();
    ImGui::InputInt("Boid Amount", &m_iZoidAmount);
    ImGui::InputInt("Shape Radius", &m_iRadius);
    ImGui::InputInt("Rotation", &m_iRotation);
    ImGui::InputInt("Coils", &m_iCoils);
    
    /*
    if (ImGui::Button("Square"))
    {
        g_Boids.clear();
        squareFormation(i_boidAmount);
    }
    if (ImGui::Button("Circle"))
    {
        g_Boids.clear();
        circleFormation(i_radius, i_boidAmount);
    }
    if (ImGui::Button("Spiral"))
    {
        g_Boids.clear();
        spiralFormation(i_coils, i_radius, i_rotation);
    }
    */

    ImGui::NewLine();
    ImGui::SliderFloat("Seperation Multiplier", &m_fSeperation, vMin, vMax);
    ImGui::InputFloat("Seperation:", &m_fSeperation, vMin, vMax, "%.4f");
    ImGui::SliderFloat("Alignment Multiplier", &m_fAlignment, vMin, vMax);
    ImGui::InputFloat("Alignment :", &m_fAlignment, vMin, vMax, "%.4f");
    ImGui::SliderFloat("Cohesion Multiplier", &m_fCohesion, vMin, vMax);
    ImGui::InputFloat("Cohesion:", &m_fCohesion, vMin, vMax, "%.4f");

    ImGui::NewLine();
    ImGui::SliderFloat("Velocity Multiplier", &m_fVelocity, vMin, vMax);
    ImGui::InputFloat("Velocity:", &m_fVelocity, vMin, vMax, "%.4f");

    ImGui::NewLine();
    // TODO: use collision class to get the stats of Zoids to display
    /*
    BoidData stats = Boid::getStats();
    std::string msg =
        "Time Elapsed: " +
        std::to_string(stats.fTotalTime);
    ImGui::Text(msg.c_str());

    msg =
        "Captured: " +
        std::to_string(stats.stats.size());
    ImGui::Text(msg.c_str());

    msg =
        "Average Bite Time: " +
        std::to_string(stats.GetAverage());
    ImGui::Text(msg.c_str());
    */

    /*for(int i = 0; i < stats.stats.size(); i++)
    {
        msg =
            std::to_string(i) +
            ": " +
            std::to_string(stats.stats[i]);
        ImGui::Text(msg.c_str());
    }*/

    ImGui::NewLine();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

ImGuiWrapper::ImGuiWrapper(const ImGuiWrapper&)
{
}

void ImGuiWrapper::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
