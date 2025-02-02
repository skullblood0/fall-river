#include <Windows.h>
#include <vector>
#include <assert.h>
#include <tchar.h>
using namespace std;

#include "ViewManager.h"
#include "Texture.h"
#include "Font.h"
#include "Animation.h"
#include "TutorialState.h"
#include "AnimInfo.h"
#include "GamePlayState.h"
#include "LoadingScreen.h"
#include "HighScoresMenuState.h"
#include "DirectInput.h"
#include "WinMenuState.h"
#include "CreditsMenuState.h"
#include "LoadMenuState.h"
#include "PauseMenuState.h"
#include "MainMenuState.h"
#include "HowToPlayMenuState.h"
#include "HUD.h"
#include "CutScene.h"
#include "XMLManager.h"
#include "Frame.h"
#include "CGame.h"
#include "Player.h"
#include "CompanionAI.h"
#include "NPC.h"
#include "Particle_Manager.h"
#include "Emitter.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			if (p) { p->Release(); p = NULL; }
#endif

ViewManager::ViewManager()
{
	m_lpDirect3DObject	= NULL;
	m_lpDirect3DDevice	= NULL;
	m_lpSprite			= NULL;
	m_lpFont			= NULL;
	m_lpLine			= NULL;
	memset(&m_PresentParams, 0, sizeof(D3DPRESENT_PARAMETERS));
}
ViewManager* ViewManager::GetInstance() 
{
	static ViewManager s_Instance;

	return &s_Instance;
}

int ViewManager::RegisterTexture(char* aFilePath)
{
	if (!aFilePath)	return -1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		if (strcmp(aFilePath, textures[i].filename) == 0)
		{
			textures[i].ref++;
			return i;
		}
	}

	int nID = -1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		if (textures[i].ref == 0)
		{
			nID = i;
			break;
		}
	}

	if (nID == -1)
	{
		Texture loaded;

		// Copy the filename of the loaded texture.
		strcpy_s(loaded.filename, _countof(loaded.filename), aFilePath);

		// Load the texture from the given file.
		HRESULT hr = 0;
		hr = D3DXCreateTextureFromFileA(m_lpDirect3DDevice, aFilePath, &loaded.texture);

		// AddRef.
		loaded.ref = 1;

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		loaded.texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		loaded.Width	= d3dSurfDesc.Width;
		loaded.Height	= d3dSurfDesc.Height;

		// Put the texture into the list.
		textures.push_back(loaded);

		// Return the nID of the texture.
		return (int)textures.size() - 1;
	}
	// we found an open spot
	else
	{
		// Make sure the texture has been released.
		SAFE_RELEASE(textures[nID].texture);

		// Copy the filename of the loaded texture.
		strcpy_s(textures[nID].filename, _countof(textures[nID].filename), aFilePath);

		// Load the texture from the given file.
		HRESULT hr = 0;
		hr = D3DXCreateTextureFromFileA(m_lpDirect3DDevice, aFilePath, &textures[nID].texture);

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		textures[nID].texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		textures[nID].Width	= d3dSurfDesc.Width;
		textures[nID].Height	= d3dSurfDesc.Height;

		// AddRef
		textures[nID].ref = 1;

		// Return the nID of the texture.
		return nID;
	}
}

int ViewManager::RegisterFont(char* filename )
{
	XMLManager::GetInstance()->LoadFont("resource/graphics/FallRiver.xml", fonts, kerns);
	return RegisterTexture(filename);
}

