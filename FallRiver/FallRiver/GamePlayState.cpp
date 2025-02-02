#include "GamePlayState.h"
#include "ViewManager.h"
#include "AudioManager.h"
#include "ObjectManager.h"
#include "OptionsMenuState.h"
#include "LoadingScreen.h"
#include "PauseMenuState.h"
#include "Level.h"
#include "Particle_Manager.h"
#include "Emitter.h"
#include "CGame.h"
#include "DirectInput.h"
#include "DestroyEnemy.h"
#include "Enemy.h"
#include "EventSystem.h"
#include "Player.h"
#include "CompanionAI.h"
#include "Weapon.h"
#include "HUD.h"
#include "XMLManager.h"
#include "Message.h"
#include "Bullet.h"
#include "ChasingAI.h"
#include "Boss1.h"
#include "Boss2.h"
#include "ShootingAi.h"
#include "NPC.h"
#include "PickUp.h"
#include "CreateBullet.h"
#include "DestroyBullet.h"
#include "DestroyEnemyC.h"
#include "DestroyEnemyS.h"
#include "DestroyNPC.h"
#include "DestroyPickUp.h"
#include "Bush.h"
#include "SpawnPoint.h"
#include "LoseMenuState.h"
#include "WinMenuState.h"


GamePlayState::GamePlayState()
{
	m_pVM = nullptr;
	m_pDI = nullptr;
	m_pAM = nullptr;
	m_pOF = nullptr;
	m_pOM = nullptr;
	m_pES = nullptr;
	m_pPM = nullptr;
	m_cPlayer = nullptr;
	m_cBoss1 = nullptr;
	m_cBoss2 = nullptr;
	m_cWeapon = nullptr;

	//	backGroundID = -1;
	swingHitID = -1;
	SpawnEnemyAniID = -1;
	logID = -1;

	winLose = true;
	questFlag = false;
	rainA = -1;
	rainL = -1;
	bush = -1;
	smokeL = -1;

	fire1L = -1;
	fire2L = -1;
	fire3L = -1;

	bloodL1 = -1;
	bloodL2 = -1;
	bloodL3 = -1;
	bloodL4 = -1;
	bloodL5 = -1;
	bloodL6 = -1;
	bloodL7 = -1;
	bloodL8 = -1;
	goreL1 = -1;
	goreL2 = -1;
	goreL3 = -1;
	goreL4 = -1;
	goreL5 = -1;
	goreL6 = -1;
	goreL7 = -1;
	goreL8 = -1;
	townX = 0;
	townY = 0;
	hospitalX = 0;
	hospitalY = 0;


	loadedLevel = -1;
	//	pianoID = -1;

}

GamePlayState* GamePlayState::GetInstance() 
{
	static GamePlayState s_Instance;

	return &s_Instance;
}

