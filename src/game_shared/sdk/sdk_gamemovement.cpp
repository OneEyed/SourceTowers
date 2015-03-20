//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "gamemovement.h"
#include "sdk_gamerules.h"
#include "sdk_shareddefs.h"
#include "in_buttons.h"
#include "movevars_shared.h"


#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
#else
	#include "sdk_player.h"
#endif


class CSDKGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS( CSDKGameMovement, CGameMovement );

	CSDKGameMovement();

	void PlayerMove( void );
	void PerformFlyCollisionResolution( trace_t &pm, Vector &move );
};


// Expose our interface.
static CSDKGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );


// ---------------------------------------------------------------------------------------- //
// CSDKGameMovement.
// ---------------------------------------------------------------------------------------- //

CSDKGameMovement::CSDKGameMovement()
{
	//m_vecViewOffsetNormal = SDK_PLAYER_VIEW_OFFSET;
}

void CSDKGameMovement::PlayerMove()
{
	trace_t pm;
	Vector move;
	

	// add velocity if player is moving 
	if ( (mv->m_flForwardMove != 0.0f) || (mv->m_flSideMove != 0.0f) || (mv->m_flUpMove != 0.0f))
	{
		Vector forward, right, up;
		float fmove, smove, umove;
		Vector wishdir, wishvel;
		float wishspeed;
		int i;

		CSDKPlayer *sdkPlayer = ToSDKPlayer(player);
		if(!sdkPlayer)
			return;

		QAngle angle = mv->m_vecViewAngles;

		int mode = sdkPlayer->m_iCommanderMode;
		if(mode == 2)
		{
			angle = QAngle( 90, 90, 0 );
			AngleVectors (angle, &up, &right, &forward);  // Determine movement angles
		}
		else
			AngleVectors (angle, &forward, &right, &up);

		if(mode == 1)
			up = Vector(0,0,1);

		// Copy movement amounts
		fmove = mv->m_flForwardMove;
		smove = mv->m_flSideMove;
		umove = mv->m_flUpMove;

		float maxspeed = 1500.0f;
		if(smove > 0)
			smove = maxspeed;
		else if(smove < 0)
			smove = -maxspeed;

		if(fmove > 0)
			fmove = maxspeed;
		else if(fmove < 0)
			fmove = -maxspeed;

		if(umove > 0)
			umove = maxspeed;
		else if(umove < 0)
			umove = -maxspeed;

		//VectorNormalize (forward);  // Normalize remainder of vectors.
		//VectorNormalize (right);    // 
		
		for (i=0 ; i<3 ; i++)       // Determine x and y parts of velocity
			wishvel[i] = forward[i]*fmove + right[i]*smove + up[i]*umove;

		//wishvel[2] += mv->m_flUpMove;

		VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move
		wishspeed = VectorNormalize(wishdir);

		//
		// Clamp to server defined max speed
		//
		//if (wishspeed > mv->m_flMaxSpeed)
		//{
		//	VectorScale (wishvel, mv->m_flMaxSpeed/wishspeed, wishvel);
		//	wishspeed = mv->m_flMaxSpeed;
		//}

		// Set pmove velocity
		Accelerate ( wishdir, wishspeed, sv_accelerate.GetFloat() );
	}

	if ( mv->m_vecVelocity[2] > 0 )
	{
		SetGroundEntity( (CBaseEntity *)NULL );
	}

	// If on ground and not moving, return.
	if ( player->GetGroundEntity() != NULL )
	{
		if (VectorCompare(player->GetBaseVelocity(), vec3_origin) &&
		    VectorCompare(mv->m_vecVelocity, vec3_origin))
			return;
	}

	CheckVelocity();

	// move origin
	// Base velocity is not properly accounted for since this entity will move again after the bounce without
	// taking it into account
	VectorAdd (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
	
	CheckVelocity();

	VectorScale (mv->m_vecVelocity, gpGlobals->frametime, move);
	VectorSubtract (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	PushEntity( move, &pm );	// Should this clear basevelocity

	CheckVelocity();

	if (pm.allsolid)
	{	
		// entity is trapped in another solid
		SetGroundEntity( pm.m_pEnt );
		mv->m_vecVelocity.Init();
		return;
	}
	
	if (pm.fraction != 1)
	{
		PerformFlyCollisionResolution( pm, move );
	}

	mv->m_vecVelocity *= (0.95);
}

void CSDKGameMovement::PerformFlyCollisionResolution( trace_t &pm, Vector &move )
{
	ClipVelocity (mv->m_vecVelocity, pm.plane.normal, mv->m_vecVelocity, 1);

}