int ViewManager::RegisterAnimation(char* aFilePath) 
{
	if (!aFilePath)	return -1;

	Animation Animate;
	//Create the tinyXML document structure
	TiXmlDocument doc;

	//attempt to load the file
	if(doc.LoadFile(aFilePath) == false)
		return false;

	//Access the root element ("players_list")
	TiXmlElement* pRoot = doc.RootElement();

	if(pRoot == nullptr)
		return false;

	const char* pText = pRoot->GetText();
	if(pText)
		strcpy_s(Animate.filepath, 260, pText);

	TiXmlElement* pAnimation = pRoot->FirstChildElement("animation");
	int j = 0;
	while(pAnimation)
	{

		vector<Frame> TempAnim;

		int loop = 0;
		pAnimation->Attribute("looping", &loop);
		if(loop == 0)
			Animate.looping.push_back(false);
		else
			Animate.looping.push_back(true);

		//Iterate through the nodes to load player data
		TiXmlElement* pPlayer = pAnimation->FirstChildElement("frame_info");

		while(pPlayer)
		{
			Frame info = { };

			//Read Name
			pText = pPlayer->GetText();
			if(pText)
				strcpy_s(info.eventMsg, 100, pText);

			int x = 0;
			TiXmlElement* pSource = pPlayer->FirstChildElement("source_rect_info");
			pSource->Attribute("sourceRectLeft", &x);
			info.sourceRect.left = x;
			pSource->Attribute("sourceRectRight", &x);
			info.sourceRect.right = x;
			pSource->Attribute("sourceRectTop", &x);
			info.sourceRect.top = x;
			pSource->Attribute("sourceRectBottom", &x);
			info.sourceRect.bottom = x;

			TiXmlElement* pCol = pPlayer->FirstChildElement("col_rect_info");
			pCol->Attribute("colRectLeft", &x);
			info.colRect.left = x;
			pCol->Attribute("colRectRight", &x);
			info.colRect.right = x;
			pCol->Attribute("colRectTop", &x);
			info.colRect.top = x;
			pCol->Attribute("colRectBottom", &x);
			info.colRect.bottom = x;

			TiXmlElement* pAct = pPlayer->FirstChildElement("act_rect_info");
			pAct->Attribute("actRectLeft", &x);
			info.activeRect.left = x;
			pAct->Attribute("actRectRight", &x);
			info.activeRect.right = x;
			pAct->Attribute("actRectTop", &x);
			info.activeRect.top = x;
			pAct->Attribute("actRectBottom", &x);
			info.activeRect.bottom = x;

			double y = 0;
			TiXmlElement* pDur = pPlayer->FirstChildElement("duration_info");
			pDur->Attribute("duration", &y);
			info.duration = (float)y;

			TiXmlElement* pAnchor = pPlayer->FirstChildElement("anchor_info");
			pAnchor->Attribute("anchorPointX", &x);
			info.anchor.x = x;
			pAnchor->Attribute("anchorPointY", &x);
			info.anchor.y = x;

			//Save this info the the vector

			TempAnim.push_back(info);

			pPlayer = pPlayer->NextSiblingElement("frame_info");
		}

		Animate.frames.push_back(TempAnim);
		j++;
		pAnimation = pAnimation->NextSiblingElement("animation");
	}
	char temp[260] = "resource/graphics/";
	strcat_s(temp, 260, Animate.filepath);
	Animate.nTextureID = RegisterTexture(temp);

	animations.push_back(Animate);

	return (int)animations.size() - 1;
}


//For Drawing Animations
bool ViewManager::DrawAnimation(AnimInfo* aAnimInfo, float nPosX, float nPosY, float fScaleX, float fScaleY, float fRotCenterX, 
	float fRotCenterY, float fRotation, D3DCOLOR color)
{
	assert(aAnimInfo->curAnimID > -1 && aAnimInfo->curAnimID < (int)animations.size() && "Anim ID is out of range");
	assert(aAnimInfo->curAnimation < (int)animations[aAnimInfo->curAnimID].frames.size() && "Current Animation is out of range");
	assert(animations[aAnimInfo->curAnimID].nTextureID <  (int)textures.size() && "Texture ID is out of range");
	assert(aAnimInfo->curFrame < (int)animations[aAnimInfo->curAnimID].frames[aAnimInfo->curAnimation].size() && "Current frame is out of Range");

	Frame info = animations[aAnimInfo->curAnimID].frames[aAnimInfo->curAnimation][aAnimInfo->curFrame];

	DrawStaticTexture(animations[aAnimInfo->curAnimID].nTextureID, nPosX - int((info.anchor.x * fScaleX)), nPosY - int((info.anchor.y * fScaleY)), fScaleX, fScaleY, 
		&(animations[aAnimInfo->curAnimID].frames[aAnimInfo->curAnimation][aAnimInfo->curFrame].sourceRect), fRotCenterX, fRotCenterY, fRotation, color);
	return true;
}


//For Drawing Fonts
bool ViewManager::DrawFont(int nFontID, char* cString, float nPosX, float nPosY, float fScaleX, float fScaleY, float fRotCenterX, 
	float fRotCenterY, float fRotation, D3DCOLOR color)
{
	assert(nFontID > -1 && nFontID < (int)textures.size() && "Font ID is out of range");

	//assert(fonts[nFontID].nTextureID <  (int)textures.size() && "Texture ID is out of range");

	float nX = nPosX;
	float nColStart = nX;
	float nY = nPosY;
	char first = 0;
	char second = 0;
	//char newline;

	for(int i = 0; cString[i] != '\0'; i++)
	{
		char ch = cString[i];
		int id = ch - ' ';
		//int find = 0;

		if(i == 0)
			second = ch;
		else if(i == 1)
			first = ch;

		for(unsigned int j = 0; j < fonts.size(); j++)
		{
			if( ch == fonts[j].id )
			{
				id = j;
				break;
			}
		}

		if(ch == ' ')
		{
			nX += 7;
			continue;
		}
		else if(ch == '\n' )
		{
			// move the y down
			nY += 20 * fScaleY;
			nX = nColStart;
			continue;
		}
		else if(ch == '\t' )
		{
			nX += 50 * fScaleX;
			continue;
		}

		if(i == 1)
		{
			for(unsigned int k = 0; k <kerns.size(); k++)
			{
				if( first == kerns[k].firstID && second == kerns[k].secondID )
				{
					nX += kerns[k].amount;
					break;
				}
			}
		}

		//Draw!
		RECT srcRect = {fonts[id].x, fonts[id].y, fonts[id].x+fonts[id].width, fonts[id].y+fonts[id].height};
		DrawStaticTexture(nFontID, nX+fonts[id].xoffset, nY+fonts[id].yoffset, fScaleX, fScaleY, &srcRect, fRotCenterX, fRotCenterY, fRotation, color);

		// Move position to next char
		nX += int(fonts[id].width * fScaleX)+fonts[id].xoffset;
	}

	return true;
}


