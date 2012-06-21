#ifndef __Boss1_h__
#define __Boss1_h__

#include "Enemy.h"
#include "IListener.h"

class Boss1: public Enemy
{
	bool m_bIsChasing;
	bool m_bIsMoving;


	BaseObject* m_cInTheWay;

public: 
	Boss1();
	~Boss1();

	virtual void Update(float fElapsedTime);

	virtual void Render();

	virtual bool CheckCollision(IObjects* pBase);

	virtual void HandleEvent(Event* pEvent);
};

#endif
