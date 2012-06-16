#include <Windows.h>
#include <vector>
using namespace std;

#ifndef __Level_h__
#define __Level_h__

// #include "Terrain.h"
// #include "GamePlayState.h"
// #include "PickUp.h"

//class Terrain;
//class GamePlayState;
//class PickUp;
//class Level;

struct leveldata
{
	RECT m_rCollision;
	int height, width, x,y;
	char m_cType[ 32 ];
};



class Level
{
private:
	vector<leveldata> m_vCollisions;
	int m_nBackgroundID;
	float m_nPosX;
	float m_nPosY;
	TCHAR buffer[100];

public: 
	Level();

	~Level();

	//void Update(float time);
	bool LoadLevel( const char* szFilename );

	void Update(float fElapsed);
	void Render();
	static Level* GetInstance();

	void CheckCollision();
	int GetBGID() { return m_nBackgroundID; }
	void SetBGID(int x ) { m_nBackgroundID = x; }
};

#endif