//For Anything Else
bool ViewManager::DrawStaticTexture(int nTextureID, float nPosX, float nPosY, float fScaleX, float fScaleY, RECT* sourceRect, float fRotCenterX, 
	float fRotCenterY, float fRotation, D3DCOLOR color)
{
	// Make sure the nID is in range.
	assert(nTextureID > -1 && nTextureID < (int)textures.size() && "TextureID is out of range");

	// Make sure that the texture is valid
	assert(textures[nTextureID].texture != NULL && "Attempting to draw released texture id");

	// Make sure the sprite was created and we have a valid texture.
	if (!m_lpSprite)
		return false;

	D3DXMATRIX scale;
	D3DXMATRIX rotation;
	D3DXMATRIX translate;
	D3DXMATRIX combined;

	// Initialize the Combined matrix.
	D3DXMatrixIdentity(&combined);

	// Scale the sprite.
	D3DXMatrixScaling(&scale, fScaleX, fScaleY, 1.0f);
	combined *= scale;

	// Rotate the sprite.
	D3DXMatrixTranslation(&translate, -fRotCenterX * fScaleX, -fRotCenterY * fScaleY, 0.0f);
	combined *= translate;
	D3DXMatrixRotationZ(&rotation, fRotation);
	combined *= rotation;
	D3DXMatrixTranslation(&translate, fRotCenterX * fScaleX, fRotCenterY * fScaleY, 0.0f);
	combined *= translate;

	// Translate the sprite
	D3DXMatrixTranslation(&translate, (float)nPosX, (float)nPosY, 0.0f);
	combined *= translate;

	// Apply the transform.
	m_lpSprite->SetTransform(&combined);

	// Draw the sprite.
	m_lpSprite->Draw(textures[nTextureID].texture, sourceRect, NULL, NULL, color);

	// Move the world back to identity.
	D3DXMatrixIdentity(&combined);
	m_lpSprite->SetTransform(&combined);

	// success.
	return true;
}