void GamePlayState::Enter()
{
	LoadingScreen* loading = LoadingScreen::GetInstance();

	m_pDI = DirectInput::GetInstance();
	m_pVM = ViewManager::GetInstance();
	m_pOF = Factory::GetInstance();
	m_pOM = ObjectManager::GetInstance();
	m_pES = EventSystem::GetInstance();
	m_pMS = MessageSystem::GetInstance();
	m_pPM = Particle_Manager::GetInstance();
	m_pAM = AudioManager::GetInstance();

	m_pVM->SetLightPos(0, 0, -1.0f);
	m_pVM->SetSpotLightPos(0, 0, -.85f);
	m_pVM->SetInnerCone(.95f);
	m_pVM->SetOuterCone(.9f);

	loading->Render();

	m_pVM->SetAmbientLight( .1f, .1f, .0f);

	// Rain particles
	rainL = m_pPM->LoadEmitter("rain.xml");
	rainA = m_pPM->ActivateEmitter(rainL);
	// Smoke particles
	smokeL = m_pPM->LoadEmitter("smoke.xml");

	fire1L = m_pPM->LoadEmitter("fire1.xml");
	fire2L = m_pPM->LoadEmitter("fire2.xml");
	fire3L = m_pPM->LoadEmitter("fire3.xml");

	bloodL1	= m_pPM->LoadEmitter("blood1.xml");
	bloodL2 = m_pPM->LoadEmitter("blood2.xml");
	bloodL3 = m_pPM->LoadEmitter("blood3.xml");
	bloodL4 = m_pPM->LoadEmitter("blood4.xml");
	bloodL5 = m_pPM->LoadEmitter("blood5.xml");
	bloodL6 = m_pPM->LoadEmitter("blood6.xml");
	bloodL7 = m_pPM->LoadEmitter("blood7.xml");
	bloodL8 = m_pPM->LoadEmitter("blood8.xml");
	goreL1 = m_pPM->LoadEmitter("gore1.xml");
	goreL2 = m_pPM->LoadEmitter("gore2.xml");
	goreL3 = m_pPM->LoadEmitter("gore3.xml");
	goreL4 = m_pPM->LoadEmitter("gore4.xml");
	goreL5 = m_pPM->LoadEmitter("gore5.xml");
	goreL6 = m_pPM->LoadEmitter("gore6.xml");
	goreL7 = m_pPM->LoadEmitter("gore7.xml");
	goreL8 = m_pPM->LoadEmitter("gore8.xml");

	loading->Update();
	loading->Render();

	bush = m_pVM->RegisterTexture("resource//graphics//Bush.png");
	SpawnEnemyAniID = m_pVM->RegisterAnimation("resource/graphics/Zombies.xml");
	logID = m_pVM->RegisterTexture("resource/graphics/logs.png");

	EventSystem::GetInstance()->RegisterClient( "ForestToTown", this );
	EventSystem::GetInstance()->RegisterClient( "HospitalToTown", this );
	EventSystem::GetInstance()->RegisterClient( "HouseToTown", this );
	EventSystem::GetInstance()->RegisterClient( "GoToHouse", this );
	EventSystem::GetInstance()->RegisterClient( "GoToHospital", this );

	m_pOF->RegisterClassType< BaseObject	>( _T("BaseObject") );
	m_pOF->RegisterClassType< Level			>( _T("Level") );
	m_pOF->RegisterClassType< Player		>( _T("Player") );
	m_pOF->RegisterClassType< Weapon		>( _T("Weapon") );
	m_pOF->RegisterClassType< Bush			>( _T("Bush") );
	m_pOF->RegisterClassType< NPC			>( _T("NPC") );
	m_pOF->RegisterClassType< PickUp		>( _T("PickUp") );
	m_pOF->RegisterClassType< Enemy			>( _T("Enemy") );
	m_pOF->RegisterClassType< ShootingAi	>( _T("ShootingAi") );
	m_pOF->RegisterClassType< CompanionAI	>( _T("CompanionAI") );
	m_pOF->RegisterClassType< ChasingAI		>( _T("ChasingAI") );
	m_pOF->RegisterClassType< Boss1			>( _T("Boss1") );
	m_pOF->RegisterClassType< Boss2			>( _T("Boss2") );
	m_pOF->RegisterClassType< Bullet		>( _T("Bullet") );
	m_pOF->RegisterClassType< SpawnPoint	>( _T("SpawnPoint") );

	loading->Update();
	loading->Render();

	Player* pPlayer = nullptr;
	Weapon* pWeapon = nullptr;
	Level* pLevel = nullptr;
	Bush* pBush = nullptr;
	SpawnPoint* pSpawn = nullptr;
	PickUp* pPickUp = nullptr;

	m_pVM->ClearOtherLights();


	if(m_cPlayer == nullptr)
	{
		m_cPlayer = (Player*)m_pOF->CreateObject( _T("Player"));
		pPlayer = (m_cPlayer);
		pPlayer->SetHeight(32);
		pPlayer->SetWidth(32);
		pPlayer->SetImageID(-1);
		pPlayer->SetPosX(600);
		pPlayer->SetPosY(500);


		Weapon* pWeapon2 = nullptr;
		Weapon* pWeapon3 = nullptr;
		Weapon* pWeapon4 = nullptr;

		pWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		pWeapon->SetHeight(20);
		pWeapon->SetWidth(10);
		pWeapon->SetImageID(-1);
		pWeapon->SetOwner(pPlayer);
		pWeapon->Init(WPN_PISTOL, 50, 0);
		pWeapon->SetPosX(pPlayer->GetPosX()+pPlayer->GetWidth()/2);
		pWeapon->SetPosY(pPlayer->GetPosY());

		pWeapon2 = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		pWeapon2->SetHeight(20);
		pWeapon2->SetWidth(10);
		pWeapon2->SetImageID(-1);
		pWeapon2->SetOwner(pPlayer);
		pWeapon2->Init(WPN_SHOTGUN, 30, 0);
		pWeapon2->SetPosX(pPlayer->GetPosX()+pPlayer->GetWidth()/2);
		pWeapon2->SetPosY(pPlayer->GetPosY());

		pWeapon3 = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		pWeapon3->SetHeight(20);
		pWeapon3->SetWidth(10);
		pWeapon3->SetImageID(-1);
		pWeapon3->SetOwner(pPlayer);
		pWeapon3->Init(WPN_RIFLE, 100, 0);
		pWeapon3->SetPosX(pPlayer->GetPosX()+pPlayer->GetWidth()/2);
		pWeapon3->SetPosY(pPlayer->GetPosY());

		pWeapon4 = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
		pWeapon4->SetHeight(20);
		pWeapon4->SetWidth(10);
		pWeapon4->SetImageID(-1);
		pWeapon4->SetOwner(pPlayer);
		pWeapon4->Init(WPN_MACHETE, 100, 0);
		pWeapon4->SetPosX(pPlayer->GetPosX()+pPlayer->GetWidth()/2);
		pWeapon4->SetPosY(pPlayer->GetPosY());

		pPlayer->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/PlayerAnimations.xml"));

		pPlayer->AddWeapon(pWeapon2);
		pPlayer->AddWeapon(pWeapon3);
		pPlayer->AddWeapon(pWeapon);
		pPlayer->AddWeapon(pWeapon4);

	}
	else
	{
		pPlayer = (m_cPlayer);
		pPlayer->SetHeight(32);
		pPlayer->SetWidth(32);
		pPlayer->SetImageID(-1);
		pPlayer->SetPosX(m_cPlayer->GetPosX());
		pPlayer->SetPosY(m_cPlayer->GetPosY());
		pPlayer->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/PlayerAnimations.xml"));

		for(unsigned int i = 0; i < m_cPlayer->GetWeapons().size(); i++)
		{
			pWeapon = m_cPlayer->GetWeapons()[i];
			m_cPlayer->GetWeapons()[i] = (Weapon*)m_pOF->CreateObject( _T("Weapon") );
			m_cPlayer->GetWeapons()[i]->Init(pWeapon->GetWeaponType(), pWeapon->GetAmmo(), 0);
			m_cPlayer->GetWeapons()[i]->SetPosX(pPlayer->GetPosX()+(pPlayer->GetWidth()/2));
			m_cPlayer->GetWeapons()[i]->SetPosY(pPlayer->GetPosY());
			m_cPlayer->GetWeapons()[i]->SetOwner(pWeapon->GetOwner());
			m_cPlayer->GetWeapons()[i]->SetHeight(pWeapon->GetHeight());
			m_cPlayer->GetWeapons()[i]->SetWidth(pWeapon->GetWidth());
		}
	}
	m_pHUD = new HUD;

	loading->Update();
	loading->Render();

	m_pMS->InitMessageSystem( &MessageProc );

	//	backGroundID = m_pAM->registerMusic("resource/Sounds/background.mp3");
	//	pianoID = m_pAM->registerMusic("resource/Sounds/piano.mp3");
	swingHitID = m_pAM->RegisterSound("resource/Sounds/swingHit.mp3");
	FMOD_VECTOR sound1 = { 0, 0, 0 };

	//	m_pAM->setMusicPos(backGroundID, sound1);
	//	m_pAM->setMusicVel(backGroundID, sound1);
	//	m_pAM->setMusicLooping(backGroundID, true);
	//	m_pAM->playMusic(backGroundID);
	//	m_pAM->setMusicPos(pianoID, sound1);
	//	m_pAM->setMusicVel(pianoID, sound1);
	//	m_pAM->setMusicLooping(pianoID, true);
	//	m_pAM->playMusic(pianoID);

	soundID2 = m_pAM->RegisterSound("resource/Sounds/thunder.wav");
	m_pAM->setSoundPos(soundID2, sound1);

	m_pAM->setSoundVel(soundID2, sound1);
	m_pAM->setSoundLooping(soundID2, false);

	//	musicID = m_pAM->registerMusic("resource/Sounds/rainroof.wav");
	//	m_pAM->setMusicPos(musicID, sound1);

	//	m_pAM->setMusicVel(musicID, sound1);
	//	m_pAM->setMusicLooping(musicID, true);
	//	m_pAM->playMusic(musicID);
	winLose = true;

	loading->Update();
	loading->Render();

	m_pHUD->m_nHudID = m_pVM->RegisterTexture("resource//graphics//sprites_HUD.png");
	m_pHUD->m_nArrowID = m_pVM->RegisterTexture("resource//graphics//Arrow.png");
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_01.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_02.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_03.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_04.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_05.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_06.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_07.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_08.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_09.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_10.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_11.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_12.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_13.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_14.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_15.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_16.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_17.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_18.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_19.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_20.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_21.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_22.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_23.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_24.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_25.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_26.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_27.png.png"));
	m_pHUD->m_vFrameIDs.push_back( m_pVM->RegisterTexture("resource//graphics//health_animation//health_anm_28.png.png"));

	loading->Update();
	loading->Render();


	if( pLevel == nullptr && loadedLevel <= 0)
	{
		m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
		pLevel = m_clevel;
		pLevel->LoadLevel("level.xml");
		pLevel->whichlevel = FOREST;
		m_pOM->AddObject(pLevel);


		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/pills.png"));
				pPickUp->SetPickUpType(MEDICINE);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(5);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				pNpc->SetQuest(m_cNpcs.size());
				pNpc->SetLabel(m_cNpcs.size()-1);
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				if( m_cNpcs.size() == 2 )
				{
					m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				}
				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget(float(nth->x), float(nth->y));
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(64);
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();
		}
		pLevel->SetCollision(tmp);
		m_pOM->AddObject(pPlayer);

	}
	else
	{
		if(loadedLevel == TOWN )
		{
			EventSystem::GetInstance()->SendUniqueEvent( "ForestToTown", nullptr);
		}
		else if(loadedLevel == HOUSE)
		{
			EventSystem::GetInstance()->SendUniqueEvent( "GoToHouse", nullptr);
		}
		else if(loadedLevel == HOSPITAL)
		{
			EventSystem::GetInstance()->SendUniqueEvent( "GoToHospital", nullptr);
		}
		m_pES->ProcessEvents();
		loadedLevel  = -1;
	}

	m_pVM->SetAmbientLight( .0f, .0f, .0f);

	loading->Reset();
	loading = nullptr;
}

