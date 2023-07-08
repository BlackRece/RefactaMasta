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

    m_pGame = std::make_unique<Game>();
    m_pTimer = std::make_unique<FrameTimer>(DEFAULT_FPS);
}

AppEngine::~AppEngine()
{
}

void AppEngine::Start()
{
	m_bRunning = true;

    m_pGame->placeFish(Game::SQUARE);
    m_bRunning = m_pGame->Initialise(m_pAppWindow->GetHandle(), *m_pGraphics);
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

    m_pGraphics->EndDraw();
}
