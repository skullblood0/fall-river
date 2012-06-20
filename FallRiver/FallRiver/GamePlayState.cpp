#include "GamePlayState.h"
#include "ViewManager.h"
#include "AudioManager.h"
#include "ObjectManager.h"
#include "OptionsMenuState.h"
#include "PauseMenuState.h"
#include "Level.h"
#include "CGame.h"
#include "DirectInput.h"
#include "Enemy.h"
#include "EventSystem.h"
#include "Player.h"
#include "Weapon.h"
#include "Particle_Manager.h"
#include "HUD.h"
#include "XMLManager.h"
#include "Message.h"
#include "Bullet.h"
#include "ChasingAI.h"
#include "ShootingAi.h"
#include "NPC.h"
#include "PickUp.h"
#include "CreateBullet.h"
#include "DestroyBullet.h"
#include "DestroyEnemyC.h"
#include "DestroyEnemyS.h"
#include "DestroyNPC.h"
#include "DestroyPickUp.h"

GamePlayState::GamePlayState()
{
	m_pVM = nullptr;
	m_pDI = nullptr;
	m_pAM = nullptr;
	m_pOF = nullptr;
	m_pOM = nullptr;
	m_pES = nullptr;
	m_cPlayer = nullptr;

	m_cWeapon = nullptr;

	m_bCanMoveLeft	= true;
	m_bCanMoveRight	= true;
	m_bCanMoveUp	= true;
	m_bCanMoveDown	= true;
}

GamePlayState* GamePlayState::GetInstance() 
{
	static GamePlayState s_Instance;

	return &s_Instance;
}

void GamePlayState::Enter()
{
	m_pDI = DirectInput::GetInstance();
	m_pVM = ViewManager::GetInstance();
	m_pOF = Factory::GetInstance();
	m_pOM = ObjectManager::GetInstance();
	m_pES = EventSystem::GetInstance();
	m_pMS = MessageSystem::GetInstance();

	m_pOF->RegisterClassType< BaseObject	>( _T("BaseObject") );
	m_pOF->RegisterClassType< Player		>( _T("Player") );
	m_pOF->RegisterClassType< Weapon		>( _T("Weapon") );
	m_pOF->RegisterClassType< NPC			>( _T("NPC") );
	m_pOF->RegisterClassType< Enemy			>( _T("Enemy") );
	m_pOF->RegisterClassType< ShootingAi	>( _T("ShootingAi") );
	m_pOF->RegisterClassType< ChasingAI		>( _T("ChasingAI") );
	m_pOF->RegisterClassType< Bullet		>( _T("Bullet") );

	m_clevel.LoadLevel("level.xml");

	Player* pPlayer = nullptr;
	Weapon* pWeapon = nullptr;

	if(m_cPlayer == nullptr)
	{
		m_cPlayer = (Player*)m_pOF->CreateObject( _T("Player"));
		pPlayer = (m_cPlayer);
		pPlayer->SetHeight(32);
		pPlayer->SetWidth(32);
		pPlayer->SetImageID(-1);
		pPlayer->SetPosX(float(CGame::GetInstance()->GetScreenWidth()*0.5));
		pPlayer->SetPosY(float(CGame::GetInstance()->GetScreenHeight()*0.5));

		pWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		pWeapon->SetHeight(20);
		pWeapon->SetWidth(10);
		pWeapon->SetImageID(-1);
		pWeapon->SetOwner(pPlayer);
		pWeapon->Init(WPN_RIFLE, 100, 10, 0);
		pWeapon->SetPosX(pPlayer->GetPosX()+pPlayer->GetWidth()/2);
		pWeapon->SetPosY(pPlayer->GetPosY());

		pPlayer->AddWeapon(pWeapon);

	}
	else
	{
		//m_cPlayer = (Player*)m_pOF->CreateObject( _T("Player"));
		pPlayer = (m_cPlayer);
		pPlayer->SetHeight(32);
		pPlayer->SetWidth(32);
		pPlayer->SetImageID(-1);
		pPlayer->SetPosX(m_cPlayer->GetPosX());
		pPlayer->SetPosY(m_cPlayer->GetPosY());

		for(unsigned int i = 0; i < m_cPlayer->GetWeapons().size(); i++)
		{
			pWeapon = m_cPlayer->GetWeapons()[i];
			m_cPlayer->GetWeapons()[i] = (Weapon*)m_pOF->CreateObject( _T("Weapon") );
			m_cPlayer->GetWeapons()[i]->Init(pWeapon->GetWeaponType(), pWeapon->GetAmmo(), pWeapon->GetDamage(), 0);
			m_cPlayer->GetWeapons()[i]->SetPosX(pPlayer->GetPosX()+(pPlayer->GetWidth()/2));
			m_cPlayer->GetWeapons()[i]->SetPosY(pPlayer->GetPosY());
			m_cPlayer->GetWeapons()[i]->SetOwner(pWeapon->GetOwner());
			m_cPlayer->GetWeapons()[i]->SetHeight(pWeapon->GetHeight());
			m_cPlayer->GetWeapons()[i]->SetWidth(pWeapon->GetWidth());
		}
	}

	for(int i = 0; i < 0; i++)
	{
		m_cEnemies.push_back(nullptr);
		m_cEnemies[i] = (ChasingAI*)m_pOF->CreateObject( _T("ChasingAI") );
		ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[i]);
		pEnemy->SetHeight(32);
		pEnemy->SetWidth(32);
		pEnemy->SetImageID(-1);
		pEnemy->SetTarget(m_cPlayer);
		pEnemy->SetPosX(float(50*i+200));
		pEnemy->SetPosY(200);
		pEnemy->SetHealth(100);
		m_pOM->AddObject(pEnemy);
	}

	for(int i = 0; i < 1; i++)
	{
		m_cEnemies.push_back(nullptr);
		m_cEnemies[i] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
		ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[i]);
		pEnemy->SetHeight(32);
		pEnemy->SetWidth(32);
		pEnemy->SetImageID(-1);
		pEnemy->SetTarget(m_cPlayer);
		pEnemy->SetPosX(200);
		pEnemy->SetPosY(100);
		pEnemy->SetHealth(100);
		m_pOM->AddObject(pEnemy);

		Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		eWeapon->SetHeight(20);
		eWeapon->SetWidth(10);
		eWeapon->SetImageID(-1);
		eWeapon->SetOwner(pEnemy);
		eWeapon->Init(WPN_RIFLE, 100, 10, 0);
		eWeapon->SetPosX(pEnemy->GetPosX()+pPlayer->GetWidth()/2);
		eWeapon->SetPosY(pEnemy->GetPosY());
		pEnemy->SetWeapon(eWeapon);
	}

	for(int i = 0; i < 1; i++)
	{
		m_cNpcs.push_back(nullptr);
		m_cNpcs[i] = (NPC*)m_pOF->CreateObject( _T("NPC") );
		NPC* pNpc =(NPC*)(m_cNpcs[i]);
		pNpc->SetHeight(32);
		pNpc->SetWidth(32);
		pNpc->SetImageID(-1);
		pNpc->SetPosX(400);
		pNpc->SetPosY(100);
		m_pOM->AddObject(pNpc);
	}

	m_pOM->AddObject(pPlayer);

	m_pMS->InitMessageSystem( &MessageProc );
}