bool ViewManager::InitViewManager(HWND hWnd, int nScreenWidth, int nScreenHeight, bool bIsWindowed, bool bVsync)
{
	// Make sure the hWnd is valid.
	if (!hWnd) return false;

	// Set the handle to the window.
	m_hWnd = hWnd;

	// Create the Direct3D9 Object.
	m_lpDirect3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Setup the parameters for using Direct3D.
	m_PresentParams.BackBufferWidth				= nScreenWidth;
	m_PresentParams.BackBufferHeight			= nScreenHeight;
	m_PresentParams.BackBufferFormat			= (bIsWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	m_PresentParams.BackBufferCount				= 1;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.MultiSampleQuality			= 0;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_COPY;
	m_PresentParams.hDeviceWindow				= hWnd;
	m_PresentParams.Windowed					= bIsWindowed;
	m_PresentParams.EnableAutoDepthStencil		= false;
	m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_PresentParams.PresentationInterval		= (bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D Device.
	m_lpDirect3DObject->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_PresentParams, &m_lpDirect3DDevice);

	// Create Sprite Object.
	D3DXCreateSprite(m_lpDirect3DDevice, &m_lpSprite);

	// Create the Line Object.
	D3DXCreateLine(m_lpDirect3DDevice, &m_lpLine);

	// Setup Line parameters.
	m_lpLine->SetAntialias(TRUE);
	m_lpLine->SetWidth(3.0f);

	// grab the actual display width
	IDirect3DSurface9 *bBuffer = 0; 
	m_lpDirect3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&bBuffer);
	bBuffer->GetDesc(&backbuffer);
	bBuffer->Release(); // drop ref count

	// Load Shader
	//HRESULT hr = 
	D3DXCreateEffectFromFile(m_lpDirect3DDevice,L"resource/Shaders/Lights.fx",0,0,0,0,&postEffect,0);
	//hr;
	// Create Render Target
	D3DXCreateTexture(m_lpDirect3DDevice, backbuffer.Width, backbuffer.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &renderTarget); 

	// Wall wa are gonna watch
	VERTUV tmp[6];
	int WINDOW_WIDTH = CGame::GetInstance()->GetScreenWidth();
	int WINDOW_HEIGHT = CGame::GetInstance()->GetScreenHeight();
	tmp[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);		tmp[0].uv = D3DXVECTOR2((1/WINDOW_WIDTH) * .5f,(1/WINDOW_HEIGHT) * .5f);
	tmp[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);			tmp[1].uv = D3DXVECTOR2(1 + ((1/WINDOW_WIDTH) * .5f),(1/WINDOW_HEIGHT) * .5f);
	tmp[2].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		tmp[2].uv = D3DXVECTOR2(1 + ((1/WINDOW_WIDTH) * .5f),1 + ((1/WINDOW_HEIGHT) * .5f));
	tmp[3].pos = tmp[0].pos;							tmp[3].uv = tmp[0].uv;
	tmp[4].pos = tmp[2].pos;							tmp[4].uv = tmp[2].uv;
	tmp[5].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);		tmp[5].uv = D3DXVECTOR2((1/WINDOW_WIDTH) * .5f,1 + ((1/WINDOW_HEIGHT) * .5f));
	void* mem = 0;
	m_lpDirect3DDevice->CreateVertexBuffer( sizeof(VERTUV) * 6, 0, 0, D3DPOOL_MANAGED, &wallbuff, 0);
	wallbuff->Lock(0,0,&mem,0);
	memcpy(mem, tmp, 6 * sizeof(VERTUV)); 
	wallbuff->Unlock();

	D3DVERTEXELEMENT9 decl[] = 
	{ 
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	m_lpDirect3DDevice->CreateVertexDeclaration(decl, &cubedecl);

	D3DXVECTOR3 eye(0,2,-4), at(0,0,1), up(0,1,0);
	D3DXMatrixLookAtLH(&cam,&eye,&at,&up);
	float fov = (D3DXToRadian(75)), aspect = (WINDOW_WIDTH/(float)WINDOW_HEIGHT), znear = 0.01f, zfar = 100.0f; 
	D3DXMatrixPerspectiveFovLH(&proj,fov,aspect,znear,zfar);

	D3DXMatrixIdentity(&wall);
	D3DXMatrixTranslation(&wall, 0, 0, 0);

	ambientLight[0] = 1.0f;
	ambientLight[1] = 1.0f;
	ambientLight[2] = 1.0f;
	color[0] = .5f;
	color[1] = .5f;
	color[2] = .5f;
	lightDir[0] = 0.0f;
	lightDir[1] = 1.0f;
	lightDir[2] = 0.0f;
	lightPos[0] = 0.0f;
	lightPos[1] = 0.0f;
	lightPos[2] = 1.0f;
	spotLightPos[0] = 0.0f;
	spotLightPos[1] = 0.0f;
	spotLightPos[2] = -.7f;
	innerCone = .95f;
	outerCone = .9f;
	lightNum = 0;
	//	Return success.
	return true;
}

bool ViewManager::DeviceBegin(void)
{
	// store backbuffer
	current = 0;
	output = 0;
	//HRESULT hr = 
	m_lpDirect3DDevice->GetRenderTarget(0,&current);
	//hr;
	// get texture surface and set render target
	renderTarget->GetSurfaceLevel(0,&output);
	m_lpDirect3DDevice->SetRenderTarget(0,output);
	// clear rendertarget
	m_lpDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET , D3DCOLOR_XRGB(0,0,0), 1.0f, 0);	// modify for zbuffer
	m_lpDirect3DDevice->BeginScene();
	return true;
}

bool ViewManager::SpriteBegin(void)
{
	m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	return true;
}

bool ViewManager::LineBegin(void)
{
	m_lpLine->Begin();
	return true;
}

