#include "AppEngine.h"
#include "Game.h"

AppEngine::AppEngine(AppEngineParams params)
{
    AppWindow::AppWindowParams winParams;
    winParams.pTitle = params.pTitle;
    winParams.nWidth = params.nWidth;
    winParams.nHeight = params.nHeight;
    winParams.nCmdShow = params.nCmdShow;

    m_pAppWindow = std::make_unique<AppWindow>(params.hInstance, winParams);

    m_pGraphics = std::make_unique<Graphics>(m_pAppWindow->GetHandle());
    m_pGraphics->Initialise(params.nWidth, params.nHeight);

    m_pGame = std::make_unique<Game>(m_pGraphics.get());
    m_pTimer = std::make_unique<FrameTimer>(DEFAULT_FPS);
}

AppEngine::~AppEngine()
{
}

void AppEngine::Start()
{
}

void AppEngine::Run()
{
    m_pTimer->Start();

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
    m_pTimer->Stop();
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
            PostQuitMessage(0);
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
    if(m_pTimer->Tick())
        return;

    m_pGame->Update(m_pTimer->GetDelta());
}

void AppEngine::Render()
{
    m_pGraphics->BeginDraw();
    m_pGame->Render(*m_pGraphics);

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
