#ifndef TOWERS_MOVE_H
#define TOWERS_MOVE_H

#ifdef CLIENT_DLL
class C_SDKPlayer;
#endif

#include "towers/BaseClickable.h"

#ifdef GAME_DLL
class CTowersMove : public CPointEntity
{
public:
	DECLARE_CLASS( CTowersMove , CPointEntity );
	DECLARE_DATADESC();

	// Constructor
	CTowersMove ( void ) {}


	int				m_iPos;	// Count at which to fire our output
};

extern CUtlVector< CTowersMove *> g_MoveEnt;
extern CBaseEntity *g_SpawnStart;
#endif

#endif //TOWERS_MOVE_H