bool ViewManager::DeviceEnd(void)
{
	m_lpDirect3DDevice->EndScene();
	// restore backbuffer
	m_lpDirect3DDevice->SetRenderTarget(0,current);
	// drop ref counts
	if(current != nullptr)
		current->Release();
	if(output != nullptr)
		output->Release();

	QuestLog = RegisterTexture("resource/graphics/PaperThing.png");

	// Post Processing
	// clear backbuffer
	m_lpDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,255,0), 1.0f, 0);	// modify for zbuffer
	m_lpDirect3DDevice->BeginScene();

	// render quad with post shader
	unsigned passes(0);
	D3DXMATRIX tmp;
	D3DXMatrixInverse(&tmp, 0, &wall);
	postEffect->Begin(&passes, 0);


	D3DXVECTOR3 tmpDir[6];
	D3DXVECTOR3 tmpPos[6];
	D3DXVECTOR3 tmpColor[6];
	float tmpInnerCone[6];
	float tmpOuterCone[6];

	unsigned int j = 0;
	lightNum = 0;
	CreateOtherLights();
	for(; j < lightsToRender.size(); ++j)
	{
		tmpDir[j] = lightsToRender[j]->lightDir;
		tmpPos[j] = lightsToRender[j]->lightPos;
		tmpColor[j] = lightsToRender[j]->color;
		tmpInnerCone[j] = lightsToRender[j]->innerCone;
		tmpOuterCone[j] = lightsToRender[j]->outerCone;
		++lightNum;
	}

	for(; j < 6; ++j)
	{
		Light* light = new Light();
		tmpDir[j] = light->lightDir;
		tmpPos[j] = light->lightPos;
		tmpColor[j] = light->color;
		tmpInnerCone[j] = light->innerCone;
		tmpOuterCone[j] = light->outerCone;
		delete light;
	}

	for(unsigned i(0); i<passes; ++i)
	{
		postEffect->BeginPass(i);
		postEffect->SetTexture("gDiffuseTexture", renderTarget);
		postEffect->SetMatrix("gWorldInv", &tmp);
		postEffect->SetFloatArray("gLightDir2", lightDir,3);
		postEffect->SetFloatArray("ambientLight", ambientLight, 3);
		postEffect->SetFloatArray("gLightPos2", lightPos,3);
		postEffect->SetFloatArray("gLightPos", spotLightPos, 3);
		postEffect->SetFloat("gInnerCone2", innerCone);
		postEffect->SetFloat("gOuterCone2", outerCone);
		postEffect->SetFloatArray("lightColor", color, 3);
		postEffect->SetInt("gSetting", 0);

		postEffect->SetInt("gLightNum", lightNum);
		postEffect->SetValue("gLightPos3", tmpPos, sizeof(D3DXVECTOR3) * 6);
		postEffect->SetValue("gLightDir3", tmpDir, sizeof(D3DXVECTOR3) * 6);
		postEffect->SetValue("lightColor3", tmpColor, sizeof(D3DXVECTOR3) * 6);
		postEffect->SetValue("gInnerCone3", tmpInnerCone, sizeof(float) * 6);
		postEffect->SetValue("gOuterCone3", tmpOuterCone, sizeof(float) * 6);


		postEffect->CommitChanges();
		m_lpDirect3DDevice->SetVertexDeclaration(cubedecl);

		m_lpDirect3DDevice->SetStreamSource( 0, wallbuff, 0, sizeof(VERTUV));

		m_lpDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);

		postEffect->EndPass();
	}
	postEffect->End();

	m_lpDirect3DDevice->EndScene();

	if(ambientLight[0] == .1f && ambientLight[2] == .0f)
	{
		m_lpDirect3DDevice->BeginScene();
		m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

		DrawFont(LoadingScreen::GetInstance()->GetFontID(), "Loading\nPlease Wait..", 10, 500);

		RECT progrect = {250, 525, progrect.left+(3*LoadingScreen::GetInstance()->GetProgress()), 550};

		DrawRect(progrect, 255, 0, 0);

		m_lpSprite->End();
		m_lpDirect3DDevice->EndScene();
	}
	else if(ambientLight[0] == .0f)
	{
		//if(CGame::GetInstance()->GetState() == GamePlayState::GetInstance() )
		{
			m_lpDirect3DDevice->BeginScene();
			m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

			GamePlayState::GetInstance()->m_pHUD->Render();
			Player* tmp = GamePlayState::GetInstance()->GetPlayer();
			//Quest Log Rect
			RECT src_Rect = {0,0,200,200};

			if(tmp->questLogToggle)
			{
				// Quest Log Box
				//RECT logRect = { 600, 0, 800, 200};
				//DrawRect(logRect, 50, 50, 50);
				this->DrawStaticTexture(QuestLog,600-(float(CGame::GetInstance()->GetScreenWidth())*0.05f),(float(CGame::GetInstance()->GetScreenHeight())*0.05f)+0,1.0f,1.0f,&src_Rect);
				DrawFont(tmp->m_nFontID,"Active Quests",640.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f),(float(CGame::GetInstance()->GetScreenHeight())*0.05f)+10.0f,0.5f,0.5f);
				for(unsigned int i = 0; i < tmp->m_vpActiveQuests.size(); i++)
				{
					if(tmp->m_vpActiveQuests[i]->QuestID % 2 == 0)
						DrawFont(tmp->m_nFontID, (char*)tmp->m_vpActiveQuests[i]->QuestTitle.c_str(), 610.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f), (float(CGame::GetInstance()->GetScreenHeight())*0.05f)+float(i * 10 + 20), 0.5f, 0.5f);
					if(tmp->m_vpActiveQuests[i]->QuestID == 2)
					{
						if(tmp->questCounter >= 10)
							tmp->questCounter = 10;
						char buffer[100];
						_itoa_s(tmp->questCounter,buffer,10);
						DrawFont(tmp->m_nFontID, buffer, 610.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f), (float(CGame::GetInstance()->GetScreenHeight())*0.05f)+float(i * 10 + 30), 0.5f, 0.5f);
						DrawFont(tmp->m_nFontID, "/10 killed", 620.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f), (float(CGame::GetInstance()->GetScreenHeight())*0.05f)+float(i * 10 + 30), 0.5f, 0.5f);
					}

				}



				// Quest Finished Box
				//RECT finishedLogRect = { 600, 200, 800, 400};
				//DrawRect(finishedLogRect,50,50,50);
				this->DrawStaticTexture(QuestLog,600-(float(CGame::GetInstance()->GetScreenWidth())*0.05f),(float(CGame::GetInstance()->GetScreenHeight())*0.05f)+200,1.0f,1.0f,&src_Rect);
				DrawFont(tmp->m_nFontID,"Finished Quests",640.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f),(float(CGame::GetInstance()->GetScreenHeight())*0.05f)+210.0f,0.5f,0.5f);
				for(unsigned int i = 0; i < tmp->m_vpFinishedQuests.size(); i++)
				{
					if(tmp->m_vpFinishedQuests[i]->QuestID % 2 == 0)
						DrawFont(tmp->m_nFontID, (char*)tmp->m_vpFinishedQuests[i]->QuestTitle.c_str(), 610.0f-(float(CGame::GetInstance()->GetScreenWidth())*0.05f), (float(CGame::GetInstance()->GetScreenHeight())*0.05f)+float(i*10+220), 0.5f, 0.5f);
				}
			}

			CompanionAI* pBud = GamePlayState::GetInstance()->GetCompanion();
			if( pBud && pBud->IsTeaching() )
			{
				pBud->SaySomething();
				if( !DirectInput::GetInstance()->JoystickIsUnplugged(0) )
					DrawFont(tmp->m_nFontID, "Press \"Back\" to Skip Tutorial", 500, 50, 0.6f, 0.6f);
				else
					DrawFont(tmp->m_nFontID, "Press \"T\" to Skip Tutorial", 500, 50, 0.6f, 0.6f);
			}

			vector<NPC*> tmpNPCs = *GamePlayState::GetInstance()->GetNPCs();
			for(unsigned int i = 0; i < tmpNPCs.size(); ++i)
			{
				tmpNPCs[i]->RenderQuests();
			}
			RECT questBox;
			questBox.left = 0;
			questBox.top = CGame::GetInstance()->GetScreenHeight() - 100;
			questBox.right = CGame::GetInstance()->GetScreenWidth();
			questBox.bottom = CGame::GetInstance()->GetScreenHeight();

			/*if(GamePlayState::GetInstance()->questFlag)
			{
			DrawRect(questBox,255,255,255);
			DrawFont(tmp->m_nFontID,"You killed enough zombies...for now \n Go turn this in now.",0,500,0.8f,0.8f,0,0,0,D3DCOLOR_XRGB(0,0,0));
			}*/

			if(ambientLight[2] == .1f)
			{
				PauseMenuState::GetInstance()->Render();
			}
			m_lpSprite->End();
			m_lpDirect3DDevice->EndScene();
		}
		/*else if( CGame::GetInstance()->GetState() == TutorialState::GetInstance() )
		{
			m_lpDirect3DDevice->BeginScene();
			m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
			CompanionAI* pBud = TutorialState::GetInstance()->GetCompanion();
			if( pBud && pBud->IsTeaching() )
			{
				pBud->SaySomething();
			}
			m_lpSprite->End();
			m_lpDirect3DDevice->EndScene();
			Present();
		}*/
	}

	m_lpDirect3DDevice->Present(0,0,0,0);
	return true;
}

