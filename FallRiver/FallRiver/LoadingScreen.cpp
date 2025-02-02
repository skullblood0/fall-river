#include "LoadingScreen.h"
#include "GamePlayState.h"
#include "CGame.h"
#include "MainMenuState.h"

LoadingScreen* LoadingScreen::GetInstance()
{
	static LoadingScreen s_Instance;

	return &s_Instance;
}

LoadingScreen::LoadingScreen(void)
{
	m_pVM = ViewManager::GetInstance();
	m_nBackLoad = ViewManager::GetInstance()->RegisterTexture("resource/graphics/Loading.png");
	m_nFontID	= m_pVM->RegisterFont("resource/graphics/FallRiver_0.png");
	Logo		= m_pVM->RegisterTexture("resource/graphics/logo_team_800.png");
	m_nProgress = 0;
	m_dwFlashlight = 0;
	first = true;
}

LoadingScreen::~LoadingScreen()
{
	m_pVM = nullptr;
}

void LoadingScreen::Reset()
{
	m_nProgress = 0;
	first = false;
}

void LoadingScreen::Update()
{
	m_nProgress++;
}

void LoadingScreen::Render()
{
	// Clear the background
	m_pVM->Clear(0, 0, 0);

	//// Start D3D rendering
	m_pVM->DeviceBegin();
	m_pVM->SpriteBegin();

	m_pVM->GetSprite()->Flush();
	if( CGame::GetInstance()->GetState() == MainMenuState::GetInstance() && first )
		m_pVM->DrawStaticTexture(Logo, 0, 0, 0.7f, 0.6f);
	else
		m_pVM->DrawStaticTexture(m_nBackLoad, 0, 0, 0.7f, 0.5f);
	/*m_pVM->DrawFont(m_nFontID, "Loading\nPlease Wait..", 10, 500);

	RECT progrect = {250, 525, progrect.left+(3*m_nProgress), 550};

	m_pVM->DrawRect(progrect, 255, 0, 0);*/

	m_pVM->SpriteEnd();
	m_pVM->DeviceEnd();

	// Render the buffer to the screen
	m_pVM->Present();
}