void GamePlayState::Exit() 
{
	if( m_pES != nullptr )
	{
		m_pES->ClearEvents();
		m_pES->Shutdown();
		m_pES = nullptr;
	}

	if( m_pMS != nullptr )
	{
		m_pMS->ClearMessages();
		m_pMS->Shutdown();
		m_pMS = nullptr;
	}

	if( m_pOM != nullptr )
	{
		m_pOM->RemoveAllObjects();
		ObjectManager::DeleteInstance();
		m_pOM = nullptr;
	}

	if( m_pOF != nullptr )
	{
		m_pOF->ShutdownObjectFactory();
		m_pOF = nullptr;
	}

	for(unsigned int i = 0; i < m_cEnemies.size(); i++)
	{
		m_cEnemies[i] = nullptr;
	}
	m_cEnemies.clear();

	for(unsigned int i = 0; i < m_cNpcs.size(); i++)
	{
		m_cNpcs[i] = nullptr;
	}
	m_cNpcs.clear();

	m_pVM = nullptr;
	m_pDI = nullptr;
	m_pAM = nullptr;
	m_pOF = nullptr;
	m_pOM = nullptr;
	m_pES = nullptr;

	m_cPlayer = nullptr;
}

bool GamePlayState::Input() 
{
	if( m_pDI->KeyPressed(DIK_ESCAPE) )
		CGame::GetInstance()->ChangeState(PauseMenuState::GetInstance());

	return true;
}

void GamePlayState::Update(float fElapsedTime) 
{
	m_clevel.Update(fElapsedTime);
	m_pOM->UpdateAllObjects(fElapsedTime);
	m_pOM->CheckCollisions();
	m_pES->ProcessEvents();
	m_pMS->ProcessMessages();
}

void GamePlayState::Render() 
{
	m_pVM->GetSprite()->Flush();
	m_clevel.Render();

	m_pOM->RenderAllObjects();
}

