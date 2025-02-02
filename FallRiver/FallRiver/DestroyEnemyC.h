#ifndef __DestroyEnemyC_h__
#define __DestroyEnemyC_h__

#include "Message.h"

class ChasingAI;

class DestroyEnemyC: public Message
{
private: 
	ChasingAI* m_pEnemy;
public:
	DestroyEnemyC(ChasingAI* aEnemyC);
	ChasingAI* GetEnemyC(void) { return m_pEnemy; }
	virtual ~DestroyEnemyC();
};

#endif