void GamePlayState::ReEnter()
{
	//	m_pAM->playMusic(backGroundID);
	//	m_pAM->playMusic(pianoID);
	//	m_pAM->playMusic(musicID);
}

void GamePlayState::Exit() 
{
	m_pVM->SetAmbientLight( 1.0f, 1.0f, 1.0f);

	for( unsigned int i = 0; i < this->fireA.size(); i++)
	{
		Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
	}

	fireA.clear();
	streetLights.clear();

	if( m_pES != nullptr )
	{
		m_pES->ClearEvents();
		m_pES->Shutdown();
		m_pES = nullptr;
	}

	m_pPM->Shutdown();


	if( m_pMS != nullptr )
	{
		m_pMS->ClearMessages();
		m_pMS->Shutdown();
		m_pMS = nullptr;
	}

	if( m_pOF != nullptr )
	{
		m_pOF->ShutdownObjectFactory();
		m_pOF = nullptr;
	}

	for(unsigned int i = 0; i < m_cEnemies.size(); i++)
	{
		m_cEnemies[i]->Release();
		m_cEnemies[i] = nullptr;
	}
	m_cEnemies.clear();

	for(unsigned int i = 0; i < m_cNpcs.size(); i++)
	{
		m_cNpcs[i]->Release();
		m_cNpcs[i] = nullptr;
	}
	m_cNpcs.clear();

	for(unsigned int i = 0; i < m_cSpawn.size(); i++)
	{
		m_cSpawn[i]->Release();
		m_cSpawn[i] = nullptr;
	}
	m_cSpawn.clear();

	for(unsigned int i = 0; i < m_cBushes.size(); i++)
	{
		m_cBushes[i]->Release();
		m_cBushes[i] = nullptr;
	}
	m_cBushes.clear();

	m_cBoss1 = nullptr;
	m_cBoss2 = nullptr;

	if( m_cBuddy != nullptr )
	{
		m_cBuddy->Release();
	}
	m_cBuddy = nullptr;
	if( m_clevel != nullptr )
	{
		m_clevel->Release();
	}

	m_cBuddy = nullptr;
	m_clevel = nullptr;

	if( m_pOM != nullptr )
	{
		m_pOM->RemoveAllObjects();
		ObjectManager::DeleteInstance();
		m_pOM = nullptr;
	}


	m_pVM = nullptr;
	m_pPM = nullptr;
	m_pDI = nullptr;
	m_pAM = nullptr;
	m_pOF = nullptr;
	m_pOM = nullptr;
	m_pES = nullptr;

	delete m_pHUD;
	m_pHUD = nullptr;

	m_cPlayer = nullptr;

	loadedLevel = -1;
}

bool GamePlayState::Input() 
{
	if( ( m_pDI->KeyPressed(DIK_ESCAPE) || m_pDI->JoystickButtonPressed(7,0) )  )
		CGame::GetInstance()->ChangeState(PauseMenuState::GetInstance());

	if( m_pDI->KeyDown(DIK_LALT) || m_pDI->KeyDown(DIK_RALT) )
	{
		if(m_pDI->KeyDown(DIK_TAB) )
			CGame::GetInstance()->ChangeState(PauseMenuState::GetInstance());
	}

	return true;
}

void GamePlayState::Update(float fElapsedTime) 
{
	//m_clevel.Update(fElapsedTime);
	m_pVM->SetAmbientLight( .0f, .0f, .0f);

	m_cPlayer->SetTimePlayed(m_cPlayer->GetTimePlayed() + fElapsedTime);

	m_pOM->UpdateAllObjects(fElapsedTime);
	m_pOM->CheckCollisions();
	if(m_cPlayer->IsOn() && m_cPlayer->GetLightType() < 2)
		m_pOM->CheckTriangleCollisions();
	camera.x = float(m_cPlayer->GetPosX() - (CGame::GetInstance()->GetScreenWidth()*0.5));
	camera.y = float(m_cPlayer->GetPosY() - (CGame::GetInstance()->GetScreenHeight()*0.5));
	FMOD_VECTOR tmp;
	tmp.x = m_cPlayer->GetPosX();
	tmp.y = m_cPlayer->GetPosY();
	tmp.z = 0;
	m_pAM->SetListenerPos(tmp);
	m_pAM->setSoundPos(soundID2,tmp);
	m_pES->ProcessEvents();
	m_pMS->ProcessMessages();
	RECT chocolateRain = { (long)camera.x, (long)camera.y, (long)camera.x + CGame::GetInstance()->GetScreenWidth(), (long)camera.y + CGame::GetInstance()->GetScreenHeight()};
	m_pPM->GetActiveEmitter(rainA)->SetRect(chocolateRain);

	bool check = false;
	for( unsigned int i = 0; i < m_cBushes.size(); i++)
	{
		m_cBushes[i]->Update(fElapsedTime);
		if( m_cBushes[i]->CheckCollision(GetPlayer() ) == true )
		{
			m_cBushes[i]->SetIsInBush( true );
			check = true;
		}
		else
		{
			m_cBushes[i]->SetIsInBush( false );
		}
	}

	if( check == true )
	{
		GetPlayer()->m_bIsHidden = true; 
	}
	else
	{
		GetPlayer()->m_bIsHidden = false; 
	}


	if( GetPlayer()->m_vpActiveQuests.size() > 0 && GetPlayer()->m_bHasMedicine == true && m_clevel->whichlevel == HOSPITAL)
	{
		m_pHUD->SetTarget((float)townX, (float)townY);
	}


	for( unsigned int i = 0; i < m_cSpawn.size(); i++)
	{
		m_cSpawn[i]->Update(fElapsedTime);
		if(m_cSpawn[i]->CanSpawn() == true)
		{
			m_cEnemies.push_back(nullptr);
			m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
			ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
			pEnemy->SetHeight(64);
			pEnemy->SetWidth(m_cSpawn[i]->GetWidth());
			pEnemy->SetImageID(-1);
			pEnemy->SetTarget(GetPlayer());
			pEnemy->SetPosX(m_cSpawn[i]->GetPosX()+(rand()%20-10));
			pEnemy->SetPosY(m_cSpawn[i]->GetPosY()+(rand()%20-10));
			pEnemy->SetHealth(50);
			pEnemy->SetAnimation(SpawnEnemyAniID);
			GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
			m_cSpawn[i]->SetSpawn( false );
		}
	}


	//m_pHUD->Input();
	m_pHUD->Update(fElapsedTime);
	m_pPM->Update(fElapsedTime);
	//// Quest 2 completion
	if(GetPlayer()->questCounter == 10 )
	{
		questFlag = true;
		if(m_pDI->KeyPressed(DIK_RETURN) || m_pDI->JoystickButtonPressed(1,0))

		{
			for(unsigned int i = 0; i < GetPlayer()->m_vpActiveQuests.size(); i++)
			{
				GetPlayer()->completedQuest++;
				GetPlayer()->m_vpFinishedQuests.push_back(GetPlayer()->m_vpActiveQuests[i]);
				GetPlayer()->m_vpActiveQuests.pop_back();
			}
			questFlag = false;
		}
	}
	// Total quest completion to win the game
	if(GetPlayer()->m_vpFinishedQuests.size() == 2)
	{
		questFlag = false;
		CGame::GetInstance()->scoreCatch = m_cPlayer->GetScore();
		CGame::GetInstance()->ChangeState(WinMenuState::GetInstance());
	}


	// Auto Lose
	if(m_pDI->KeyPressed(DIK_G) && winLose == true )
	{
		winLose = false;
	}
}

