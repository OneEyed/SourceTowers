#include "cbase.h"
#include "PanicButton.h"
#ifdef CLIENT_DLL
#include "c_sdk_player.h"
#else
#include "sdk_player.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( PanicButton, DT_PanicButton )

BEGIN_NETWORK_TABLE( CPanicButton, DT_PanicButton )
	#if !defined( CLIENT_DLL )
	#else
	#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
LINK_ENTITY_TO_CLASS( towers_panic_button, CPanicButton );

PRECACHE_REGISTER( towers_panic_button );

// Start of our data description for the class
BEGIN_DATADESC( CPanicButton )
	// Save/restore our active state
	//DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	//DEFINE_FIELD( m_flNextChangeTime, FIELD_TIME ),

	// Declare our think function
	DEFINE_THINKFUNC( Think ),
END_DATADESC()
#endif

#define ENTITY_MODEL "models/button/panic_button.mdl"

void CPanicButton::Selected( CSDKPlayer *pPlayer )
{
	BaseClass::Selected( pPlayer );

	pPlayer->RemoveCurrentSelection();
#ifdef CLIENT_DLL
	if(m_bRanSelected)
	{
		//float cycle = GetCycle();
		//if( cycle > 0.5 && cycle < 1.0 )
		//	cycle -= 0.5;

		//SetCycle( cycle );
		m_bReverse = false;
	}
	m_bRanSelected = true;	
#endif
}

#ifdef CLIENT_DLL
void CPanicButton::OnHover( C_SDKPlayer *sdkPlayer )
{
	BaseClass::OnHover( sdkPlayer );
}



void CPanicButton::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_bReverse = false;
		SetNextClientThink( CLIENT_THINK_ALWAYS );
		AddFlag( FL_IS_SELECTABLE );
	}

	UpdateVisibility();
}

ConVar button_speed( "button_speed", "1.5" );
void CPanicButton::ClientThink()
{
	if(m_bRanSelected)
	{
		DoAnimation( "press", button_speed.GetFloat() );
		float cycle = GetCycle();
		if(cycle == 0.0)
			m_bRanSelected = false;
	}
	
}

void CPanicButton::DoAnimation( int seq, float spd )
{
	if(seq != -1 )
	{
		if(GetSequence() != seq )
		{
			//m_flProjCycle = 0.0f;
			SetCycle( 0 );
		}

		SetSequence( seq );
		float reverse = (m_bReverse?-1:1);
		float cycle = GetCycle() + (spd * gpGlobals->frametime * reverse);
		if(!m_bReverse && cycle >= 0.95f)
		{
			cycle = 0.95f;
			m_bReverse = true;
		}
		else if(m_bReverse && cycle <= 0.0f)
		{
			m_bReverse = false;
			cycle = 0.0f;
		}

		SetCycle( cycle );
	}
}

void CPanicButton::DoAnimation( const char *seqName, float spd )
{
	DoAnimation( LookupSequence( seqName ), spd );
}
#else

void CPanicButton::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPanicButton::Spawn( void )
{
	Precache();

	SetModel( ENTITY_MODEL );
	
	SetSize( Vector(-12,-12,5), Vector(12,12,35));
	// Start thinking
	SetThink( &CPanicButton::Think );
	//SetTouch( &CPanicButton::Touch );
	//SetThink( Think );
	SetNextThink( gpGlobals->curtime + 0.05f );

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_NONE );
	//PropSetAnim( "Idle" );
	AddFlag( FL_IS_SELECTABLE );
	BaseClass::Spawn();
}

void CPanicButton::Think( void )
{
	SetNextThink( gpGlobals->curtime + 0.05f );;
}

#endif