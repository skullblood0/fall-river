#include <Windows.h>

#include "Boss2.h"
#include "ObjectFactory.h"
#include "BaseCharacter.h"
#include "Enemy.h"

void Boss2::Update(float Time) 
{
	BaseCharacter::Update(Time);
}

void Boss2::Render() 
{
	
}

bool Boss2::CheckCollision(IObjects* pBase) 
{
	if( BaseCharacter::CheckCollision(pBase) == false)
		return false;

	return true;
}

void Boss2::HandleEvent(Event* Event) 
{
}