void GamePlayState::MessageProc(IMessage* pMsg)
{
	GamePlayState* self = GamePlayState::GetInstance();
	switch( pMsg->GetMessageID() )
	{
	case MSG_CREATE_BULLET:
		{
			// Create bullet
			Bullet* bullet = (Bullet*)( self->m_pOF->CreateObject( _T("Bullet") ) );
			Weapon* pOwner = dynamic_cast< CreateBullet* > (pMsg)->GetWeapon();
			//Set up data members			
			bullet->SetImageID( -1 );
			bullet->SetHeight(16);
			bullet->SetWidth(16);
			bullet->SetOwner(pOwner);
			bullet->SetPosX(pOwner->GetPosX());
			bullet->SetPosY(pOwner->GetPosY());

			switch(pOwner->GetOwner()->GetDirection())
			{
			case DIRE_UP:
				{
					bullet->SetSpeedX(0);
					bullet->SetSpeedY(-200);
					break;
				}
			case DIRE_LEFT:
				{
					bullet->SetSpeedX(-200);
					bullet->SetSpeedY(0);
					break;
				}
			case DIRE_RIGHT:
				{
					bullet->SetSpeedX(200);
					bullet->SetSpeedY(0);
					break;
				}
			case DIRE_DOWN:
				{
					bullet->SetSpeedX(0);
					bullet->SetSpeedY(200);
					break;
				}
			case DIRE_UPRIGHT:
				{
					bullet->SetSpeedX(200);
					bullet->SetSpeedY(-200);
					break;
				}
			case DIRE_UPLEFT:
				{
					bullet->SetSpeedX(-200);
					bullet->SetSpeedY(-200);
					break;
				}
			case DIRE_DOWNLEFT:
				{
					bullet->SetSpeedX(-200);
					bullet->SetSpeedY(200);
					break;
				}
			case DIRE_DOWNRIGHT:
				{
					bullet->SetSpeedX(200);
					bullet->SetSpeedY(200);
					break;
				}
			}

			// Add bullet to object manager
			self->m_pOM->AddObject( bullet );
			bullet->Release();
			bullet = nullptr;

			break;
		}
	case MSG_CREATE_ENEMYC:
		{
			// Create ChasingAI
			ChasingAI* enemyc = dynamic_cast< ChasingAI* >( self->m_pOF->CreateObject( _T("ChasingAI") ) );
			//Set up data members
			//enemyc->SetImageID( );
			//enemyc->SetHealth( );
			//enemyc->SetHeight( );
			//enemyc->SetWidth( );
			//enemyc->SetPosX( );
			//enemyc->SetPosY( );
			//enemyc->SetVelX( );
			//enemyc->SetVelY( );

			// Add enemyc to object manager
			self->m_pOM->AddObject(enemyc);
			enemyc->Release();
			enemyc = nullptr;
			break;
		}
	case MSG_CREATE_ENEMYS:
		{
			// Create ShootingAI
			ShootingAi* enemys = dynamic_cast< ShootingAi* >( self->m_pOF->CreateObject( _T("ShootingAi") ) );
			//Set up data members			
			//enemys->SetImageID( );
			//enemys->SetHealth( );
			//enemys->SetHeight( );
			//enemys->SetWidth( );
			//enemys->SetPosX( );
			//enemys->SetPosY( );
			//enemys->SetVelX( );
			//enemys->SetVelY( );

			// Add enemys to object manager
			self->m_pOM->AddObject(enemys);
			enemys->Release();
			enemys = nullptr;
			break;
		}
	case MSG_CREATE_NPC:
		{
			// Create NPC
			NPC* npc = dynamic_cast< NPC* >( self->m_pOF->CreateObject( _T("NPC") ) );
			//Set up data members			
			//npc->SetImageID( );
			//npc->SetHealth( );
			//npc->SetHeight( );
			//npc->SetWidth( );
			//npc->SetPosX( );
			//npc->SetPosY( );
			//npc->SetVelX( );
			//npc->SetVelY( );

			// Add npc to object manager
			self->m_pOM->AddObject(npc);
			npc->Release();
			npc = nullptr;
			break;
		}
	case MSG__CREATE_PICKUP:
		{
			// Create PickUp
			PickUp* pickup = dynamic_cast< PickUp* >( self->m_pOF->CreateObject( _T("PickUp") ) );
			//Set up data members
			//pickup->SetHeight( );
			//pickup->SetImageID( );
			//pickup->SetPosX( );
			//pickup->SetPosY( );
			//pickup->SetWidth( );

			// Add pickup to object manager
			self->m_pOM->AddObject(pickup);
			pickup->Release();
			pickup = nullptr;
			break;
		}
	case MSG_DESTROY_BULLET:
		{
			Bullet* bullet = dynamic_cast<DestroyBullet*>(pMsg)->GetBullet();
			self->m_pOM->RemoveObject( bullet );
			break;
		}
	case MSG_DESTROY_ENEMYC:
		{
			ChasingAI* enemyc = dynamic_cast<DestroyEnemyC*>(pMsg)->GetEnemyC();
			self->m_pOM->RemoveObject( enemyc );
			break;
		}
	case MSG_DESTROY_ENEMYS:
		{
			ShootingAi* enemys = dynamic_cast<DestroyEnemyS*>(pMsg)->GetEnemyS();
			self->m_pOM->RemoveObject( enemys );
			break;
		}
	case MSG_DESTROY_NPC:
		{
			NPC* npc = dynamic_cast<DestroyNPC*>(pMsg)->GetNPC();
			self->m_pOM->RemoveObject( npc );
			break;
		}
	case MSG_DESTROY_PICKUP:
		{
			PickUp* pickup = dynamic_cast<DestroyPickUp*>(pMsg)->GetPickUp();
			self->m_pOM->RemoveObject( pickup );
			break;
		}
	}
}