bool ViewManager::SpriteEnd(void)
{
	m_lpSprite->End();
	return true;
}

bool ViewManager::LineEnd(void)
{
	m_lpLine->End();
	return true;
}

void ViewManager::Present(void)
{
	m_lpDirect3DDevice->Present(0,0,0,0);
}

void ViewManager::DrawLine(int nX1, int nY1, int nX2, int nY2, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	D3DXVECTOR2 verts[2];

	verts[0].x = (float)nX1;
	verts[0].y = (float)nY1;
	verts[1].x = (float)nX2;
	verts[1].y = (float)nY2;

	m_lpLine->Draw(verts, 2, D3DCOLOR_XRGB(ucRed, ucGreen, ucBlue));
}


void ViewManager::DrawUnfilledRect(RECT test, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	D3DXVECTOR2 verts[5];

	verts[0].x = (float)test.left;

	verts[0].y = (float)test.top;

	verts[1].x = (float)test.right;

	verts[1].y = (float)test.top;

	verts[2].x = (float)test.right;

	verts[2].y = (float)test.bottom;

	verts[3].x = (float)test.left;

	verts[3].y = (float)test.bottom;

	verts[4].x = (float)test.left;

	verts[4].y = (float)test.top;


	m_lpLine->Draw(verts, 5, D3DCOLOR_XRGB(ucRed, ucGreen, ucBlue));
}


