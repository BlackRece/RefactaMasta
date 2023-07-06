#include "AppEngine.h"

AppEngine::AppEngine(AppEngineParams params)
{
    AppWindow::AppWindowParams winParams;
    winParams.pTitle = params.pTitle;
    winParams.nWidth = params.nWidth;
    winParams.nHeight = params.nHeight;
    winParams.nCmdShow = params.nCmdShow;

    m_pAppWindow = std::make_unique<AppWindow>(hInstance, winParams);

    m_pGraphics = std::make_unique<Graphics>(g_pAppWindow->GetHandle());
    m_pGraphics->Initialise(params.nWidth, params.nHeight);

    m_pCamera = std::make_unique<Camera>(params.nWidth, params.nHeight);
}

AppEngine::~AppEngine()
{
}

void AppEngine::Start()
{
}

void AppEngine::Run()
{
    while (m_bRunning)
    {
        ProcessMessages();
        // TODO: add call to change state
        Update();
        Render();
    }

    // TODO: deactivate current state
    //if (currentState != 0)
    //    currentState->OnDeactivate();
}

void AppEngine::Stop()
{
}

void AppEngine::ProcessMessages()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        switch (msg.message)
        {
        case WM_QUIT:
			m_bRunning = false;
			break;
        case WM_DESTROY:
                PostQuitMessage();
                break;
        default:
            /* do nothing */
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void AppEngine::Update()
{
    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    t = (timeCur - timeStart) / 1000.0f;
    timeStart = timeCur;

    float FPS60 = 1.0f / 60.0f;
    static float cummulativeTime = 0;

    // cap the framerate at 60 fps 
    cummulativeTime += t;
    if (cummulativeTime >= FPS60) {
        cummulativeTime = cummulativeTime - FPS60;
    }
    else {
        return;
    }
}

void AppEngine::Render()
{
    m_pGraphics->BeginDraw();

    // TODO: boid stuff should be handled by a gamestate class
    //Boid::updateStats(t);

    //for (unsigned int i = 0; i < g_Boids.size(); i++)
    //{
    //    g_Boids[i]->setSeperationMultiplier(g_seperation);
    //    g_Boids[i]->setAlignmentMultiplier(g_alignment);
    //    g_Boids[i]->setCohesionMultiplier(g_cohesion);
    //    g_Boids[i]->setVelocityMultiplier(g_velocity);

    //    g_Boids[i]->update(t, &g_Boids);

    //    //      g_View = g_pGraphics->GetViewMatrix();
    //    //      g_Projection = g_pGraphics->GetProjectionMatrix();
    //          //XMMATRIX vp = g_View * g_Projection;
    //          //Boid* dob = (Boid*)g_Boids[i];

    //          //g_Boids[i]->checkIsOnScreenAndFix(
    //          //    g_pGraphics->GetViewMatrix(),
    //          //    g_pGraphics->GetProjectionMatrix());

    //    g_Boids[i]->checkIsOnScreenAndFix(
    //        XMMatrixTranspose(XMLoadFloat4x4(g_pCamera->GetView())),
    //        XMMatrixTranspose(XMLoadFloat4x4(g_pCamera->GetProjection())));

    //    m_pGraphics->SetupTransformConstantBuffer(
    //        *g_Boids[i]->getTransform(),
    //        *g_pCamera.get());
    //    m_pGraphics->SetupLightingConstantBuffer(*g_pCamera.get());
    //    m_pGraphics->SetupMaterialConstantBuffer(g_Boids[i]->getMaterial());

    //    // Render a cube
    //    m_pGraphics->SetupConstantBuffers(
    //        *g_Boids[i]->getTextureResourceView(),
    //        *g_Boids[i]->getTextureSamplerState());

    //    // draw 
    //    g_Boids[i]->draw(g_pGraphics->GetContext().Get());
    //}

    //RenderImGui();

    // Present our back buffer to our front buffer
    m_pGraphics->EndDraw();
}