void GamePlayState::Render() 
{
	m_pVM->GetSprite()->Flush();
	//m_clevel.Render();


	m_pOM->RenderAllObjects();



	for( unsigned int i = 0; i < m_cBushes.size(); i++)
	{
		m_cBushes[i]->Render();
	}
	m_pPM->Render();

	//m_cPlayer->Render();
	//m_pPM->GetActiveEmitter(rainA)->Render();

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
			if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
				bullet->SetImageID( self->m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png"));
			else
				bullet->SetImageID( self->m_pVM->RegisterTexture("resource/graphics/bullet.png"));
			bullet->SetHeight(16);
			bullet->SetWidth(16);
			bullet->SetOwner(pOwner);
			bullet->SetPosX(pOwner->GetPosX());
			bullet->SetPosY(pOwner->GetPosY());
			bullet->SetStartPos(pOwner->GetPosX(), pOwner->GetPosY());

			// Smoke effects
			RECT temp = {(long)pOwner->GetPosX(),(long)pOwner->GetPosY()-5,(long)pOwner->GetPosX() + (long)16.0f,(long)16.0f + (long)pOwner->GetPosY()};

			/*bullet->SetSmokeID(Particle_Manager::GetInstance()->ActivateEmitter(self->smokeL));*/

			/*self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetRect(temp);
			self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetLoopin(false);*/

			if( pOwner->GetWeaponType() == WPN_SHOTGUN )
			{
				Bullet* bullet2 = (Bullet*)( self->m_pOF->CreateObject( _T("Bullet") ) );
				Bullet* bullet3 = (Bullet*)( self->m_pOF->CreateObject( _T("Bullet") ) );
				bullet2->SetImageID( self->m_pVM->RegisterTexture("resource/graphics/bullet.png"));
				bullet3->SetImageID( self->m_pVM->RegisterTexture("resource/graphics/bullet.png"));
				//bullet 2
				bullet2->SetHeight(16);
				bullet2->SetWidth(16);
				bullet2->SetOwner(pOwner);
				bullet2->SetPosX(pOwner->GetPosX());
				bullet2->SetPosY(pOwner->GetPosY());
				bullet2->SetStartPos(pOwner->GetPosX(), pOwner->GetPosY());
				//bullet 3
				bullet3->SetHeight(16);
				bullet3->SetWidth(16);
				bullet3->SetOwner(pOwner);
				bullet3->SetPosX(pOwner->GetPosX());
				bullet3->SetPosY(pOwner->GetPosY());
				bullet3->SetStartPos(pOwner->GetPosX(), pOwner->GetPosY());

				switch(pOwner->GetOwner()->GetDirection())
				{
				case DIRE_UP:
					{
						bullet->SetPosY(pOwner->GetPosY()-30);
						bullet2->SetPosY(bullet->GetPosY());
						bullet3->SetPosY(bullet->GetPosY());

						temp.top = (long)bullet->GetPosY();
						temp.bottom = (long)16.0f + (long)bullet->GetPosY();

						bullet->SetSpeedX(0);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-D3DX_PI/2);
						bullet2->SetSpeedX(50);
						bullet2->SetSpeedY(-500);
						bullet2->SetRotation(-D3DX_PI/2);
						bullet3->SetSpeedX(-50);
						bullet3->SetSpeedY(-500);
						bullet3->SetRotation(-D3DX_PI/2);
						break;
					}
				case DIRE_LEFT:
					{
						bullet->SetPosX((float)pOwner->GetOwner()->GetRect().left);
						bullet->SetPosY((float)bullet->GetPosY()+ 15);
						bullet2->SetPosX(bullet->GetPosX());
						bullet2->SetPosY(bullet->GetPosY());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						temp.left = (long)bullet->GetPosX();
						temp.right = (long)16.0f + temp.left;

						bullet->SetSpeedX(-500);
						bullet->SetSpeedY(0);
						bullet->SetRotation(D3DX_PI);
						bullet2->SetSpeedX(-500);
						bullet2->SetSpeedY(50);
						bullet2->SetRotation(D3DX_PI);
						bullet3->SetSpeedX(-500);
						bullet3->SetSpeedY(-50);
						bullet3->SetRotation(D3DX_PI);
						break;
					}
				case DIRE_RIGHT:
					{
						bullet->SetPosX((float)pOwner->GetPosX()+30);
						bullet->SetPosY((float)bullet->GetPosY()-15);
						bullet2->SetPosX(bullet->GetPosX());
						bullet2->SetPosY(bullet->GetPosY());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						temp.left = (long)bullet->GetPosX();
						temp.right = (long)16.0f + temp.left;

						bullet->SetSpeedX(500);
						bullet->SetSpeedY(0);
						bullet2->SetSpeedX(500);
						bullet2->SetSpeedY(50);
						bullet3->SetSpeedX(500);
						bullet3->SetSpeedY(-50);
						break;
					}
				case DIRE_DOWN:
					{
						bullet->SetPosY(pOwner->GetPosY()+20);
						bullet->SetPosX(pOwner->GetPosX());
						bullet2->SetPosY(bullet->GetPosY());
						bullet2->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());
						bullet3->SetPosX(bullet->GetPosX());

						temp.top = (long)bullet->GetPosY();
						temp.bottom = (long)16.0f + (long)bullet->GetPosY();

						bullet->SetSpeedX(0);
						bullet->SetSpeedY(500);
						bullet->SetRotation(D3DX_PI/2);
						bullet2->SetSpeedX(50);
						bullet2->SetSpeedY(500);
						bullet2->SetRotation(D3DX_PI/2);
						bullet3->SetSpeedX(-50);
						bullet3->SetSpeedY(500);
						bullet3->SetRotation(D3DX_PI/2);
						break;
					}
				case DIRE_UPRIGHT:
					{
						bullet->SetPosY(pOwner->GetPosY()-20);
						bullet->SetPosX(pOwner->GetPosX()-20);
						bullet2->SetPosY(bullet->GetPosY());
						bullet2->SetPosX(bullet->GetPosX());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						bullet->SetSpeedX(500);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-D3DX_PI/4);
						bullet2->SetSpeedX(500);
						bullet2->SetSpeedY(-550);
						bullet2->SetRotation(-D3DX_PI/4);
						bullet3->SetSpeedX(550);
						bullet3->SetSpeedY(-500);
						bullet3->SetRotation(-D3DX_PI/4);
						break;
					}
				case DIRE_UPLEFT:
					{
						bullet->SetPosY(pOwner->GetPosY()-20);
						bullet->SetPosX(pOwner->GetPosX());
						bullet2->SetPosY(bullet->GetPosY());
						bullet2->SetPosX(bullet->GetPosX());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						bullet->SetSpeedX(-500);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-3*D3DX_PI/4);
						bullet2->SetSpeedX(-500);
						bullet2->SetSpeedY(-550);
						bullet2->SetRotation(-3*D3DX_PI/4);
						bullet3->SetSpeedX(-550);
						bullet3->SetSpeedY(-500);
						bullet3->SetRotation(-3*D3DX_PI/4);
						break;
					}
				case DIRE_DOWNLEFT:
					{
						bullet->SetPosY(pOwner->GetPosY()+25);
						bullet->SetPosX(pOwner->GetPosX()+10);
						bullet2->SetPosY(bullet->GetPosY());
						bullet2->SetPosX(bullet->GetPosX());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						bullet->SetSpeedX(-500);
						bullet->SetSpeedY(500);
						bullet->SetRotation(3*D3DX_PI/4);
						bullet2->SetSpeedX(-500);
						bullet2->SetSpeedY(550);
						bullet2->SetRotation(3*D3DX_PI/4);
						bullet3->SetSpeedX(-550);
						bullet3->SetSpeedY(500);
						bullet3->SetRotation(3*D3DX_PI/4);
						break;
					}
				case DIRE_DOWNRIGHT:
					{
						bullet->SetPosY(pOwner->GetPosY()+20);
						bullet->SetPosX(pOwner->GetPosX());
						bullet2->SetPosY(bullet->GetPosY());
						bullet2->SetPosX(bullet->GetPosX());
						bullet3->SetPosX(bullet->GetPosX());
						bullet3->SetPosY(bullet->GetPosY());

						bullet->SetSpeedX(500);
						bullet->SetSpeedY(500);
						bullet->SetRotation(D3DX_PI/4);
						bullet2->SetSpeedX(500);
						bullet2->SetSpeedY(550);
						bullet2->SetRotation(D3DX_PI/4);
						bullet3->SetSpeedX(550);
						bullet3->SetSpeedY(500);
						bullet3->SetRotation(D3DX_PI/4);
						break;
					}
				}

				bullet->SetSmokeID(Particle_Manager::GetInstance()->ActivateEmitter(self->smokeL));

				self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetRect(temp);
				self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetLoopin(false);

				self->m_pOM->AddObject( bullet );
				self->m_pOM->AddObject( bullet2 );
				self->m_pOM->AddObject( bullet3 );
				bullet->Release();
				bullet2->Release();
				bullet3->Release();
				bullet = nullptr;
				bullet2 = nullptr;
				bullet3 = nullptr;
			}
			else
			{
				switch(pOwner->GetOwner()->GetDirection())
				{
				case DIRE_UP:
					{
						bullet->SetPosY(pOwner->GetPosY()-30);
						bullet->SetSpeedX(0);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-D3DX_PI/2);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
							bullet->SetSpeedY(-150);
						break;
					}
				case DIRE_LEFT:
					{
						if(pOwner->GetWeaponType() == WPN_RIFLE)
							bullet->SetPosY(bullet->GetPosY()+15);
						else
							bullet->SetPosY(bullet->GetPosY()+5);
						bullet->SetPosX((float)pOwner->GetOwner()->GetRect2().left);
						bullet->SetSpeedX(-500);
						bullet->SetRotation(D3DX_PI);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
							bullet->SetSpeedX(-150);
						bullet->SetSpeedY(0);
						break;
					}
				case DIRE_RIGHT:
					{
						if( pOwner->GetWeaponType() == WPN_RIFLE )
							bullet->SetPosY(pOwner->GetPosY()-15);
						else
							bullet->SetPosY(bullet->GetPosY()-25);
						bullet->SetPosX((float)pOwner->GetPosX()+30);
						bullet->SetSpeedX(500);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
							bullet->SetSpeedX(150);

						bullet->SetSpeedY(0);
						break;
					}
				case DIRE_DOWN:
					{
						bullet->SetPosY(pOwner->GetPosY()+20);
						bullet->SetPosX(pOwner->GetPosX()+5);
						bullet->SetSpeedX(0);
						bullet->SetSpeedY(500);
						bullet->SetRotation(D3DX_PI/2);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
							bullet->SetSpeedY(150);
						break;
					}
				case DIRE_UPRIGHT:
					{
						if( pOwner->GetWeaponType() == WPN_RIFLE )
						{
							bullet->SetPosY(pOwner->GetPosY()-35);
							bullet->SetPosX(pOwner->GetPosX());
						}
						else
						{
							bullet->SetPosY(pOwner->GetPosY()-35);
							bullet->SetPosX(pOwner->GetPosX());
						}

						bullet->SetSpeedX(500);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-D3DX_PI/4);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
						{
							bullet->SetSpeedX(150);
							bullet->SetSpeedY(-150);
						}
						break;
					}
				case DIRE_UPLEFT:
					{
						if( pOwner->GetWeaponType() == WPN_RIFLE )
						{
							bullet->SetPosY(pOwner->GetPosY()-35);
							bullet->SetPosX(pOwner->GetPosX());
						}
						else
						{
							bullet->SetPosY(pOwner->GetPosY()-35);
							bullet->SetPosX(pOwner->GetPosX());
						}
						bullet->SetSpeedX(-500);
						bullet->SetSpeedY(-500);
						bullet->SetRotation(-3*D3DX_PI/4);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
						{
							bullet->SetSpeedX(-150);
							bullet->SetSpeedY(-150);
						}
						break;
					}
				case DIRE_DOWNLEFT:
					{
						if( pOwner->GetWeaponType() == WPN_RIFLE )
						{
							bullet->SetPosY(pOwner->GetPosY()+20);
							bullet->SetPosX(pOwner->GetPosX()+15);
						}
						else
						{
							bullet->SetPosY(pOwner->GetPosY()+20);
							bullet->SetPosX(pOwner->GetPosX()+15);
						}
						bullet->SetSpeedX(-500);
						bullet->SetSpeedY(500);
						bullet->SetRotation(3*D3DX_PI/4);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
						{
							bullet->SetSpeedX(-150);
							bullet->SetSpeedY(150);
						}
						break;
					}
				case DIRE_DOWNRIGHT:
					{
						if( pOwner->GetWeaponType() == WPN_RIFLE )
						{
							bullet->SetPosY(pOwner->GetPosY()+20);
							bullet->SetPosX(pOwner->GetPosX());
						}
						else
						{
							bullet->SetPosY(pOwner->GetPosY()+20);
							bullet->SetPosX(pOwner->GetPosX());
						}

						bullet->SetSpeedX(500);
						bullet->SetSpeedY(500);
						bullet->SetRotation(D3DX_PI/4);
						if(pOwner->GetOwner()->GetCharacterType() == CHA_BOSS2)
						{
							bullet->SetSpeedX(150);
							bullet->SetSpeedY(150);
						}
						break;
					}
				}

				bullet->SetSmokeID(Particle_Manager::GetInstance()->ActivateEmitter(self->smokeL));

				self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetRect(temp);
				self->m_pPM->GetActiveEmitter(bullet->GetSmokeID())->SetLoopin(false);

				self->m_pOM->AddObject( bullet );
				bullet->Release();
				bullet = nullptr;
			}

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
			for( unsigned int i = 0; i < self->m_cEnemies.size(); i++ )
			{
				if( enemyc == self->m_cEnemies[i] )
				{
					vector<Enemy*>::iterator nth = self->m_cEnemies.begin() + i;
					self->m_cEnemies.erase(nth);
					break;
				}
			}
			for(unsigned int i = 0; i < self->GetPlayer()->m_vpActiveQuests.size();i++)
			{
				if(self->GetPlayer()->m_vpActiveQuests[i]->QuestID == 2)
					self->GetPlayer()->questCounter++;
			}		

			self->m_pOM->RemoveObject( enemyc );
			self->m_cPlayer->SetScore( self->m_cPlayer->GetScore() + 25 );
			break;
		}
	case MSG_DESTROY_ENEMYS:
		{
			ShootingAi* enemys = dynamic_cast<DestroyEnemyS*>(pMsg)->GetEnemyS();
			for( unsigned int i = 0; i < self->m_cEnemies.size(); i++ )
			{
				if( enemys == self->m_cEnemies[i] )
				{
					vector<Enemy*>::iterator nth = self->m_cEnemies.begin() + i;
					if(enemys->GetBossBool())
						--(self->m_cBoss1->enemies);
					self->m_cEnemies[i] = nullptr;
					self->m_cEnemies.erase(nth);
					break;
				}
				self->m_cPlayer->SetScore( self->m_cPlayer->GetScore() + 35 );
			}
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
	case MSG_DESTROY_ENEMY:
		{
			Enemy* pEnemy = dynamic_cast<DestroyEnemy*>(pMsg)->GetEnemy();
			self->m_pOM->RemoveObject( pEnemy );
			self->m_cPlayer->SetScore( self->m_cPlayer->GetScore() + 250 );
			break;
		}
	}
}

