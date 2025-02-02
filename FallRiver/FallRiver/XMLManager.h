#include <Windows.h>
#include <string>
#include <vector>
using namespace std;

#ifndef __XMLManager_h__
#define __XMLManager_h__

class Emitter;
class Level;
class Font;
class Kerning;
class Animation;
class Player;

#include "tinystr.h"
#include "tinyxml.h"

struct tHighscore
{
	char	szName[ 11 ];
	int		nScore;
};


class XMLManager
{

public:
	static XMLManager* GetInstance(void);
	Emitter* ParseEmitter(string aFile);

	// For Saving and Loading the Players Progress
	void SaveProgress( const char* szFilename, Player &player);
	bool CheckSlot( const char* szFilename );
	Player* LoadProgress(const char* szFilename);

	void GeneratePlayers( vector< tHighscore >& vHighscore );
	bool LoadHighScores( const char* szFilename, vector< tHighscore >& vHighscore );
	void SaveHighScores( const char* szFilename, const vector<tHighscore>& vHighscore);

	bool LoadSettings( const char* szFilename, vector< int >& vSettings );
	void SaveSettings( const char* szFilename, const vector<int>& vSettings);

	// Loading Fonts
	bool LoadFont( char* szFilename,  vector<Font>&fonts, vector<Kerning>&kerns);
};

#endif
