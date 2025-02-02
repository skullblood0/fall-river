#include "Level.h"
#include "DirectInput.h"
#include "tinyxml.h"
#include "ViewManager.h"
#include "GamePlayState.h"
#include "TutorialState.h"
#include <string>
#include "CGame.h"
#include "BaseCharacter.h"
#include "Enemy.h"
#include "EventSystem.h"
#include "DestroyBullet.h"
#include "Bullet.h"
#include "Player.h"
#include "Particle_Manager.h"
#include "Emitter.h"

Level::Level() 
{
	inside = false;
	m_nObjectType = OBJ_LEVEL;
	for(int i =0; i < 4; ++i)
	{
		fogID[i] = ViewManager::GetInstance()->RegisterTexture("resource/graphics/fog.png");
		fog[i].x = 0;
		fog[i].y = 0;
		if(i == 2 )
			fog[i].x = 1024;

		if(i == 0)
			fog[i].y = 1024;
	}
	m_bNoClip = false;
	whichlevel = -1;
}

Level* Level::GetInstance() 
{
	static Level s_Instance;
	return &s_Instance;
}

Level::~Level() 
{
}

void Level::Update(float fElapsedTime)
{
	fElapsedTime;
	/*DirectInput* pDI = DirectInput::GetInstance();

	if(pDI->KeyPressed(DIK_P) )
	{
		m_bNoClip = !m_bNoClip;
	}*/
	if( GamePlayState::GetInstance()->GetPlayer()->IsOn() )
	{
		CheckTriangleCollisions();
	}
}
bool Level::CheckIntersect( RECT* rect1, RECT* rect2)
{
	if(rect1->right <= rect2->left)
		return false;
	if(rect1->left >= rect2->right)
		return false;
	if(rect1->bottom <= rect2->top) 
		return false;
	if(rect1->top    >= rect2->bottom)
		return false;
	return true;

}

