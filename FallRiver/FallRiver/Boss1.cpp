#include "Boss1.h"
#include "ObjectFactory.h"
#include "BaseCharacter.h"
#include "MessageSystem.h"
#include "DestroyEnemyC.h"
#include "Enemy.h"

Boss1::Boss1()
{

}

Boss1::~Boss1()
{

}

void Boss1::Update(float fElapsedTime)
{
	Enemy::Update(fElapsedTime);


	Enemy::Update(fElapsedTime);

	float distance = ( m_pTarget->GetPosX() + m_pTarget->GetPosY() ) - ( GetPosX() + GetPosY() );

	if( distance < 0)
		distance = -distance;

	if( distance < 200 && distance > 2 && CanMove() )
	{
		m_bIsChasing = true;
		MoveTo(m_pTarget->GetPosX(), m_pTarget->GetPosY(), 80 );
		BaseCharacter::Update(fElapsedTime);
	}
	else if( m_bIsChasing && m_cInTheWay )
	{
		if( m_cInTheWay->GetObjectType() == OBJ_CHARACTER )
		{
			BaseCharacter* pCH = (BaseCharacter*)m_cInTheWay;

			if(pCH->GetCharacterType() == CHA_ENEMY)
			{
				Enemy* pEN = (Enemy*)m_cInTheWay;

				float ourDist = (GetPosX() + GetPosY()) - (m_pTarget->GetPosX() + m_pTarget->GetPosY());
				float theirDist = (pEN->GetPosX() + pEN->GetPosY()) - (m_pTarget->GetPosX() + m_pTarget->GetPosY());

				if(ourDist < 0)
					ourDist = -ourDist;
				if(theirDist < 0)
					theirDist = -theirDist;
				
				if( ourDist < theirDist )
				{
					pEN->SetCanMove(false);
					SetCanMove(true);
					MoveTo(m_pTarget->GetPosX(), m_pTarget->GetPosY(), 80);
					BaseCharacter::Update(fElapsedTime);
				}
				else
				{
					pEN->SetCanMove(true);
					SetCanMove(false);
					pEN->MoveTo(m_pTarget->GetPosX(), m_pTarget->GetPosY(), 80);
					pEN->BaseCharacter::Update(fElapsedTime);
				}
			}
			else
			{
				if(m_pTarget->GetPosY() > m_cInTheWay->GetPosY() && m_pTarget->GetPosX() > GetPosX() && GetPosY() < m_cInTheWay->GetRect().bottom)
				{
					float DistToBottom = GetPosY() - m_cInTheWay->GetPosY();
					float DistToRight = GetPosX() - m_cInTheWay->GetPosX();

					if(DistToBottom < 0)
						DistToBottom = -DistToBottom;

					if(DistToBottom > DistToRight)
					{
						SetCanMove(true);
						MoveTo(GetPosX(), (float)m_cInTheWay->GetRect().bottom, 80);
						BaseCharacter::Update(fElapsedTime);
					}
					else if(m_pTarget->GetPosX() > GetPosX() && m_pTarget->GetPosX() > m_cInTheWay->GetPosX())
					{
						SetCanMove(true);
						MoveTo((float)m_cInTheWay->GetRect().right, GetPosY(), 80);
						BaseCharacter::Update(fElapsedTime);
					}
				}
				else if(m_pTarget->GetPosY() > m_cInTheWay->GetPosY() && m_pTarget->GetPosX() < GetPosX() && GetPosY() < m_cInTheWay->GetRect().bottom)
				{
					float DistToBottom = GetPosY() - m_cInTheWay->GetPosY();
					float DistToLeft = m_cInTheWay->GetPosX() - GetPosX();

					if(DistToBottom < 0)
						DistToBottom = -DistToBottom;

					if(DistToBottom > DistToLeft)
					{
						SetCanMove(true);
						MoveTo(GetPosX(), (float)m_cInTheWay->GetRect().bottom, 80);
						BaseCharacter::Update(fElapsedTime);
					}
					else if(m_pTarget->GetPosX() < GetPosX() && m_pTarget->GetPosX() < m_cInTheWay->GetPosX())
					{
						SetCanMove(true);
						MoveTo((float)m_cInTheWay->GetRect().left-GetWidth(), GetPosY(), 80);
						BaseCharacter::Update(fElapsedTime);
					}
				}
				else if(m_pTarget->GetPosY() < m_cInTheWay->GetPosY() && m_pTarget->GetPosX() > GetPosX() && GetPosY() < m_cInTheWay->GetRect().top)
				{
					float DistToTop = GetPosY() - m_cInTheWay->GetPosY();
					float DistToRight = m_cInTheWay->GetPosX() - GetPosX();

					if(DistToTop < 0)
						DistToTop = -DistToTop;

					if(DistToTop < DistToRight)
					{
						SetCanMove(true);
						MoveTo(GetPosX(), (float)m_cInTheWay->GetRect().top-GetHeight(), 80);
						BaseCharacter::Update(fElapsedTime);
					}
					else if(m_pTarget->GetPosX() > GetPosX() && m_pTarget->GetPosX() > m_cInTheWay->GetPosX())
					{
						SetCanMove(true);
						MoveTo((float)m_cInTheWay->GetRect().right, GetPosY(), 80);
						BaseCharacter::Update(fElapsedTime);
					}

				}
				else if(m_pTarget->GetPosY() < m_cInTheWay->GetPosY() && m_pTarget->GetPosX() < GetPosX() && GetPosY() < m_cInTheWay->GetRect().top)
				{
					float DistToTop = GetPosY() - m_cInTheWay->GetPosY();
					float DistToLeft = GetPosX() - m_cInTheWay->GetPosX();

					if(DistToTop < 0)
						DistToTop = -DistToTop;

					if(DistToTop < DistToLeft)
					{
						SetCanMove(true);
						MoveTo(GetPosX(), (float)m_cInTheWay->GetRect().top-GetHeight(), 80);
						BaseCharacter::Update(fElapsedTime);
					}
					else if(m_pTarget->GetPosX() < GetPosX() && m_pTarget->GetPosX() < m_cInTheWay->GetPosX())
					{
						SetCanMove(true);
						MoveTo((float)m_cInTheWay->GetRect().left-GetWidth(), GetPosY(), 80);
						BaseCharacter::Update(fElapsedTime);
					}
				}
			}
		}
	}
}

void Boss1::Render()
{

}

bool Boss1::CheckCollision(IObjects* pBase)
{
	if( BaseCharacter::CheckCollision(pBase) == false )
		return false;

	return true;
}

void Boss1::HandleEvent(Event* pEvent)
{

}




