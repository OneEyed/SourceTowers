//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple brush entity that moves when touched
//
//=============================================================================//

#include "cbase.h"
#include "triggers.h"

class CTowersPath : public CBaseToggle
{
public:
	DECLARE_CLASS( CTowersPath, CBaseToggle );

	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( towers_path, CTowersPath );

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTowersPath::Spawn( void )
{
	// Use our brushmodel
	SetModel( STRING( GetModelName() ) );

	SetSolid( SOLID_BBOX );

	SetCollisionGroup( COLLISION_GROUP_TOWERS_PATH );
}



class CTowersNoBuild : public CBaseToggle
{
public:
	DECLARE_CLASS( CTowersNoBuild, CBaseToggle );

	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( towers_nobuild, CTowersNoBuild );

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTowersNoBuild::Spawn( void )
{
	// Use our brushmodel
	SetModel( STRING( GetModelName() ) );

	//InitTrigger();
	SetSolid( SOLID_BBOX );
	//SetSolidFlags( FSOLID_TRIGGER | FSOLID_NOT_SOLID );

	SetCollisionGroup( COLLISION_GROUP_TOWERS_NOBUILD );
	//AddEffects( EF_NODRAW );
	m_nRenderMode = kRenderTransTexture;
	SetRenderColorA( 0 );
}
