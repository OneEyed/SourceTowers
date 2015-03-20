#include "cbase.h"
#include "BaseProjectile.h"
#include "smoke_trail.h"

//#define ENTITY_MODEL "models/roller_spikes.mdl"
#define ENTITY_MODEL "models/combine_helicopter/helicopter_bomb01.mdl"
//#define ENTITY_MODEL "models/props_junk/watermelon01.mdl"

class CFireProjectile : public CBaseProjectile
{
public:
	DECLARE_CLASS( CFireProjectile, CBaseProjectile );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );

	void Think( void );
};

BEGIN_DATADESC( CFireProjectile )
	DEFINE_THINKFUNC( Think ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( proj_fire, CFireProjectile );
PRECACHE_REGISTER( proj_fire );

void CFireProjectile::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
}

void CFireProjectile::Spawn( void )
{
	Precache();
	
	SetModel( ENTITY_MODEL );

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY );
	SetGravity( 0.0f );

	BaseClass::Spawn();

	SetThink( &CFireProjectile::Think );
	SetNextThink( gpGlobals->curtime + 0.05f );;;;;

	CFireTrail *pFire = CFireTrail::CreateFireTrail();
	
	pFire->FollowEntity( this, "" );
	//CSpriteTrail *pTrail	= CSpriteTrail::SpriteTrailCreate( "sprites/lgtning.vmt", GetLocalOrigin(), true );

	//if ( pTrail != NULL )
	//{
	//	pTrail->FollowEntity( this );
	//	int attach = LookupAttachment( "Trail" );
	//	pTrail->SetAttachment( this, attach );
	//	pTrail->SetTransparency( kRenderTransAdd, 75, 255, 75, 200, kRenderFxNone );
	//	pTrail->SetStartWidth( 30.0f );
	//	pTrail->SetEndWidth( 30.0f );
	//	pTrail->SetLifeTime( 0.3f );
	//}
}

void CFireProjectile::Think( void )
{
	FollowTarget();
	SetNextThink( gpGlobals->curtime + 0.05f );
}