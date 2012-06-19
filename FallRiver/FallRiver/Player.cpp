#include "Player.h"
#include "Quests.h"
#include "GamePlayState.h"
#include "Enemy.h"
#include "ViewManager.h"
#include "DirectInput.h"
#include "Weapon.h"
#include "Light.h"

Player::Player()
{
	m_nCharacterType = CHA_PLAYER;
	m_bIsAlive = true;
	m_bIsHidden = false;
	SetDirection(DIRE_UP);
}

Player::~Player()
{

}

void Player::Update(float aFElapsedTime) 
{
	DirectInput* pDI = DirectInput::GetInstance();

	if( pDI->KeyDown(DIK_RIGHT))
	{
		if( pDI->KeyDown(DIK_UP))
			SetDirection(DIRE_UPRIGHT);
		else if(pDI->KeyDown(DIK_DOWN))
			SetDirection(DIRE_DOWNRIGHT);
		else
			SetDirection(DIRE_RIGHT);		

	}
	else if( pDI->KeyDown(DIK_LEFT))
	{
		if( pDI->KeyDown(DIK_UP))
			SetDirection(DIRE_UPLEFT);
		else if(pDI->KeyDown(DIK_DOWN))
			SetDirection(DIRE_DOWNLEFT);
		else
			SetDirection(DIRE_LEFT);		
	}
	else if( pDI->KeyDown(DIK_UP))
		SetDirection(DIRE_UP);
	else if( pDI->KeyDown(DIK_DOWN))
		SetDirection(DIRE_DOWN);
}

void Player::Render()
{
	ViewManager* pVM = ViewManager::GetInstance();

	pVM->DrawRect(GetRect(), 0, 0, 255);
}

bool Player::CheckCollision(IObjects* pBase) 
{
	if(BaseObject::CheckCollision(pBase) == true )
	{
		if(pBase->GetObjectType() == OBJ_CHARACTER)
		{
			BaseCharacter* pCh = (BaseCharacter*)pBase;
			//if(pCh->GetCharacterType() == CHA_ENEMY)
			{
				if( GetRect().right <= pBase->GetRect().left + 5 )
					GamePlayState::GetInstance()->SetCanMoveRight(false);
				else if( GetRect().left >= pBase->GetRect().right - 5 )
					GamePlayState::GetInstance()->SetCanMoveLeft(false);
				else if( GetRect().top >= pBase->GetRect().bottom -5 )
					GamePlayState::GetInstance()->SetCanMoveUp(false);
				else if( GetRect().bottom <= pBase->GetRect().top + 5 )
					GamePlayState::GetInstance()->SetCanMoveDown(false);
			}
		}
		return true;
	}
	GamePlayState::GetInstance()->SetCanMoveDown(true);
	GamePlayState::GetInstance()->SetCanMoveUp(true);
	GamePlayState::GetInstance()->SetCanMoveRight(true);
	GamePlayState::GetInstance()->SetCanMoveLeft(true);
	return false;
}

// Check if the player is still alive
bool Player::CheckLifes() {return m_bIsAlive;}

void Player::AddQuest(Quests* pQuest)
{
	// Just in case
	if(pQuest == nullptr)
		return;

	// Add the quest
	m_vpActiveQuests.push_back(pQuest);
}

void Player::AddWeapon(Weapon* pWeapon)
{
	// Just in case
	if(pWeapon == nullptr)
		return;

	// Add the weapon and make it current
	m_vpWeapons.push_back(pWeapon);
	m_currWeapon = pWeapon;
}

void Player::AddLight(Light* pLight) 
{
	// Just in case
	if(pLight == nullptr)
		return;

	// Add the weapon and make it current
	m_vpLights.push_back(pLight);
	m_currLight = pLight;
}

