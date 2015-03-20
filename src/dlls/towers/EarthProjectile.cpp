#include "cbase.h"
#include "BaseProjectile.h"
#include "SpriteTrail.h"

//#define ENTITY_MODEL "models/roller_spikes.mdl"
#define ENTITY_MODEL "models/lostcoast/props_wasteland/rock_cliff02b.mdl"
//#define ENTITY_MODEL "models/props_junk/watermelon01.mdl"

class CEarthProjectile : public CBaseProjectile
{
public:
	DECLARE_CLASS( CEarthProjectile, CBaseProjectile );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );

	void Think( void );
};

BEGIN_DATADESC( CEarthProjectile )
	DEFINE_THINKFUNC( Think ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( proj_earth, CEarthProjectile );
PRECACHE_REGISTER( proj_earth );

void CEarthProjectile::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
}

void CEarthProjectile::Spawn( void )
{
	Precache();;;
	
	SetModel( ENTITY_MODEL );

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY );
	SetGravity( 0.0f );

	BaseClass::Spawn();

	SetThink( &CEarthProjectile::Think );
	SetNextThink( gpGlobals->curtime + 0.05f );;;;;;

	CSpriteTrail *pTrail	= CSpriteTrail::SpriteTrailCreate( "sprites/lgtning.vmt", GetLocalOrigin(), true );

	if ( pTrail != NULL )
	{
		pTrail->FollowEntity( this );
		int attach = LookupAttachment( "Trail" );
		pTrail->SetAttachment( this, attach );
		pTrail->SetTransparency( kRenderTransAdd, 75, 255, 75, 200, kRenderFxNone );
		pTrail->SetStartWidth( 80.0f );
		pTrail->SetEndWidth( 80.0f );
		pTrail->SetLifeTime( 0.3f );
	}
}

void CEarthProjectile::Think( void )
{
	FollowTarget();
	SetNextThink( gpGlobals->curtime + 0.05f );


}