void Level::Render() 
{
	POINTFLOAT cam = GamePlayState::GetInstance()->GetCamera();

	ViewManager* pView = ViewManager::GetInstance();


	CGame* pGame = CGame::GetInstance();

	RECT cull;
	cull.left = (long)cam.x;
	cull.top = (long)cam.y;
	cull.right = (long)cam.x+pGame->GetScreenWidth();
	cull.bottom = (long)cam.y+pGame->GetScreenHeight(); 

	//RECT intersect;

	vector<POINTFLOAT> lightsToRender;
	//RECT cRect;
	RECT camRect = { (LONG)GamePlayState::GetInstance()->GetCamera().x - CGame::GetInstance()->GetScreenWidth(), (LONG)GamePlayState::GetInstance()->GetCamera().y - CGame::GetInstance()->GetScreenHeight(), LONG(GamePlayState::GetInstance()->GetCamera().x + CGame::GetInstance()->GetScreenWidth() * 2), LONG(GamePlayState::GetInstance()->GetCamera().y + CGame::GetInstance()->GetScreenHeight() * 2)};
	vector<int> fireEffects = GamePlayState::GetInstance()->GetFireA();
	for( unsigned int i = 0; lightsToRender.size() < 6 && i < fireEffects.size(); i += 3)
	{
		RECT fire = Particle_Manager::GetInstance()->GetActiveEmitter(fireEffects[i])->GetRect();
		if( CheckIntersect( &camRect, &fire ) == true && CGame::GetInstance()->GetState() == GamePlayState::GetInstance())
		{
			//if(!GamePlayState::GetInstance()->GetPlayer())
			//	break;
	
			POINTFLOAT tmp;
			tmp.x = (((((fire.left + fire.right) * .5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosX() )));

			tmp.y = (((((fire.bottom + fire.top) *.5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosY() )));
			lightsToRender.push_back(tmp);
		}
	}
	vector<RECT> streetLights = GamePlayState::GetInstance()->GetStreelights();
	for( unsigned int i = 0; lightsToRender.size() < 6 && i < streetLights.size(); ++i)
	{
		if(	CheckIntersect(&camRect, &streetLights[i] ) == true && CGame::GetInstance()->GetState() == GamePlayState::GetInstance())
		{
			//if(!GamePlayState::GetInstance()->GetPlayer())
			//	break;
	
			POINTFLOAT tmp;	
			tmp.x = (((((streetLights[i].left + streetLights[i].right) * .5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosX() )));
			tmp.y = (((((streetLights[i].bottom + streetLights[i].top) *.5f) - GamePlayState::GetInstance()->GetPlayer()->GetPosY() )));
			lightsToRender.push_back(tmp);
		}
	}

	unsigned int size = m_vTiles.size();
	
	for(unsigned int i = 0; i < size; i++)
	{
		if( CheckIntersect(&m_vTiles[i].m_TileRect, &cull) == true )
		{
			if( m_vTiles[i].m_Layer == 2)
			{
				Player*	tmp = GamePlayState::GetInstance()->GetPlayer();

				if( tmp->IsOn() &&((tmp->GetLightType() > 1 ) || m_vTiles[i].shadow))
				{
					float angle = 0;
					float x2 = tmp->GetPosX() - m_vTiles[i].m_nWorldPosX;
					float x = x2;
					float y2 = tmp->GetPosY() - m_vTiles[i].m_nWorldPosY;
					float y = y2;
					x2 *= x2;
					y2 *= y2;
					float distance = sqrt(x2 + y2);

					angle = acos(x/distance);
					if( y < 0)
						angle *=  -1;

					angle -= 1.57079f;
					pView->DrawStaticTexture(m_vTiles[i].m_nTileID, (int)m_vTiles[i].m_nWorldPosX-cam.x, (int)m_vTiles[i].m_nWorldPosY-cam.y, 1.0f, 1.02f, &m_vTiles[i].m_rImageRect, 32, 50, angle, D3DCOLOR_ARGB( 200, 0, 0, 0));
				}
				unsigned int lightsize = lightsToRender.size();
				for(unsigned int i = 0; i < lightsize;++i)
				{
					float angle = 0;
					float x2 = lightsToRender[i].x - m_vTiles[i].m_nWorldPosX;
					float x = x2;
					float y2 =lightsToRender[i].y - m_vTiles[i].m_nWorldPosY;
					float y = y2;
					x2 *= x2;
					y2 *= y2;
					float distance = sqrt(x2 + y2);

					angle = acos(x/distance);
					if( y < 0)
						angle *=  -1;

					angle -= 1.57079f;
					pView->DrawStaticTexture(m_vTiles[i].m_nTileID, (int)m_vTiles[i].m_nWorldPosX-cam.x, (int)m_vTiles[i].m_nWorldPosY-cam.y, 1.0f, 1.02f, &m_vTiles[i].m_rImageRect, 32, 50, angle, D3DCOLOR_ARGB( 200, 0, 0, 0));
				}
			}
			pView->DrawStaticTexture(m_vTiles[i].m_nTileID, (int)m_vTiles[i].m_nWorldPosX-cam.x, (int)m_vTiles[i].m_nWorldPosY-cam.y,1.0f,1.0f, &m_vTiles[i].m_rImageRect );

		}
	}


	// Render Fog
	if(!inside)
	{
		for( int i = 0; i < 4; ++i)
		{
			if(i < 2)
				fog[i].y += 1;
			else
				fog[i].x += 1;

			if( fog[i].x > 1024 )
				fog[i].x = -1024;
			if( fog[i].y > 1024 )
				fog[i].y = -1024;
			D3DCOLOR tmpColor = 0x3FFFFFFF;
			pView->DrawStaticTexture(fogID[i], cam.x - fog[i].x - cam.x, cam.y - fog[i].y - cam.y, 4, 4, nullptr, 0, 0, 0, tmpColor);
		}
	}
	
	pView->GetSprite()->Flush();


}



bool Level::LoadLevel( const char* szFilename )
{
	m_nPosX = 0;
	m_nPosY = 0;
	//CSGD_TextureManager* pTM = CSGD_TextureManager::GetInstance();

	// Create the TinyXML document
	TiXmlDocument doc;

	//Attempt to load the file
	if( doc.LoadFile( szFilename ) == false )
		return false;


	// Access the root element ( "players_list")
	TiXmlElement* pRoot = doc.RootElement();

	if( pRoot == nullptr )
		return false;

	// Clear the existing player data in the vector (if any)
	m_vCollisions.clear();


	//Iterate through the nodes to load player data
	TiXmlElement* pLevel = pRoot->FirstChildElement();


	vector<TwoInts> ids;


	if( pLevel != nullptr )
	{
		string p = pLevel->Value();

		if( p != "Assets")
		{
			return false;
		}

		while( pLevel != nullptr )
		{

			const char* pText = pLevel->GetText();

			string test;
			if( pText != nullptr )
			{
				test = pText;
			}
			else
			{
				return false;
			}
			test = "resource/graphics/"+test;
			
			TwoInts tmp;
			
			pLevel->Attribute("TileID",&tmp.x);

			//ids.push_back(tmp);

			char filepath[100];
			if( pText != nullptr )
				strcpy_s( filepath, 100, test.c_str() );


			//TCHAR buffer[100];
			//mbstowcs_s( nullptr, buffer, 100, filepath, _TRUNCATE );
			
			tmp.y =  ViewManager::GetInstance()->RegisterTexture(filepath);

			ids.push_back(tmp);

			pLevel = pLevel->NextSiblingElement();

			string tmp123 = pLevel->Value();

			if( tmp123 == "Tile")
			{
				break;
			}
		}


		while( pLevel != nullptr )
		{
			//Read info from the node
			mapTiles info = {};

			TiXmlElement* pTiles = pLevel->FirstChildElement();

			int tmpX,tmpY,tmpH,tmpW,tmpX2,tmpY2,tmpID,tmpLayer;
			pLevel->Attribute("layer",&tmpLayer);
			pTiles->Attribute( "x", &tmpX );
			pTiles->Attribute( "y", &tmpY );
			pTiles->Attribute( "width", &tmpW );
			pTiles->Attribute( "height", &tmpH );

			info.m_Layer = tmpLayer;
			info.m_rImageRect.left = (long)tmpX;
			info.m_rImageRect.top = (long)tmpY;
			info.m_rImageRect.right = long(tmpX+tmpW);
			info.m_rImageRect.bottom = long(tmpY+tmpH);
			info.height = (float)tmpH;
			info.width = (float)tmpW;

			pTiles = pTiles->NextSiblingElement();


			pTiles->Attribute("x", &tmpX2);
			pTiles->Attribute("y", &tmpY2);

			info.m_nWorldPosX = (float)tmpX2;
			info.m_nWorldPosY = (float)tmpY2;



			pTiles = pTiles->NextSiblingElement();

			pTiles->Attribute("ID",&tmpID);

			for(unsigned int i = 0; i < ids.size(); i++)
			{
				if( tmpID == ids[i].x )
				{
					tmpID = ids[i].y;
					break;
				}
			}
			info.m_nTileID = tmpID;

			RECT tmp = { (long)tmpX2,(long)tmpY2,long(tmpX2+tmpW),long(tmpY2+tmpH)};
			info.m_TileRect = tmp;

			m_vTiles.push_back(info);

			pLevel = pLevel->NextSiblingElement();
			string tmp123 = pLevel->Value();

			if( tmp123 == "Collision")
			{
				break;
			}

		}

		while( pLevel != nullptr )
		{
			leveldata info = {};
			TiXmlElement* pCollision = pLevel->FirstChildElement();

			//Read name
			const char* pText = pCollision->GetText();
			if( pText != nullptr )
				strcpy_s( info.m_cType, 32, pText );

			pCollision = pCollision->NextSiblingElement();

			//// Read attributes
			//pLevel->Attribute( "score", &info.nScore );
			int tmpX,tmpY,tmpH,tmpW;
			pCollision->Attribute( "x", &tmpX );
			pCollision->Attribute( "y", &tmpY );
			pCollision->Attribute( "width", &tmpW );
			pCollision->Attribute( "height", &tmpH );

			info.m_rCollision.left = (long)tmpX;
			info.m_rCollision.top = (long)tmpY;
			info.m_rCollision.right = (long)(tmpX+tmpW);
			info.m_rCollision.bottom = (long)(tmpY+tmpH);

			info.m_bPrevColliding = false;

			info.height = tmpH;
			info.width = tmpW;
			info.x = tmpX;
			info.y = tmpY;

			// Save this info to the vector
			m_vCollisions.push_back( info );

			pLevel = pLevel->NextSiblingElement();
		}


	}
	else
	{
		return false;
	}

	return true;
}



bool Level::CheckCollision(IObjects* pBase)
{
	bool checkcol = false;

	if( m_bNoClip == false )
	{
		for(unsigned int i = 0; i < m_vCollisions.size(); i++)
		{
			RECT cRect;
			RECT temp = pBase->GetRect();
			if( IntersectRect(&cRect, &m_vCollisions[i].m_rCollision, &temp ) == FALSE )
			{
				if( m_vCollisions[i].m_bPrevColliding == true )
				{
				}
				continue;
			}
			else
			{
				checkcol = true;
				if( pBase->GetObjectType() == OBJ_BULLET )
				{
					DestroyBullet* pMsg = new DestroyBullet((Bullet*)pBase);
					MessageSystem::GetInstance()->SendMsg(pMsg);
					pMsg = nullptr;
					return true;
				}
				else if(pBase->GetObjectType() == OBJ_CHARACTER)
				{
					BaseCharacter* pCh = (BaseCharacter*)pBase;

					if( _stricmp(m_vCollisions[i].m_cType,"Pickup") != 0 && _stricmp(m_vCollisions[i].m_cType,"Town") != 0 && _stricmp(m_vCollisions[i].m_cType,"Hospital") != 0 && _stricmp(m_vCollisions[i].m_cType,"House") != 0 )
					{
						int check = 0;


						float intmid = float(cRect.top + cRect.bottom) / 2.0f;
						float intmidx = float(cRect.left + cRect.right) / 2.0f;


						float tilemid = float(m_vCollisions[i].m_rCollision.top + m_vCollisions[i].m_rCollision.bottom) / 2.0f;
						float tilemidx = float(m_vCollisions[i].m_rCollision.left + m_vCollisions[i].m_rCollision.right) / 2.0f;


						long x = cRect.bottom - cRect.top;
						long y = cRect.right - cRect.left;

						if( x != y )
						{
							if( y > x )
							{
								if( intmid < tilemid )
								{
									check = 1;
								}
								else
								{
									check = 2;
								}
							}

							if( y < x )
							{
								if( intmidx < tilemidx )
								{
									check = 3;
								}
								else
								{
									check = 4;
								}
							}

						}
						else
						{
							for(unsigned int j = 0; j < m_vCollisions.size(); j++)
							{

								if(  m_vCollisions[j].m_bPrevColliding == true)
								{
									check = m_vCollisions[j].test;
									break;
								}

								if( i == j )
								{
									if(  m_vCollisions[i].m_bPrevColliding == true)
									{
										check = m_vCollisions[i].test;
										break;
									}
								}
							}

							//check = 0;
							if( check == 0 )
							{
							}
						}

						if (check == 1)
						{
							pCh->SetPosY(pCh->GetPosY()-x);
							m_vCollisions[i].m_bPrevColliding = true;
							m_vCollisions[i].test = check;
						}
						else if (check == 2)
						{
							pCh->SetPosY(pCh->GetPosY()+x);
							m_vCollisions[i].m_bPrevColliding = true;
							m_vCollisions[i].test = check;
						}
						else if (check == 3)
						{
							pCh->SetPosX(pCh->GetPosX()-y);
							m_vCollisions[i].m_bPrevColliding = true;
							m_vCollisions[i].test = check;
						}
						else if (check == 4)
						{
							pCh->SetPosX(pCh->GetPosX()+y);
							m_vCollisions[i].m_bPrevColliding = true;
							m_vCollisions[i].test = check;
						}
					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"Pickup") == 0 )
					{
						EventSystem::GetInstance()->SendUniqueEvent( "got_pickup", pBase );
					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"Town") == 0 && whichlevel == FOREST )
					{
						EventSystem::GetInstance()->SendUniqueEvent( "ForestToTown", pBase );
					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"Town") == 0 && whichlevel == HOSPITAL )
					{
						EventSystem::GetInstance()->SendUniqueEvent( "HospitalToTown", pBase );
					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"Town") == 0 && whichlevel == HOUSE )
					{
						EventSystem::GetInstance()->SendUniqueEvent( "HouseToTown", pBase );
					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"House") == 0 )
					{
						EventSystem::GetInstance()->SendUniqueEvent( "GoToHouse", pBase );

					}
					else if(  _stricmp(m_vCollisions[i].m_cType,"Hospital") == 0 && GamePlayState::GetInstance()->GetPlayer()->m_vpActiveQuests.size() > 0)
					{
						EventSystem::GetInstance()->SendUniqueEvent( "GoToHospital", pBase );
					}
					if(pCh->GetCharacterType() == CHA_BOSS2)
					{
						EventSystem::GetInstance()->SendUniqueEvent( "bossWall_hit", pBase );
					}
				}

			}
		}

		if( checkcol == true )
		{
			return true;
		}

	}

	return false;
}

