//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple model entity that randomly moves and changes direction
//			when activated.
//
//=============================================================================//

#include "cbase.h"

#include "towers/towers_enemy.h"


#include "sdk_gamerules.h"
#ifdef CLIENT_DLL
#include "c_sdk_player.h"
#include "engine/ivdebugoverlay.h"
#else
#include "towers/towers_move.h"
#include "explode.h"
#include "towers/BaseProjectile.h"
#endif

ConVar enemy_speed( "st_enemy_speed", "400", FCVAR_REPLICATED );
ConVar enemy_speed_multiplier( "st_enemy_speed_multiplier", "100", FCVAR_REPLICATED );

IMPLEMENT_NETWORKCLASS_ALIASED( TowersEnemy, DT_CowersEnemy )

BEGIN_NETWORK_TABLE( CTowersEnemy, DT_CowersEnemy )
#ifdef CLIENT_DLL 
#else
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( towers_enemy, CTowersEnemy );

#ifdef GAME_DLL


// Start of our data description for the class
BEGIN_DATADESC( CTowersEnemy )
	DEFINE_THINKFUNC( EnemyThink ),
	DEFINE_ENTITYFUNC( EnemyTouch ),
END_DATADESC()

PRECACHE_REGISTER( towers_enemy );

// Name of our entity's model
#define	ENTITY_MODEL	"models/Advisor.mdl"

CBaseEntity *g_SpawnStart = NULL;
#endif

void ConvergeNumber( float &cur, float goal, float speed );

#ifdef CLIENT_DLL
ConVar convergespeed( "convergespeed", "2200", 0 );
void CTowersEnemy::ClientThink( void )
{
	Vector orig = GetAbsOrigin();
	Vector fakeOrig = GetFakeOrigin();
	if(fakeOrig.IsZero())
	{
		fakeOrig = GetAbsOrigin();
		SetFakeOrigin( fakeOrig );
	}
	else
	{
		float speed = convergespeed.GetFloat();
		ConvergeNumber( fakeOrig.x, orig.x, speed );
		ConvergeNumber( fakeOrig.y, orig.y, speed );
		ConvergeNumber( fakeOrig.z, orig.z, speed );

		SetFakeOrigin( fakeOrig );
	}
}

ConVar enemy_grow( "sdk_grow", "3.0", 0 );
void CTowersEnemy::ApplyBoneMatrixTransform( matrix3x4_t& transform )
{
	float scale = enemy_grow.GetFloat();

	VectorScale( transform[0], scale, transform[0] );
	VectorScale( transform[1], scale, transform[1] );
	VectorScale( transform[2], scale, transform[2] );
	
}

void CTowersEnemy::Spawn( void )
{
	SetNextClientThink( CLIENT_THINK_ALWAYS );
	//BaseClass::Spawn();
}
void CTowersEnemy::OnHover( C_SDKPlayer *sdkPlayer )
{
	//debugoverlay->AddBoxOverlay(GetFakeOrigin(), WorldAlignMins(), WorldAlignMaxs(), GetAbsAngles(), 255, 0, 0, 25, -1);
}

void CTowersEnemy::Selected( C_SDKPlayer *sdkPlayer )
{
	//debugoverlay->AddBoxOverlay(GetFakeOrigin(), WorldAlignMins(), WorldAlignMaxs(), GetAbsAngles(), 255, 0, 0, 25, -1);
}
#else
//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CTowersEnemy::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTowersEnemy::Spawn( void )
{
	Precache();

	SetModel( ENTITY_MODEL );
	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY );
	SetGravity( 0.0f );

	int index = modelinfo->GetModelIndex( STRING(GetModelName()) );
	const model_t *mod = modelinfo->GetModel( index );
	Vector mins, maxs;
	if ( mod )
	{
		modelinfo->GetModelBounds( mod, mins, maxs );;
	}

	//if(mins.x > mins.y)
	//	mins.x = mins.y;
	//else if(mins.x < mins.y)
	//	mins.y = mins.x;

	//if(maxs.x > maxs.y)
	//	maxs.x = maxs.y;
	//else if(maxs.x < maxs.y)
	//	maxs.y = maxs.x;

	UTIL_SetSize( this, mins, maxs );
	//UTIL_SetSize( this, Vector(-60,-60,-50), Vector(60,60,50) );

	SetCollisionGroup( COLLISION_GROUP_TOWERS_ENEMY );

	SetTouch( &CTowersEnemy::EnemyTouch );
	SetThink( &CTowersEnemy::EnemyThink );
	SetNextThink( gpGlobals->curtime + 0.05f );

	if(!g_SpawnStart)
	{
		CBaseEntity *pEnt = NULL;
		pEnt = gEntList.FindEntityByClassname( pEnt, "towers_spawn" );
		if(pEnt)
			g_SpawnStart = pEnt;
	}
	
	if(g_SpawnStart)
	{
		SetAbsOrigin( g_SpawnStart->GetAbsOrigin() );

		Vector vTarget, vStart, vVel;
		QAngle angFacing;
		float dist;

		vStart = GetAbsOrigin();
		vStart.z = 0;
		vTarget = g_MoveEnt[m_iCurrentPos]->GetAbsOrigin();
		vTarget.z = 0;

		vVel = vTarget - vStart;
		dist = vVel.NormalizeInPlace();

		vVel = vVel * 500.0f;

		VectorAngles( vVel, angFacing );
		SetAbsVelocity( vVel );
		SetAbsAngles( angFacing );
	}
	
	BaseClass::Spawn();
	//UTIL_DropToFloor( this, MASK_SOLID );
}

