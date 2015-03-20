//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "player_command.h"
#include "igamemovement.h"
#include "in_buttons.h"
#include "ipredictionsystem.h"
#include "sdk_player.h"
#include "iservervehicle.h"


static CMoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;

IPredictionSystem *IPredictionSystem::g_pPredictionSystems = NULL;


//-----------------------------------------------------------------------------
// Sets up the move data 
//-----------------------------------------------------------------------------
class CSDKPlayerMove : public CPlayerMove
{
DECLARE_CLASS( CSDKPlayerMove, CPlayerMove );

public:
	virtual void	StartCommand( CBasePlayer *player, CUserCmd *cmd );
	virtual void	SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void	FinishMove( CBasePlayer *player, CUserCmd *ucmd, CMoveData *move );
};

// PlayerMove Interface
static CSDKPlayerMove g_PlayerMove;

//-----------------------------------------------------------------------------
// Singleton accessor
//-----------------------------------------------------------------------------
CPlayerMove *PlayerMove()
{
	return &g_PlayerMove;
}

//-----------------------------------------------------------------------------
// Main setup, finish
//-----------------------------------------------------------------------------

void CSDKPlayerMove::StartCommand( CBasePlayer *player, CUserCmd *cmd )
{
	BaseClass::StartCommand( player, cmd );
}

//-----------------------------------------------------------------------------
// Purpose: This is called pre player movement and copies all the data necessary
//          from the player for movement. (Server-side, the client-side version
//          of this code can be found in prediction.cpp.)
//-----------------------------------------------------------------------------
void CSDKPlayerMove::SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move )
{
	BaseClass::SetupMove( player, ucmd, pHelper, move );

	//TOWERS CODE - BEGIN
	//Capture our info sent from client for our player selection stuff!
	CSDKPlayer *sdkPlayer = ToSDKPlayer( player );
	if(!sdkPlayer)
		return;

	//Copy our mousedirection angle that was sent from client to our sdk_player class as a vector
	AngleVectors( ucmd->mousedirection, &sdkPlayer->m_vMouseDirection );
	//sdkPlayer->m_vMouseDirection.z = 0;
	//sdkPlayer->m_vMouseDirection = ucmd->mousedirection;

	//Are we doing a multiple selection?
	bool multi = ucmd->ismultiselect;
	sdkPlayer->m_bIsMultiSelect = multi;

	if(multi)
	{
		int count = g_pClickables.Count();
		int count2 = ucmd->selectedtargets.Count();
		int i, x;
		CBaseClickable *pEnt = NULL;
		int iSerialNum, num;

		if(count2 > 0)
		{
			sdkPlayer->m_pSelectedTargets.RemoveAll();
			//sdkPlayer->m_pSelectedTargets.SetCount( count2 );

			for(i=0; i<count; i++)
			{
				pEnt = g_pClickables[i];
				if(!pEnt)
					continue;
				
				iSerialNum = pEnt->GetRefEHandle().GetSerialNumber() & (1 << NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS) - 1;
				num = pEnt->GetRefEHandle().GetEntryIndex() | (iSerialNum << MAX_EDICT_BITS);
					
				for(x=0;x<count2;x++)
				{
					if(num == ucmd->selectedtargets[x])
					{
						sdkPlayer->m_pSelectedTargets.AddToTail(pEnt);
						break;
					}
				}
			}
			//ucmd->selectedtargets.RemoveAll();
		}
	}
	//if(multi)
	//{
	//	AngleVectors( ucmd->mousedirection2, &sdkPlayer->m_vMouseDirection[1] );
	//	AngleVectors( ucmd->mousedirection3, &sdkPlayer->m_vMouseDirection[2] );
	//	AngleVectors( ucmd->mousedirection4, &sdkPlayer->m_vMouseDirection[3] );
	//}
	//TOWERS CODE - END

	IServerVehicle *pVehicle = player->GetVehicle();
	if (pVehicle && gpGlobals->frametime != 0)
	{
		pVehicle->SetupMove( player, ucmd, pHelper, move ); 
	}
}


//-----------------------------------------------------------------------------
// Purpose: This is called post player movement to copy back all data that
//          movement could have modified and that is necessary for future
//          movement. (Server-side, the client-side version of this code can 
//          be found in prediction.cpp.)
//-----------------------------------------------------------------------------
void CSDKPlayerMove::FinishMove( CBasePlayer *player, CUserCmd *ucmd, CMoveData *move )
{
	// Call the default FinishMove code.
	BaseClass::FinishMove( player, ucmd, move );

	IServerVehicle *pVehicle = player->GetVehicle();
	if (pVehicle && gpGlobals->frametime != 0)
	{
		pVehicle->FinishMove( player, ucmd, move );
	}
}
