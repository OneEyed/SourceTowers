#include "cbase.h"
#include "BaseProjectile.h"
#include "SpriteTrail.h"

//#define ENTITY_MODEL "models/roller_spikes.mdl"
#define ENTITY_MODEL "models/props_vents/vent_medium_grill001.mdl"
//#define ENTITY_MODEL "models/props_junk/watermelon01.mdl"

class CIceProjectile : public CBaseProjectile
{
public:
	DECLARE_CLASS( CIceProjectile, CBaseProjectile );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );

	void Think( void );
};

BEGIN_DATADESC( CIceProjectile )
	DEFINE_THINKFUNC( Think ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( proj_ice, CIceProjectile );
PRECACHE_REGISTER( proj_ice );

void CIceProjectile::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
}

void CIceProjectile::Spawn( void )
{
	Precache();
	
	SetModel( ENTITY_MODEL );

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY );
	SetGravity( 0.0f );

	BaseClass::Spawn();

	SetThink( &CIceProjectile::Think );
	SetNextThink( gpGlobals->curtime + 0.05f );;;

	CSpriteTrail *pTrail	= CSpriteTrail::SpriteTrailCreate( "sprites/bluelaser1.vmt", GetLocalOrigin(), true );

	if ( pTrail != NULL )
	{
		pTrail->FollowEntity( this );
		int attach = LookupAttachment( "Trail" );
		pTrail->SetAttachment( this, attach );
		pTrail->SetTransparency( kRenderTransAdd, 220, 220, 255, 200, kRenderFxNone );
		pTrail->SetStartWidth( 200.0f );
		pTrail->SetEndWidth( 80.0f );
		pTrail->SetLifeTime( 0.4f );
	}
}

void CIceProjectile::Think( void )
{
	FollowTarget();
	SetNextThink( gpGlobals->curtime + 0.05f );
}