void GamePlayState::HandleEvent(Event* aPEvent)
{
	if(aPEvent->GetEventID() == "ForestToTown")///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		m_pPM->GetActiveEmitter(rainA)->SetLoopin(true);
		m_pPM->GetActiveEmitter(rainA)->age = 0;
		for( unsigned int i = 0; i < this->fireA.size(); i++)
		{
			Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
		}
		m_pVM->SetLightPos(0, 0, -1.0f);
		m_pVM->SetSpotLightPos(0, 0, -.85f);
		m_pVM->SetInnerCone(.95f);
		m_pVM->SetOuterCone(.9f);
		//Player* tmpPlayer = this->GetPlayer();
		//HUD* tmpHud = this->m_pHUD;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
		LoadingScreen* loading = LoadingScreen::GetInstance();
		loading->Render();
		ChangeLevel();

		Player* pPlayer = this->GetPlayer();
		//Weapon* pWeapon = nullptr;
		Level* pLevel = nullptr;

		Bush* pBush = nullptr;
		SpawnPoint* pSpawn = nullptr;
		PickUp* pPickUp = nullptr;

		if( pLevel == nullptr )
		{
			m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
			pLevel = m_clevel;
			pLevel->LoadLevel("town.xml");
			pLevel->whichlevel = TOWN;
			m_pOM->AddObject(pLevel);
		}
		loading->Update();
		loading->Render();


		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/pills.png"));
				pPickUp->SetPickUpType(MEDICINE);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(10);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				pNpc->SetQuest(9);
				pNpc->SetLabel(8);
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget((float)nth->x, (float)nth->y);
			}
			else if( _stricmp(nth->m_cType,"Hospital") == 0 )
			{
				hospitalX = nth->x;
				hospitalY = nth->y;
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(64);
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();

		}
		pLevel->SetCollision(tmp);

		if(loadedLevel == -1)
		{
			pPlayer->SetPosX(3131);
			pPlayer->SetPosY(374);
		}
		m_pOM->AddObject(pPlayer);

		loading->Reset();
		loading = nullptr;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);

	}
	if(aPEvent->GetEventID() == "HospitalToTown")//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		m_pPM->GetActiveEmitter(rainA)->SetLoopin(true);
		m_pPM->GetActiveEmitter(rainA)->age = 0;
		for( unsigned int i = 0; i < this->fireA.size(); i++)
		{
			Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
		}
		//Player* tmpPlayer = this->GetPlayer();
		//HUD* tmpHud = this->m_pHUD;
		m_pVM->SetLightPos(0, 0, -1.0f);
		m_pVM->SetSpotLightPos(0, 0, -.85f);
		m_pVM->SetInnerCone(.95f);
		m_pVM->SetOuterCone(.9f);
		m_pVM->SetAmbientLight( .1f, .1f, .0f);

		LoadingScreen* loading = LoadingScreen::GetInstance();
		loading->Render();
		ChangeLevel();

		Player* pPlayer = this->GetPlayer();
		//Weapon* pWeapon = nullptr;
		Level* pLevel = nullptr;

		Bush* pBush = nullptr;
		SpawnPoint* pSpawn = nullptr;
		PickUp* pPickUp = nullptr;

		if( pLevel == nullptr )
		{
			m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
			pLevel = m_clevel;
			pLevel->LoadLevel("town.xml");
			pLevel->whichlevel = TOWN;
			m_pOM->AddObject(pLevel);
		}
		loading->Update();
		loading->Render();


		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(_stricmp(nth->m_cType,"Closet") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				pBush->m_bCloset = true;
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/pills.png"));
				pPickUp->SetPickUpType(MEDICINE);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(10);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				if( pPlayer->m_bHasMedicine == false )
				{
					pNpc->SetQuest(9);
					pNpc->SetLabel(8);
				}
				else
				{
					pNpc->SetQuest(10);
					pNpc->SetLabel(9);
				}
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				int x = pPlayer->m_vpActiveQuests.size();
				if( pPlayer->m_bHasMedicine == true  && x > 0 )
				{
					m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				}
				else if( pPlayer->m_bHasMedicine == false  && pPlayer->m_vpActiveQuests.size() > 0 )
				{
					m_pHUD->SetTarget((float)hospitalX, (float)hospitalY);
				}
				else if( pPlayer->m_bHasMedicine == false && pPlayer->m_vpActiveQuests.size() == 0 )
				{
					m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				}

				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget((float)nth->x, (float)nth->y);
			}
			else if( _stricmp(nth->m_cType,"Hospital") == 0 )
			{
				//m_pHUD->SetTarget(nth->x, nth->y);
				hospitalX = nth->x;
				hospitalY = nth->y;
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(m_cSpawn[m_cSpawn.size()-1]->GetHeight());
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();

		}
		pLevel->SetCollision(tmp);

		pPlayer->SetPosX(739);
		pPlayer->SetPosY(4166);

		m_pOM->AddObject(pPlayer);

		loading->Reset();
		loading = nullptr;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
	}
	if(aPEvent->GetEventID() == "HouseToTown")////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		m_pPM->GetActiveEmitter(rainA)->SetLoopin(true);
		m_pPM->GetActiveEmitter(rainA)->age = 0;
		for( unsigned int i = 0; i < this->fireA.size(); i++)
		{
			Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
		}
		m_pVM->SetLightPos(0, 0, -1.0f);
		m_pVM->SetSpotLightPos(0, 0, -.85f);
		m_pVM->SetInnerCone(.95f);
		m_pVM->SetOuterCone(.9f);
		//Player* tmpPlayer = this->GetPlayer();
		//HUD* tmpHud = this->m_pHUD;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);

		LoadingScreen* loading = LoadingScreen::GetInstance();
		loading->Render();
		ChangeLevel();

		Player* pPlayer = this->GetPlayer();
		//Weapon* pWeapon = nullptr;
		Level* pLevel = nullptr;

		Bush* pBush = nullptr;
		SpawnPoint* pSpawn = nullptr;
		PickUp* pPickUp = nullptr;

		if( pLevel == nullptr )
		{
			m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
			pLevel = m_clevel;
			pLevel->LoadLevel("town.xml");
			pLevel->whichlevel = TOWN;
			m_pOM->AddObject(pLevel);
		}
		loading->Update();
		loading->Render();


		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(_stricmp(nth->m_cType,"Closet") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				pBush->m_bCloset = true;
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/pills.png"));
				pPickUp->SetPickUpType(MEDICINE);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(10);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				if( pPlayer->m_bHasMedicine == false )
				{
					pNpc->SetQuest(9);
					pNpc->SetLabel(8);
				}
				else
				{
					pNpc->SetQuest(10);
					pNpc->SetLabel(9);
				}
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				int x = pPlayer->m_vpActiveQuests.size();
				if( pPlayer->m_bHasMedicine == true  && x > 0 )
				{
					m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				}
				else if( pPlayer->m_bHasMedicine == false  && pPlayer->m_vpActiveQuests.size() > 0 )
				{
					m_pHUD->SetTarget((float)hospitalX, (float)hospitalY);
				}
				else if( pPlayer->m_bHasMedicine == false && pPlayer->m_vpActiveQuests.size() == 0 )
				{
					m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				}

				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget((float)nth->x, (float)nth->y);
			}
			else if( _stricmp(nth->m_cType,"Hospital") == 0 )
			{
				//m_pHUD->SetTarget(nth->x, nth->y);
				hospitalX = nth->x;
				hospitalY = nth->y;
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(m_cSpawn[m_cSpawn.size()-1]->GetHeight());
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();

		}
		pLevel->SetCollision(tmp);

		pPlayer->SetPosX(651);
		pPlayer->SetPosY(1704);

		m_pOM->AddObject(pPlayer);

		loading->Reset();
		loading = nullptr;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
	}
	if(aPEvent->GetEventID() == "GoToHouse")
	{
		m_pPM->GetActiveEmitter(rainA)->SetLoopin(false);

		for( unsigned int i = 0; i < this->fireA.size(); i++)
		{
			Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
		}
		//Player* tmpPlayer = this->GetPlayer();
		//HUD* tmpHud = this->m_pHUD;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
		LoadingScreen* loading = LoadingScreen::GetInstance();
		loading->Render();
		ChangeLevel();

		Player* pPlayer = this->GetPlayer();
		//Weapon* pWeapon = nullptr;
		Level* pLevel = nullptr;

		Bush* pBush = nullptr;
		SpawnPoint* pSpawn = nullptr;
		PickUp* pPickUp = nullptr;

		if( pLevel == nullptr )
		{
			m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
			pLevel = m_clevel;
			pLevel->LoadLevel("house.xml");
			pLevel->whichlevel = HOSPITAL;
			m_pOM->AddObject(pLevel);
			pLevel->SetInside(true);
		}
		loading->Update();
		loading->Render();

		pLevel->SetInside(true);
		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(_stricmp(nth->m_cType,"Closet") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				pBush->m_bCloset = true;
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				if( GetPlayer()->m_vpActiveQuests.size() > 0 && GetPlayer()->m_bHasMedicine == false )
				{
					pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
					pPickUp->SetPosX((float)nth->x);
					pPickUp->SetPosY((float)nth->y);
					pPickUp->SetWidth(nth->width);
					pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/pills.png"));
					pPickUp->SetHeight(nth->height);
					pPickUp->SetPickUpType(MEDICINE);
					m_pOM->AddObject(pPickUp);
					pPickUp = nullptr;
				}
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(10);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				pNpc->SetQuest(9);
				pNpc->SetLabel(8);
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget((float)nth->x, (float)nth->y);
				townX = nth->x;
				townY = nth->y;
				if( GetPlayer()->m_vpActiveQuests.size() == 0 && GetPlayer()->m_bHasMedicine == false )
				{
					m_pHUD->SetTarget((float)nth->x, (float)nth->y);
				}
			}
			else if( _stricmp(nth->m_cType,"Hospital") == 0 )
			{
				hospitalX = nth->x;
				hospitalY = nth->y;
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(64);
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();

		}
		pLevel->SetCollision(tmp);

		if(loadedLevel == -1)
		{
			pPlayer->SetPosX(500);
			pPlayer->SetPosY(300);
		}

		m_pOM->AddObject(pPlayer);

		loading->Reset();
		loading = nullptr;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
	}
	if(aPEvent->GetEventID() == "GoToHospital")////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		m_pPM->GetActiveEmitter(rainA)->SetLoopin(false);

		for( unsigned int i = 0; i < this->fireA.size(); i++)
		{
			Particle_Manager::GetInstance()->GetActiveEmitter(fireA[i])->SetLoopin(false);
		}
		//Player* tmpPlayer = this->GetPlayer();
		//HUD* tmpHud = this->m_pHUD;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
		LoadingScreen* loading = LoadingScreen::GetInstance();
		loading->Render();
		ChangeLevel();

		Player* pPlayer = this->GetPlayer();
		//Weapon* pWeapon = nullptr;
		Level* pLevel = nullptr;

		Bush* pBush = nullptr;
		SpawnPoint* pSpawn = nullptr;
		PickUp* pPickUp = nullptr;

		if( pLevel == nullptr )
		{
			m_clevel = (Level*)m_pOF->CreateObject( _T("Level"));
			pLevel = m_clevel;
			pLevel->LoadLevel("hospital.xml");
			pLevel->whichlevel = HOSPITAL;
			m_pOM->AddObject(pLevel);
			pLevel->SetInside(true);
		}
		loading->Update();
		loading->Render();

		pLevel->SetInside(true);
		vector<leveldata> tmp = pLevel->GetCollision();
		for(unsigned int i = 0; i < tmp.size(); i++) 
		{
			vector<leveldata>::iterator nth = tmp.begin() + i;

			if( _stricmp(nth->m_cType,"Bush") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(_stricmp(nth->m_cType,"Closet") == 0 )
			{
				m_cBushes.push_back(nullptr);
				m_cBushes[m_cBushes.size()-1] = (Bush*)m_pOF->CreateObject( _T("Bush") );
				pBush = m_cBushes[m_cBushes.size()-1];
				pBush->SetPosX((float)nth->x);
				pBush->SetPosY((float)nth->y);
				pBush->SetWidth(nth->width);
				pBush->SetHeight(nth->height);
				pBush->SetImageID(bush);
				pBush->m_bCloset = true;
				m_pOM->AddObject(pBush);
				pBush = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shotgun Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(SHOTGUN_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if(  _stricmp(nth->m_cType,"Fire") == 0)
			{
				int tmp1 = m_pPM->ActivateEmitter(fire1L);
				m_pPM->GetActiveEmitter(tmp1)->SetRect(tmp[i].m_rCollision);
				int tmp2 = m_pPM->ActivateEmitter(fire2L);
				m_pPM->GetActiveEmitter(tmp2)->SetRect(tmp[i].m_rCollision);
				int tmp3 = m_pPM->ActivateEmitter(fire3L);
				m_pPM->GetActiveEmitter(tmp3)->SetRect(tmp[i].m_rCollision);
				fireA.push_back(tmp3);
				fireA.push_back(tmp2);
				fireA.push_back(tmp1);
				m_pPM->GetActiveEmitter(tmp1)->SetSoundID( m_pAM->RegisterSound("resource/Sounds/fire.wav"));
			}
			else if(  _stricmp(nth->m_cType,"Streetlight") == 0)
			{
				streetLights.push_back(tmp[i].m_rCollision);
			}
			else if( _stricmp(nth->m_cType,"Rifle Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(RIFLE_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Pistol Ammo") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/ammo.png"));
				pPickUp->SetPickUpType(PISTOL_AMMO);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Medicine") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(MEDICINE);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Health") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/health.png"));
				pPickUp->SetPickUpType(HEALTH);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Battery") == 0)
			{
				pPickUp = (PickUp*)m_pOF->CreateObject( _T("PickUp"));
				pPickUp->SetPosX((float)nth->x);
				pPickUp->SetPosY((float)nth->y);
				pPickUp->SetWidth(nth->width);
				pPickUp->SetHeight(nth->height);
				pPickUp->SetImageID(m_pVM->RegisterTexture("resource/graphics/battery.png"));
				pPickUp->SetPickUpType(BATTERY);
				m_pOM->AddObject(pPickUp);
				pPickUp = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Shooting Enemy") == 0)
			{
				m_cEnemies.push_back(nullptr);
				m_cEnemies[m_cEnemies.size()-1] = (ShootingAi*)m_pOF->CreateObject( _T("ShootingAi") );
				ShootingAi* pEnemy = (ShootingAi*)(m_cEnemies[m_cEnemies.size()-1]);
				pEnemy->SetHeight( nth->height);
				pEnemy->SetWidth( nth->width);
				pEnemy->SetImageID(-1);
				pEnemy->SetTarget(m_cPlayer);
				pEnemy->SetPosX((float)nth->x);
				pEnemy->SetPosY((float)nth->y);
				pEnemy->SetHealth(50);
				pEnemy->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				m_pOM->AddObject(pEnemy);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pEnemy);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pEnemy->GetPosX()+pEnemy->GetWidth()/2);
				eWeapon->SetPosY(pEnemy->GetPosY());
				eWeapon->SetDamage(10);
				pEnemy->SetWeapon(eWeapon);
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"NPC") == 0)
			{
				m_cNpcs.push_back(nullptr);
				m_cNpcs[m_cNpcs.size()-1] = (NPC*)m_pOF->CreateObject( _T("NPC") );
				NPC* pNpc =(NPC*)(m_cNpcs[m_cNpcs.size()-1]);
				pNpc->SetHeight(32);
				pNpc->SetWidth(32);
				pNpc->SetImageID(-1);
				pNpc->SetPosX((float)nth->x);
				pNpc->SetPosY((float)nth->y);
				pNpc->SetQuest(9);
				pNpc->SetLabel(8);
				pNpc->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Npc.xml"));
				m_pHUD->SetTarget(pNpc->GetPosX(), pNpc->GetPosY());
				m_pOM->AddObject(pNpc);
				pNpc = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if( _stricmp(nth->m_cType,"Town") == 0)
			{
				m_pHUD->SetTarget((float)nth->x, (float)nth->y);
				townX = nth->x;
				townY = nth->y;
				if( GetPlayer()->m_vpActiveQuests.size() == 0 && GetPlayer()->m_bHasMedicine == false )
				{
					m_pHUD->SetTarget((float)nth->x, (float)nth->y);
				}
			}
			else if( _stricmp(nth->m_cType,"Hospital") == 0 )
			{
				hospitalX = nth->x;
				hospitalY = nth->y;
			}
			else if ( _stricmp(nth->m_cType,"Spawn Point") == 0 )
			{
				m_cSpawn.push_back(nullptr);
				m_cSpawn[m_cSpawn.size()-1] = (SpawnPoint*)m_pOF->CreateObject( _T("SpawnPoint") );
				pSpawn = m_cSpawn[m_cSpawn.size()-1];
				pSpawn->SetPosX( (float)nth->x );
				pSpawn->SetPosY( (float)nth->y );
				pSpawn->SetHeight( nth->height );
				pSpawn->SetWidth( nth->width );
				pSpawn->SetImageID( -1 );
				//m_pOM->AddObject(pSpawn);
				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
				for( int i = 0; i < 1; i++)
				{
					m_cEnemies.push_back(nullptr);
					m_cEnemies[m_cEnemies.size()-1] = (ChasingAI*)GamePlayState::GetInstance()->m_pOF->CreateObject( _T("ChasingAI") );
					ChasingAI* pEnemy = (ChasingAI*)(m_cEnemies[m_cEnemies.size()-1]);
					pEnemy->SetHeight(64);
					pEnemy->SetWidth(m_cSpawn[m_cSpawn.size()-1]->GetWidth());
					pEnemy->SetImageID(-1);
					pEnemy->SetTarget(GetPlayer());
					pEnemy->SetPosX((float)m_cSpawn[m_cSpawn.size()-1]->GetPosX()/*+(rand()%20-10)*/);
					pEnemy->SetPosY((float)m_cSpawn[m_cSpawn.size()-1]->GetPosY()/*+(rand()%20-10)*/);
					pEnemy->SetHealth(50);
					pEnemy->SetAnimation(SpawnEnemyAniID);
					GamePlayState::GetInstance()->m_pOM->AddObject(pEnemy);
					m_cSpawn[m_cSpawn.size()-1]->SetSpawn( false );
				}
			} 
			else if ( _stricmp(nth->m_cType,"Boss1") == 0 )
			{
				m_cBoss1 = (Boss1*)m_pOF->CreateObject( _T("Boss1") );

				Boss1* pBoss = (Boss1*)m_cBoss1;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/BanditAnimations.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);
				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				eWeapon->SetImageID(-1);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_SHOTGUN, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			else if ( _stricmp(nth->m_cType,"Boss2") == 0 )
			{
				m_cBoss2 = (Boss2*)m_pOF->CreateObject( _T("Boss2") );
				Boss2* pBoss = (Boss2*)m_cBoss2;
				pBoss->SetHealth(100);
				pBoss->SetHeight(nth->height);
				pBoss->SetWidth(nth->width);
				pBoss->SetPosX( (float)nth->x );
				pBoss->SetPosY( (float)nth->y );
				pBoss->SetAnimation(m_pVM->RegisterAnimation("resource/graphics/Zombies.xml"));
				pBoss->SetTarget(pPlayer);
				m_pOM->AddObject(pBoss);
				m_cEnemies.push_back(pBoss);

				Weapon* eWeapon = (Weapon*)m_pOF->CreateObject( _T("Weapon"));
				eWeapon->SetHeight(20);
				eWeapon->SetWidth(10);
				int tmpID = m_pVM->RegisterTexture("resource/graphics/Cinder_Block.png" );
				eWeapon->SetImageID(tmpID);
				eWeapon->SetOwner(pBoss);
				eWeapon->Init(WPN_PISTOL, 100, 0);
				eWeapon->SetPosX(pBoss->GetPosX()+pBoss->GetWidth()/2);
				pBoss->SetWeapon(eWeapon);

				pSpawn = nullptr;
				tmp.erase(nth);
				i--;
			}
			loading->Update();
			loading->Render();

		}
		pLevel->SetCollision(tmp);

		if(loadedLevel == -1)
		{
			pPlayer->SetPosX(500);
			pPlayer->SetPosY(300);
		}

		m_pOM->AddObject(pPlayer);

		loading->Reset();
		loading = nullptr;
		m_pVM->SetAmbientLight( .1f, .1f, .0f);
	}


}