void Level::CheckTriangleCollisions()
{
	GamePlayState* gamePlay = GamePlayState::GetInstance();
	CGame* game = CGame::GetInstance();
	Player* player = gamePlay->GetPlayer();
	POINTFLOAT cam = gamePlay->GetCamera();
	int height = game->GetScreenHeight();
	int width = game->GetScreenWidth();

	float angleA = acos(ViewManager::GetInstance()->GetOuterCone());
	float angleB = 1.57079f;
	float angleC = 3.14159f - (angleA + angleB);
	angleC = (float)sin(double(angleC));
	angleA = (float)sin(double(angleA));
	float playerX = player->GetPosX();
	float playerY = player->GetPosY();
	int playerDirection = player->GetDirection();

	float lightEndX=0, lightEndY=0, distanceC=0, distanceA=0;
	float point1X=0, point1Y=0, point2X=0, point2Y=0, point3X=0, point3Y=0;

	bool a=false, b=false, c=false;
	for(unsigned int i = 0; i < m_vTiles.size(); i++)
	{
		if(m_vTiles[i].m_Layer == 1)
			continue;
		switch(playerDirection)
		{

		case 0:
			{
				lightEndX = (cam.x);
				lightEndY = playerY;
			}
			break;
		case 1:
			{
				lightEndX = playerX;
				lightEndY = (cam.y);
			}
			break;
		case 2:
			{
				lightEndX = playerX + (width >> 1);
				lightEndY = playerY;
			}
			break;
		case 3:
			{
				lightEndX = playerX;
				lightEndY = playerY + (height >> 1);
			}
			break;
		case 4:
			{
				lightEndX = cam.x;
				lightEndY = cam.y;
			}
			break;
		case 5:
			{
				lightEndX = cam.x + width;
				lightEndY = cam.y;
			}
			break;
		case 6:
			{
				lightEndX = cam.x;
				lightEndY = cam.y + height;
			}
			break;
		case 7:
			{
				lightEndX = cam.x + width;
				lightEndY = cam.y + height;
			}
			break;
		default:
			break;
		}


		float x2 = (lightEndX - playerX);
		x2 *= x2;
		float y2 = (lightEndY - playerY);
		y2 *= y2;
		distanceC = (float)sqrt(double(x2 + y2));
		distanceA = (distanceC/angleC) * angleA;

		// Make final triangle
		switch(playerDirection)
		{
		case 0:
		case 2:
			point1X = lightEndX;
			point1Y = lightEndY + distanceA;
			point2X = lightEndX;
			point2Y = lightEndY - distanceA;
			break;
		case 1:
		case 3:
			point1X = lightEndX + distanceA;
			point1Y = lightEndY;
			point2X = lightEndX - distanceA;
			point2Y = lightEndY;
			break;
		case 4:
		case 7:
			point1X = lightEndX + (distanceA * .5f);
			point1Y = lightEndY + (distanceA * .5f);
			point2X = lightEndX - (distanceA * .5f);
			point2Y = lightEndY - (distanceA * .5f);
			break;
		case 5:
		case 6:
			point1X = lightEndX - (distanceA * .5f);
			point1Y = lightEndY + (distanceA * .5f);
			point2X = lightEndX + (distanceA * .5f);
			point2Y = lightEndY - (distanceA * .5f);
			break;
		default:
			break;
		}
		point3X = playerX;
		point3Y = playerY;

		RECT tmpRect = { (long)m_vTiles[i].m_nWorldPosX, (long)m_vTiles[i].m_nWorldPosY, long(m_vTiles[i].m_nWorldPosX + m_vTiles[i].width), long(m_vTiles[i].m_nWorldPosY + m_vTiles[i].height)};
		// left, top
		a = ((tmpRect.left /*+ cam.x*/ - point2X) * (point1Y - point2Y) - (point1X - point2X) * (tmpRect.top /*+ cam.y*/ - point2Y)) < 0.0f;
		b = ((tmpRect.left /*+ cam.x*/ - point3X) * (point2Y - point3Y) - (point2X - point3X) * (tmpRect.top /*+ cam.y*/ - point3Y)) < 0.0f;
		c = ((tmpRect.left /*+ cam.x*/ - point1X) * (point3Y - point1Y) - (point3X - point1X) * (tmpRect.top /*+ cam.y*/ - point1Y)) < 0.0f;
		m_vTiles[i].shadow = ((a == b) && (b == c));
		if(m_vTiles[i].shadow)
			continue;

		// left, bottom
		a = ((tmpRect.left /*+ cam.x*/ - point2X) * (point1Y - point2Y) - (point1X - point2X) * (tmpRect.bottom /*+ cam.y*/ - point2Y)) < 0.0f;
		b = ((tmpRect.left /*+ cam.x*/ - point3X) * (point2Y - point3Y) - (point2X - point3X) * (tmpRect.bottom /*+ cam.y*/ - point3Y)) < 0.0f;
		c = ((tmpRect.left /*+ cam.x*/ - point1X) * (point3Y - point1Y) - (point3X - point1X) * (tmpRect.bottom /*+ cam.y*/ - point1Y)) < 0.0f;		
		m_vTiles[i].shadow = ((a == b) && (b == c));
		if(m_vTiles[i].shadow)
			continue;

		// right, top
		a = ((tmpRect.right /*+ cam.x*/ - point2X) * (point1Y - point2Y) - (point1X - point2X) * (tmpRect.top /*+ cam.y*/ - point2Y)) < 0.0f;
		b = ((tmpRect.right /*+ cam.x*/ - point3X) * (point2Y - point3Y) - (point2X - point3X) * (tmpRect.top /*+ cam.y*/ - point3Y)) < 0.0f;
		c = ((tmpRect.right /*+ cam.x*/ - point1X) * (point3Y - point1Y) - (point3X - point1X) * (tmpRect.top /*+ cam.y*/ - point1Y)) < 0.0f;
		m_vTiles[i].shadow = ((a == b) && (b == c));
		if(m_vTiles[i].shadow)
			continue;
		
		// right, bottom;
		a = ((tmpRect.right /*+ cam.x*/ - point2X) * (point1Y - point2Y) - (point1X - point2X) * (tmpRect.bottom /*+ cam.y*/ - point2Y)) < 0.0f;
		b = ((tmpRect.right /*+ cam.x*/ - point3X) * (point2Y - point3Y) - (point2X - point3X) * (tmpRect.bottom /*+ cam.y*/ - point3Y)) < 0.0f;
		c = ((tmpRect.right /*+ cam.x*/ - point1X) * (point3Y - point1Y) - (point3X - point1X) * (tmpRect.bottom /*+ cam.y*/ - point1Y)) < 0.0f;
		m_vTiles[i].shadow = ((a == b) && (b == c));
	}
}