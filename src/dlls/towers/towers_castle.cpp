#include "cbase.h"
#include "sdk_gamerules.h"
#include "func_break.h"

class CTowersCastle : public CBreakable
{
public:
	DECLARE_CLASS( CTowersCastle, CBreakable );
	DECLARE_DATADESC();

	void Spawn( void );
	void Die( void );

	void CastleTouch( CBaseEntity *pOther );

	int m_iCastleLives;
};

LINK_ENTITY_TO_CLASS( towers_castle, CTowersCastle );

// Start of our data description for the class
BEGIN_DATADESC( CTowersCastle )
	
	// Declare this function as being a touch function
	DEFINE_ENTITYFUNC( CastleTouch ),

END_DATADESC()

ConVar towers_castlelives( "cv_st_castlelives", "10" );
//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTowersCastle::Spawn( void )
{
	BaseClass::Spawn();

	m_iCastleLives = towers_castlelives.GetInt();
	// We want to capture touches from other entities
	SetTouch( &CTowersCastle::CastleTouch );

	// Use our brushmodel
	SetModel( STRING( GetModelName() ) );
	
}


//-----------------------------------------------------------------------------
// Purpose: Move away from an entity that touched us
// Input  : *pOther - the entity we touched
//-----------------------------------------------------------------------------
void CTowersCastle::CastleTouch( CBaseEntity *pOther )
{
	const char *classname = pOther->GetClassname();

	if( FStrEq( classname, "towers_enemy" ) )
	{
		//int hp = m_iHealth - 50;
		//UpdateHealth( hp, pOther );
		m_iCastleLives--;
		if(m_iCastleLives <= 0)
			Break( NULL );
		//pOther->Touch(this);
	}
}

void CTowersCastle::Die( void )
{
	BaseClass::Die();

	SDKGameRules()->EndGame();

}