void ViewManager::DrawText(char *lpzText, int nX, int nY, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	// Pointer to the Back Buffer.
	LPDIRECT3DSURFACE9 d3dBackBuffer = 0;

	// Get the Back Buffer from the Device.
	m_lpDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3dBackBuffer);

	// Get a Device Context.
	HDC hdc = 0;
	d3dBackBuffer->GetDC(&hdc);

	// Print the string out to the screen.
	SetTextColor(hdc, RGB(ucRed, ucGreen, ucBlue));
	SetBkMode(hdc, TRANSPARENT);
	TextOutA(hdc, nX, nY, lpzText, (int)strlen(lpzText));

	// Release the Device Context.
	d3dBackBuffer->ReleaseDC(hdc);
	SAFE_RELEASE(d3dBackBuffer);
}

void ViewManager::Clear(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	m_lpDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(ucRed, ucGreen, ucBlue), 1.0f, 0);
}

void ViewManager::DrawRect(RECT rRt, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char uAlpha)
{
	D3DRECT d3dRect;
	d3dRect.x1 = rRt.left;
	d3dRect.y1 = rRt.top;
	d3dRect.x2 = rRt.right;
	d3dRect.y2 = rRt.bottom;

	m_lpDirect3DDevice->Clear(1, &d3dRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(uAlpha, ucRed, ucGreen, ucBlue), 1.0f, 0);
}

void ViewManager::ShutdownDirect3D(void)
{
	SAFE_RELEASE(m_lpFont);
	SAFE_RELEASE(m_lpLine);
	SAFE_RELEASE(m_lpSprite);
	SAFE_RELEASE(m_lpDirect3DDevice);
	SAFE_RELEASE(cubedecl); 
	SAFE_RELEASE(wallbuff);
	SAFE_RELEASE(renderTarget);
	SAFE_RELEASE(postEffect);

	
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		textures[i].ref = 0;

		SAFE_RELEASE(textures[i].texture);
		textures[i].filename[0] = '\0';
	}

	textures.clear();
}

void ViewManager::ChangeDisplayParam(int nWidth, int nHeight, bool bWindowed)
{
	bWindowed;
	nHeight;
	nWidth;
	//// Set the new Presentation Parameters.
	////m_PresentParams.BackBufferWidth				= nWidth;
	////m_PresentParams.BackBufferHeight			= nHeight;
	////m_PresentParams.BackBufferFormat			= D3DFMT_A1R5G5B5;
	////m_PresentParams.BackBufferCount				= 1;
	////m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	////m_PresentParams.MultiSampleQuality			= 0;
	////m_PresentParams.SwapEffect					= D3DSWAPEFFECT_COPY;
	////m_PresentParams.hDeviceWindow				= m_hWnd;
	////m_PresentParams.Windowed					= bWindowed;
	////m_PresentParams.EnableAutoDepthStencil		= false;
	////m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	////m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	////m_PresentParams.PresentationInterval		= (false) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	//
	//m_PresentParams.BackBufferFormat	= (bWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	//m_PresentParams.Windowed			= bWindowed;
	//m_PresentParams.BackBufferWidth		= nWidth;
	//m_PresentParams.BackBufferHeight	= nHeight;
	//
	//// Reset the device.
	//HRESULT hr;
	//// TODO: release the shader, rendertargets, and textures
	//hr = m_lpLine->OnLostDevice();
	//hr = m_lpSprite->OnLostDevice();
	//hr = m_lpDirect3DDevice->Reset(&m_PresentParams);
	//hr = m_lpSprite->OnResetDevice();
	//hr = m_lpLine->OnResetDevice();
	//
	//// Setup window style flags
	//DWORD dwWindowStyleFlags = WS_VISIBLE;
	//
	//
	//
	//HWND top;
	//if (bWindowed)
	//{
	//	dwWindowStyleFlags |= WS_OVERLAPPEDWINDOW;
	//	ShowCursor(TRUE); // show the mouse cursor
	//	top = HWND_NOTOPMOST;
	//}
	//else
	//{
	//	dwWindowStyleFlags |= WS_POPUP;
	//	ShowCursor(FALSE); // hide the mouse cursor
	//	top = HWND_TOP;
	//}
	//
	//SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyleFlags);
	//
	////	Set the window to the middle of the screen.
	//if (bWindowed)
	//{
	//	// Setup the desired client area size
	//	RECT rWindow;
	//	rWindow.left	= 0;
	//	rWindow.top		= 0;
	//	rWindow.right	= nWidth;
	//	rWindow.bottom	= nHeight;
	//
	//	// Get the dimensions of a window that will have a client rect that
	//	// will really be the resolution we're looking for.
	//	AdjustWindowRectEx(&rWindow, 
	//		dwWindowStyleFlags,
	//		FALSE, 
	//		WS_EX_APPWINDOW);
	//
	//	// Calculate the width/height of that window's dimensions
	//	int windowWidth		= rWindow.right - rWindow.left;
	//	int windowHeight	= rWindow.bottom - rWindow.top;
	//
	//	SetWindowPos(m_hWnd, top,	(GetSystemMetrics(SM_CXSCREEN)>>1) - (windowWidth>>1),
	//		(GetSystemMetrics(SM_CYSCREEN)>>1) - (windowHeight>>1),
	//		windowWidth, windowHeight, SWP_SHOWWINDOW);
	//}
	//else
	//{
	//	// Let windows know the window has changed.
	//	SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	//}
	//// grab the actual display width
	////IDirect3DSurface9 *bBuffer = 0; 
	////m_lpDirect3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&bBuffer);
	////bBuffer->GetDesc(&backbuffer);
	////bBuffer->Release(); // drop ref count
	//// Load Shader
	//hr = D3DXCreateEffectFromFile(m_lpDirect3DDevice,L"resource/Shaders/Lights.fx",0,0,0,0,&postEffect,0);
	//hr = D3DXCreateTexture(m_lpDirect3DDevice, backbuffer.Width, backbuffer.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A1R5G5B5 , D3DPOOL_DEFAULT, &renderTarget); 
	//int i = 0;

	PostMessage( m_hWnd, WM_USER, NULL, NULL);

}

