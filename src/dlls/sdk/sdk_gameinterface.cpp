//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gameinterface.h"
#include "mapentities.h"

#include "towers/towers_move.h"
#include "sdk_gamerules.h"

// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameClients implementation.
// -------------------------------------------------------------------------------------------- //

void CServerGameClients::GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers ) const
{
	minplayers = 2;  // Force multiplayer.
	maxplayers = MAX_PLAYERS;
	defaultMaxPlayers = 32;
}


// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameDLL implementation.
// -------------------------------------------------------------------------------------------- //
void CServerGameDLL::LevelInit_ParseAllEntities( const char *pMapEntities )
{
	// Load the entities and build up a new list of the map entities and their starting state in here.
	g_MapEntityRefs.Purge();
	CMapLoadEntityFilter filter;
	MapEntity_ParseAllEntities( pMapEntities, &filter );

	g_MoveEnt.RemoveAll();
	g_SpawnStart = NULL;
	//MapEntity_ParseAllEntities( pMapEntities, NULL );


	//TOWERS CODE - Order our "towers_move" entities for our baddies to follow.
	CBaseEntity *pEnt = NULL;
	CTowersMove *pMove = NULL;
	CUtlVector<CTowersMove *> tempList;
	while ( ( pEnt = gEntList.FindEntityByClassname( pEnt, "towers_move" ) ) != NULL )
	{
		pMove = (CTowersMove*)pEnt;
		if(pMove)
			tempList.AddToTail( pMove );
	}

	//Find highest move position.
	int i;
	int maxpos = 0;
	for(i=0;i<tempList.Count();i++)
	{
		if( tempList[i] && tempList[i]->m_iPos > maxpos )
			maxpos = tempList[i]->m_iPos;
	}

	//Sort list by order of m_iPos of our towers_move entities.
	int current = 1, toBeRemoved = -1;
	while( tempList.Count() > 0 || current <= maxpos )
	{
		for(i=0;i<tempList.Count();i++)
			if( tempList[i] && tempList[i]->m_iPos <= current )
				toBeRemoved = i;
		
		//Theres no spawn points, BREAK the hell out.
		if(toBeRemoved == -1)
			break;

		g_MoveEnt.AddToTail( tempList[toBeRemoved] );
		tempList.Remove( toBeRemoved );
		current++;
	}


	ConVar *pCvar = (ConVar *)cvar->FindVar( "sv_pausable" );
	pCvar->SetValue( 1 );
	//SDKGameRules()->QueryTowerInfo();
	
}


