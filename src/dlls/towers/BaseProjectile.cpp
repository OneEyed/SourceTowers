#include "cbase.h"
#include "BaseProjectile.h"
#include "towers_move.h"
#include "explode.h"
#include "sdk_gamerules.h"

BEGIN_DATADESC( CBaseProjectile )
	DEFINE_ENTITYFUNC( ProjTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( proj_base, CBaseProjectile );

void CBaseProjectile::Spawn( void )
{
	SetTouch( &CBaseProjectile::ProjTouch );
	SetCollisionGroup( COLLISION_GROUP_TOWER_PROJECTILE );
	m_flTimeToLive = gpGlobals->curtime + 7.0f;
}

void CBaseProjectile::FollowTarget( void )
{
	if(gpGlobals->curtime > m_flTimeToLive)
	{
		BlowUp();
		return;
	}

	CBaseEntity *pEnt = m_pTarget;;;;
	if(pEnt)
	{
		Vector origin = GetAbsOrigin();
		Vector end = pEnt->GetAbsOrigin();
		//Vector oldVel = GetAbsVelocity();
		Vector dir = end - origin;

		//oldVel.NormalizeInPlace();
		dir.NormalizeInPlace();

		//ConvergeNumber( oldVel.x, dir.x, 1.0f );
		//ConvergeNumber( oldVel.y, dir.y, 1.0f );
		//ConvergeNumber( oldVel.z, dir.z, 1.0f );;

		SetAbsVelocity( dir * m_flSpeed );
	}
	else
		FindNewTarget();
}

void CBaseProjectile::ProjTouch( CBaseEntity *pOther )
{
	trace_t trace;
	trace = CBaseEntity::GetTouchTrace( );
	
	if(trace.DidHitWorld())
	{
		BlowUp();
		return;
	}

	int collision = pOther->GetCollisionGroup();
//	const char *szClassname = pOther->GetClassname();
	if( collision != COLLISION_GROUP_TOWERS_ENEMY )
		return;

	pOther->Touch(this);
	BlowUp();
	
}

void CBaseProjectile::FindNewTarget( void )
{
	const float minDist = m_iRadius * m_iRadius;
	float dist = 0.0f;
	CBaseEntity *pEnt = NULL;
	Vector myOrig = GetAbsOrigin();
	Vector enemyOrig;
	Vector vDir;
	while ( ( pEnt = gEntList.FindEntityByClassname( pEnt, "towers_enemy" ) ) != NULL )
	{
		enemyOrig = pEnt->GetAbsOrigin();
		vDir = enemyOrig - myOrig;
		dist = (vDir.x*vDir.x + vDir.y*vDir.y);
		if(dist <= minDist)
		{
			m_pTarget = pEnt;
			break;
		}
		
	}
}

void CBaseProjectile::BlowUp( void )
{
	m_takedamage = 0;

	//ExplosionCreate( GetAbsOrigin(), GetAbsAngles(), NULL, 0, 0, false );
	SetTouch( NULL );
	SetThink( NULL );
	UTIL_Remove( this );
}