void GamePlayState::ChangeLevel()
{
	m_pVM->SetAmbientLight( .1f, .1f, .0f);
	for(unsigned int i = 0; i < m_cEnemies.size(); i++)
	{
		//m_cEnemies[i]->Release();
		m_cEnemies[i] = nullptr;
	}
	m_cEnemies.clear();

	for(unsigned int i = 0; i < m_cNpcs.size(); i++)
	{
		m_cNpcs[i]->Release();
		m_cNpcs[i] = nullptr;
	}
	m_cNpcs.clear();

	for(unsigned int i = 0; i < m_cSpawn.size(); i++)
	{
		m_cSpawn[i]->Release();
		m_cSpawn[i] = nullptr;
	}
	m_cSpawn.clear();

	for(unsigned int i = 0; i < m_cBushes.size(); i++)
	{
		m_cBushes[i]->Release();
		m_cBushes[i] = nullptr;
	}
	m_cBushes.clear();


	if( m_pOM != nullptr )
	{
		m_pOM->RemoveAllObjects();
	}

	m_cBoss1 = nullptr;
	m_cBoss2 = nullptr;
	if( m_cBuddy != nullptr )
	{
		m_cBuddy->Release();
	}
	m_cBuddy = nullptr;
	if( m_clevel != nullptr )
	{
		m_clevel->Release();
	}
	m_clevel = nullptr;

	fireA.clear();
	streetLights.clear();
	m_cNpcs.clear();


}