void ViewManager::CreateOtherLights(void)
{
	for(unsigned int i = 0; i < lightsToRender.size(); i++ )
	{
		delete lightsToRender[i];
		lightsToRender[i] = nullptr;
	}
	lightsToRender.clear();

	RECT cRect;
	RECT camRect = { (long)GamePlayState::GetInstance()->GetCamera().x - CGame::GetInstance()->GetScreenWidth(), (long)GamePlayState::GetInstance()->GetCamera().y - CGame::GetInstance()->GetScreenHeight(), long(GamePlayState::GetInstance()->GetCamera().x + CGame::GetInstance()->GetScreenWidth() * 2), long(GamePlayState::GetInstance()->GetCamera().y + CGame::GetInstance()->GetScreenHeight() * 2)};
	vector<int> fireEffects = GamePlayState::GetInstance()->GetFireA();
	for( unsigned int i = 0; lightsToRender.size() < 6 && i < fireEffects.size(); i += 3)
	{
		RECT fire = Particle_Manager::GetInstance()->GetActiveEmitter(fireEffects[i])->GetRect();
		if(
			IntersectRect( &cRect, &camRect, &fire ) == TRUE && CGame::GetInstance()->GetState() == GamePlayState::GetInstance()
			//float cY 
			)
	
		{
			Light* tmp = new Light();
			tmp->innerCone = (.75f);
			tmp->outerCone = (.7f);
	
			if(!GamePlayState::GetInstance()->GetPlayer())
				return;
	
	
			tmp->lightPos[0] = (((((fire.left + fire.right) * .5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosX() )/ (CGame::GetInstance()->GetScreenWidth())) * 2) + .01f;
			tmp->lightPos[1] = (((((fire.bottom + fire.top) *.5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosY() ) / (CGame::GetInstance()->GetScreenHeight())) * -2) - .01f;
			tmp->lightPos[2] = -.75;
			tmp->lightDir[0] = 0;
			tmp->lightDir[1] = 0;
			tmp->lightDir[2] = 1;
			if(rand() % 9 == 0)
			{
				tmp->color[0] = 1;
				tmp->color[1] = 0;
				tmp->color[2] = 0;
			}
			else
			{
				tmp->color[0] = 1;
				tmp->color[1] = .6f;
				tmp->color[2] = 0;
			}
			lightsToRender.push_back(tmp);
			//delete tmp;
		}
	}
	vector<RECT> streetLights = GamePlayState::GetInstance()->GetStreelights();
	for( unsigned int i = 0; lightsToRender.size() < 6 && i < streetLights.size(); ++i)
	{
		if(
			IntersectRect( &cRect, &camRect, &streetLights[i] ) == TRUE && CGame::GetInstance()->GetState() == GamePlayState::GetInstance()
			//float cY 
			)
		{

			Light* tmp = new Light();
			tmp->innerCone = (.75f);
			tmp->outerCone = (.7f);
	
			if(!GamePlayState::GetInstance()->GetPlayer())
				return;
	
	
			tmp->lightPos[0] = (((((streetLights[i].left + streetLights[i].right) * .5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosX() )/ (CGame::GetInstance()->GetScreenWidth())) * 2) + .01f;
			tmp->lightPos[1] = (((((streetLights[i].bottom + streetLights[i].top) *.5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosY() ) / (CGame::GetInstance()->GetScreenHeight())) * -2) - .01f;
			tmp->lightPos[2] = -.75;
			tmp->lightDir[0] = 0;
			tmp->lightDir[1] = 0;
			tmp->lightDir[2] = 1;
			if(rand() % 90 > 7)
			{
				tmp->color[0] = .5;
				tmp->color[1] = .5;
				tmp->color[2] = 0;
			}
			else
			{
				tmp->color[0] = 0;
				tmp->color[1] = 0;
				tmp->color[2] = 0;
			}
			lightsToRender.push_back(tmp);
		}
	}
}
