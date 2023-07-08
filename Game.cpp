#include "Game.h"
#include "Graphics.h"
#include "Camera.h"

Game::Game() :
    m_fSeperation(SEPERATION_MULTIPLIER),
	m_fAlignment(ALIGNMENT_MULTPLIER),
	m_fCohesion(COHESION_MULTIPLIER),
	m_fVelocity(VELOCITY_MULTIPLIER)
{
	m_vecZoids.reserve(100);    

}

Game::~Game()
{
}

void Game::Initialise(Graphics& pGraphics)
{
    HRESULT hr = S_OK;

    m_pCamera = std::make_unique<Camera>(pGraphics.GetWidth(), pGraphics.GetHeight());

    for (std::shared_ptr<Zoid> pZoid : m_vecZoids)
    {
        hr = pZoid->initMesh(pGraphics.GetDevice(), pGraphics.GetContext());
        if (FAILED(hr))
			return;
    }
}

void Game::Update(float dt)
{
    for (std::shared_ptr<Zoid> pZoid : m_vecZoids)
    {
        pZoid->setSeperationMultiplier(m_fSeperation);
        pZoid->setAlignmentMultiplier(m_fAlignment);
        pZoid->setCohesionMultiplier(m_fCohesion);
        pZoid->setVelocityMultiplier(m_fVelocity);

        pZoid->update(dt, m_vecZoids);

        pZoid->checkIsOnScreenAndFix(
            XMMatrixTranspose(XMLoadFloat4x4(m_pCamera->GetView())),
            XMMatrixTranspose(XMLoadFloat4x4(m_pCamera->GetProjection())));

    }
}

void Game::Render(Graphics& pGraphics)
{
    for (std::shared_ptr<Zoid> pZoid : m_vecZoids)
    {
        // Set up the constant buffers
        pGraphics.SetupTransformConstantBuffer(
			*pZoid->getTransform(),
			*m_pCamera.get());
        pGraphics.SetupLightingConstantBuffer(*m_pCamera.get());
        pGraphics.SetupMaterialConstantBuffer(pZoid->getMaterial());

        // Render a Zoid
        pGraphics.SetupConstantBuffers(
            *pZoid->getTextureResourceView(),
            *pZoid->getTextureSamplerState());

        // draw Zoid
		pZoid->draw(pGraphics.GetContext().Get());
	}
}

std::shared_ptr<Zoid> Game::createFish(XMFLOAT3 position, bool bShark)
{
    //Zoid* fish = new Zoid(position);
    std::shared_ptr<Zoid> pFish = std::make_shared<Zoid>(position);

    pFish->setScale(1.0f);
    if (bShark)
    {
        pFish->setScale(3.0f);
        pFish->setAmbientColour(XMFLOAT4(255.0f, 0.0f, 0.0f, 1.0f));
    }

    return std::move(pFish);
}

void Game::squareFormation(int amount)
{
    // clear Zoid vector
    m_vecZoids.clear();

    XMFLOAT3 position;
    bool bIsShark = false;
    int iPerLine = round(sqrt(amount));

    for (int x = 0; x < iPerLine; x++)
    {
        for (int y = 0; y < iPerLine; y++)
        {
            position = XMFLOAT3(x * iPerLine, y * iPerLine, 0);
            bIsShark = (x == 0 && y == 0);
            
            m_vecZoids.push_back(createFish(position, bIsShark));
        }
    }
}

void Game::circleFormation(int iRadius, int iAmount)
{
    // clear Zoid vector
    m_vecZoids.clear();

    int tmpX, tmpY;
    float degrees = iAmount / (2 * XM_PI);

    for (int point = 0; point < iAmount; point++) {
        float angle = point * degrees;

        tmpX = iRadius * cos(angle * XM_PI);
        tmpY = iRadius * sin(angle * XM_PI);

        m_vecZoids.push_back(createFish(XMFLOAT3(tmpX, tmpY, 0), point == 0));
    }
}

void Game::spiralFormation(int iCoils, int iRadius, int iRotation)
{
    // clear Zoid vector
    m_vecZoids.clear();

    // value of theta corresponding to end of last coil
    double thetaMax = iCoils * 2 * XM_PI;

    // How far to step away from center for each side.
    double awayStep = iRadius / thetaMax;

    // distance between points to plot
    double chord = 10;

    int centerX, centerY;
    centerX = centerY = 0;

    //Zoid* midFish = createFish(XMFLOAT3(centerX, centerY, 0), true);

    // For every side, step around and away from center.
    // start at the angle corresponding to a distance of chord
    // away from centre.
    for (double theta = chord / awayStep; theta <= thetaMax; ) {
        // How far away from center
        double away = awayStep * theta;
        
        // How far around the center.
        double around = theta + iRotation;
        
        // Convert 'around' and 'away' to X and Y.
        double x = centerX + cos(around) * away;
        double y = centerY + sin(around) * away;
        
        // Should this fish be a shark?
        bool bIsShark = (centerX == x && centerY == y);
        
        // Now that you know it, do it.
        m_vecZoids.push_back(createFish(XMFLOAT3(x, y, 0), bIsShark));
        
        // to a first approximation, the points are on a circle
        // so the angle between them is chord/radius
        theta += chord / away;
    }
}

void Game::placeFish(Formation eFormation)
{
    int iAmount = 100;
    int iRadius = 100;

    switch (eFormation)
    {
    case Game::SQUARE:
        squareFormation(iAmount);
        break;
    case Game::CIRCLE:
        circleFormation(iRadius, iAmount);
        break;
    case Game::SPIRAL:
        int iCoils = 10;
        int iRotation = 10;
        spiralFormation(iCoils, iRadius, iRotation);
        break;
    }
}

void Game::DeleteZoids()
{
    m_vecZoids.clear();
}