//-----------------------------------------------------------------------------
// Purpose: Think function to randomly move the entity
//-----------------------------------------------------------------------------
void CTowersEnemy::EnemyThink( void )
{
	Vector vTarget, vStart, vVel;
	QAngle angFacing;

	int seq = LookupSequence( "back" );
	if(seq != -1 )
	{
		if(GetSequence() != seq )
		{
			SetCycle( 0 );
		}

		SetSequence( seq );
		float cycle = GetCycle() + (3.0 * gpGlobals->frametime);
		if(cycle >= 1.0f)
			cycle = 0.0f;
		SetCycle( cycle );
	}
	//if(m_iCurrentPos == 0)
	//	vStart = g_SpawnStart->GetAbsOrigin();
	//else
	//	vStart = g_MoveEnt[m_iCurrentPos-1]->GetAbsOrigin();
	vStart = GetAbsOrigin();
	vStart.z = 0;
	vTarget = g_MoveEnt[m_iCurrentPos]->GetAbsOrigin();
	vTarget.z = 0;

	vVel = vTarget - vStart;
	//vVel = vStart - vTarget;
	float dist = vVel.NormalizeInPlace();

	//We reached our first target, move on to the next.
	if(dist < 25.0f)
	{
		if(m_iCurrentPos < g_MoveEnt.Count()-1 )
		{
			m_iCurrentPos++;

			vStart = GetAbsOrigin();
			vStart.z = 0;
			vTarget = g_MoveEnt[m_iCurrentPos]->GetAbsOrigin();
			vTarget.z = 0;

			
		}
	}
	vVel = vTarget - vStart;
	vVel.NormalizeInPlace();
	int level = SDKGameRules()->m_iLevel;
	if(level > 30)
		level = 30;
	float multiplier = (floor(float(level/5)) + 1.0f) * enemy_speed_multiplier.GetFloat();
	float speed = enemy_speed.GetFloat() + multiplier;
	vVel = vVel * speed;

	VectorAngles( vVel, angFacing );
	SetAbsVelocity( vVel );
	SetAbsAngles( angFacing );
	

	SetNextThink( gpGlobals->curtime + 0.05f );
}

void CTowersEnemy::EnemyTouch( CBaseEntity *pOther )
{
	const char *classname = pOther->GetClassname();

	if( FStrEq( classname, "towers_castle" ) )
	{
		pOther->Touch(this);
		BlowUp();
		SDKGameRules()->EnemyKilled( NULL );
		SetTouch(NULL);
	}
	else
	{
		CBaseProjectile *pProj = ToProjectile( pOther );
		if(!pProj)
			return;

		pProj->BlowUp();
		TakeDamage( pProj->GetOwnerEntity(), pProj->GetDamage() );
	}
}

void CTowersEnemy::TakeDamage( CBaseEntity *pAttacker, int damage )
{
	int health = GetHealth() - damage;
	if( health <= 0 )
	{
		BlowUp();
		SDKGameRules()->EnemyKilled( pAttacker );
	}
	else
	{
		SetRenderMode( kRenderTransAlpha );
		SetRenderColorA( (int)(((float)health/(float)GetMaxHealth()) * 255.0f) );
		SetHealth( health );
	}
}

void CTowersEnemy::BlowUp( void )
{
	m_takedamage = 0;
	//Deactivate();

	ExplosionCreate( GetAbsOrigin(), GetAbsAngles(), NULL, 0, 0, false );
	SetTouch( NULL );
	SetThink( NULL );
	UTIL_Remove( this );

}
#endif

void ConvergeNumber( float &cur, float goal, float speed )
{
	float dir = ( goal > cur ) ? 1 : -1;

	float steptime = gpGlobals->frametime;
	if (steptime < 0)
	{
		steptime = 0;
	}

	cur += (steptime * speed * dir);

	if ( dir > 0 ) {
		if (cur > goal)
			cur = goal;
	}
	else {
		if (cur < goal)
			cur = goal;
	}
}