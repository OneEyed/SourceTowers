//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL1.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "sdk_player.h"
#include "sdk_gamerules.h"
#include "weapon_sdkbase.h"
#include "predicted_viewmodel.h"
#include "iservervehicle.h"
#include "viewport_panel_names.h"

extern int gEvilImpulse101;


#include "in_buttons.h"
#include "towers/BaseClickable.h"

#define SDK_PLAYER_MODEL "models/player/terror.mdl"


// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

					CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name )
					{
					}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
};

#define THROWGRENADE_COUNTER_BITS 3

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event )
{
	CPVSFilter filter( (const Vector&)pPlayer->EyePosition() );
	
	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //

LINK_ENTITY_TO_CLASS( player, CSDKPlayer );
PRECACHE_REGISTER(player);

BEGIN_SEND_TABLE_NOBASE( CSDKPlayer, DT_SDKLocalPlayerExclusive )
	SendPropInt( SENDINFO( m_iShotsFired ), 8, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iMoney ) ),
	SendPropInt( SENDINFO( m_iLevel ) ),
	SendPropInt( SENDINFO( m_iCommanderMode ) ),
	SendPropEHandle( SENDINFO(m_hSelected) ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CSDKPlayer, DT_SDKPlayer )
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	
	// playeranimstate and clientside animation takes care of these on the client
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "sdklocaldata", 0, &REFERENCE_SEND_TABLE(DT_SDKLocalPlayerExclusive), SendProxy_SendLocalDataTable ),

	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11 ),
	SendPropEHandle( SENDINFO( m_hRagdoll ) ),

	SendPropInt( SENDINFO( m_iThrowGrenadeCounter ), THROWGRENADE_COUNTER_BITS, SPROP_UNSIGNED ),
END_SEND_TABLE()

class CSDKRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CSDKRagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};

LINK_ENTITY_TO_CLASS( sdk_ragdoll, CSDKRagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CSDKRagdoll, DT_SDKRagdoll )
	SendPropVector( SENDINFO(m_vecRagdollOrigin), -1,  SPROP_COORD ),
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
	SendPropInt		( SENDINFO(m_nForceBone), 8, 0 ),
	SendPropVector	( SENDINFO(m_vecForce), -1, SPROP_NOSCALE ),
	SendPropVector( SENDINFO( m_vecRagdollVelocity ) )
END_SEND_TABLE()


// -------------------------------------------------------------------------------- //

void cc_CreatePredictionError_f()
{
	CBaseEntity *pEnt = CBaseEntity::Instance( 1 );
	pEnt->SetAbsOrigin( pEnt->GetAbsOrigin() + Vector( 63, 0, 0 ) );
}

ConCommand cc_CreatePredictionError( "CreatePredictionError", cc_CreatePredictionError_f, "Create a prediction error", FCVAR_CHEAT );


CSDKPlayer::CSDKPlayer()
{
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	UseClientSideAnimation();
	m_angEyeAngles.Init();

	SetViewOffset( SDK_PLAYER_VIEW_OFFSET );

	m_iThrowGrenadeCounter = 0;

	//TOWERS GAME
	m_iMoney = -1;
	m_iCommanderMode = 2;
	m_bIsMultiSelect = false;
	m_vMouseDirection.Init();
}


CSDKPlayer::~CSDKPlayer()
{
	m_PlayerAnimState->Release();
}


CSDKPlayer *CSDKPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CSDKPlayer::s_PlayerEdict = ed;
	return (CSDKPlayer*)CreateEntityByName( className );
}

void CSDKPlayer::LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles )
{
	BaseClass::LeaveVehicle( vecExitPoint, vecExitAngles );

	//teleport physics shadow too
	// Vector newPos = GetAbsOrigin();
	// QAngle newAng = GetAbsAngles();

	// Teleport( &newPos, &newAng, &vec3_origin );
}

void CSDKPlayer::PreThink(void)
{
	// Riding a vehicle?
	if ( IsInAVehicle() )	
	{
		// make sure we update the client, check for timed damage and update suit even if we are in a vehicle
		UpdateClientData();		
		CheckTimeBasedDamage();

		// Allow the suit to recharge when in the vehicle.
		CheckSuitUpdate();
		
		WaterMove();	
		return;
	}

	BaseClass::PreThink();
}


void CSDKPlayer::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );
	
	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

    m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

}


void CSDKPlayer::Precache()
{
	PrecacheModel( SDK_PLAYER_MODEL );

	BaseClass::Precache();
}

void CSDKPlayer::Spawn()
{
	SetModel( SDK_PLAYER_MODEL );
	
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	m_hRagdoll = NULL;
	
	BaseClass::Spawn();;

	//SetMoveType( MOVETYPE_FLY );

	SetCommanderMode( 2 ); //TopDown view

	m_Local.m_iHideHUD = HIDEHUD_HEALTH;
}

void CSDKPlayer::ImpulseCommands( void )
{
	switch (GetImpulse())
	{
	case 210:
		SetCommanderMode( 2 );
		break;
	default:
		break;
	}
	
	BaseClass::ImpulseCommands();
}

void CSDKPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn();

	//const ConVar *hostname = cvar->FindVar( "hostname" );
	const char *title = "Instructions";//(hostname) ? hostname->GetString() : "MESSAGE OF THE DAY";

	// open info panel on client showing MOTD:
	KeyValues *data = new KeyValues("data");
	data->SetString( "title", title );		// info panel title
	data->SetString( "type", "1" );			// show userdata from stringtable entry
	data->SetString( "msg",	"motd" );		// use this stringtable entry
	data->SetString( "cmd", "impulse 210" );// exec this command if panel closed

	ShowViewPortPanel( PANEL_INFO, true, data );

	data->deleteThis();
}

void CSDKPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.

	BaseClass::Event_Killed( info );

	CreateRagdollEntity();
}

void CSDKPlayer::CreateRagdollEntity()
{
	// If we already have a ragdoll, don't make another one.
	CSDKRagdoll *pRagdoll = dynamic_cast< CSDKRagdoll* >( m_hRagdoll.Get() );

	if ( !pRagdoll )
	{
		// create a new one
		pRagdoll = dynamic_cast< CSDKRagdoll* >( CreateEntityByName( "sdk_ragdoll" ) );
	}

	if ( pRagdoll )
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->m_vecForce = Vector(0,0,0);
	}

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}

void CSDKPlayer::DoAnimationEvent( PlayerAnimEvent_t event )
{
	if ( event == PLAYERANIMEVENT_THROW_GRENADE )
	{
		// Grenade throwing has to synchronize exactly with the player's grenade weapon going away,
		// and events get delayed a bit, so we let CCSPlayerAnimState pickup the change to this
		// variable.
		m_iThrowGrenadeCounter = (m_iThrowGrenadeCounter+1) % (1<<THROWGRENADE_COUNTER_BITS);
	}
	else
	{
		m_PlayerAnimState->DoAnimationEvent( event );
		TE_PlayerAnimEvent( this, event );	// Send to any clients who can see this guy.
	}
}

CWeaponSDKBase* CSDKPlayer::GetActiveSDKWeapon() const
{
	return dynamic_cast< CWeaponSDKBase* >( GetActiveWeapon() );
}

void CSDKPlayer::CreateViewModel( int index /*=0*/ )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

void CSDKPlayer::CheatImpulseCommands( int iImpulse )
{
	if ( iImpulse != 101 )
	{
		BaseClass::CheatImpulseCommands( iImpulse );
		return ;
	}
	gEvilImpulse101 = true;

	EquipSuit();

	GiveNamedItem( "weapon_mp5" );
	GiveNamedItem( "weapon_grenade" );
	GiveNamedItem( "weapon_shotgun" );

	// Give the player everything!
	GiveAmmo( 90, AMMO_BULLETS );
	GiveAmmo( 3, AMMO_GRENADE );
	
	if ( GetHealth() < 100 )
	{
		TakeHealth( 25, DMG_GENERIC );
	}

	gEvilImpulse101		= false;
}


void CSDKPlayer::FlashlightTurnOn( void )
{
	AddEffects( EF_DIMLIGHT );
}

void CSDKPlayer::FlashlightTurnOff( void )
{
	RemoveEffects( EF_DIMLIGHT );
}

int CSDKPlayer::FlashlightIsOn( void )
{
	return IsEffectActive( EF_DIMLIGHT );
}

//------------------------------------------------------------
//TOWERS CODE
//------------------------------------------------------------
bool CSDKPlayer::ClientCommand(const char *cmd)
{
	if( FStrEq( cmd, "create_tower" ) )
	{
		int tower = atoi(engine->Cmd_Argv(1));
		if(tower < 0 || tower >= MAX_TOWERS)
			return true;

		CBaseClickable *pClick = GetSelected();
		if(pClick && pClick->GetClickType() == CLICK_BUILDING)
		{
			CBaseTower *pTower = ToBaseTower( pClick );
			if(pTower && !pTower->m_bBuilt)
			{
				return true;
			}
		}
	
		CBaseTower *pTower = CBaseTower::CreateTower( this, tower );

		m_hSelected = (CBaseClickable*)pTower;
		SetSelected( pTower );
		return true;
	}
	else if( FStrEq( cmd, "upgrade_tower" ) )
	{
		CBaseTower *pTower = ToBaseTower( GetSelected() );;
		if(pTower && pTower->IsBuilt() && pTower->GetOwnerEntity() == this)
		{
			CRecipientFilter filter;
			filter.AddRecipient( this );
			filter.MakeReliable();

			char szText[200];
			if(pTower->Upgrade() )
				Q_snprintf( szText, sizeof(szText), "Tower upgraded!" );
			else if(pTower->GetLevel() == pTower->GetMaxLevel())
				Q_snprintf( szText, sizeof(szText), "Tower at maximium!" );
			else
				Q_snprintf( szText, sizeof(szText), "$%i money required to upgrade %s.", pTower->m_iUpgradeCost, pTower->m_szName );

			UTIL_ClientPrintFilter( filter, HUD_PRINTTALK, szText );
		}
		return true;
	}
	else if( FStrEq( cmd, "sell_tower" ) )
	{
		CBaseTower *pTower = ToBaseTower( GetSelected() );;
		if(pTower && pTower->IsBuilt() && pTower->GetOwnerEntity() == this)
		{
			CRecipientFilter filter;
			filter.AddRecipient( this );
			filter.MakeReliable();
			char szText[200];
			Q_snprintf( szText, sizeof(szText), "Tower sold for $%i.",pTower->Sell());
			UTIL_ClientPrintFilter( filter, HUD_PRINTTALK, szText );
		}
		return true;
		
	}
	else if( FStrEq( cmd, "commandermode" ) )
	{
		int moveMe = atoi(engine->Cmd_Argv(1));
		if(moveMe < 0 || moveMe > 2)
			return false;;

		SetCommanderMode( moveMe );
		return true;
	}

	return BaseClass::ClientCommand( cmd );
}

void CSDKPlayer::SetCommanderMode( int mode )
{
	if(mode == 0) //commandermode 0 (Toggle between freelook and freelook commander mode)
	{
		if(m_iCommanderMode == 1)
			m_iCommanderMode = 0;
		else if(m_iCommanderMode == 0)
			m_iCommanderMode = 1;
		else
			m_iCommanderMode = 0;
	}
	else 
		m_iCommanderMode = 2; //Enable old school RTS commander mode

	if(m_iCommanderMode == 1 || m_iCommanderMode == 2)
	{
		ShowViewPortPanel( "TowersCommand", true );;

		//If we are mode 2, snap to above our targetted tower or at default spawn (facing down)
		if(mode > 0)
		{
			CBaseTower *pTower = ToBaseTower( GetSelected() );
			if(pTower)
			{
				Vector orig = pTower->GetAbsOrigin();
				SetAbsOrigin( orig + Vector(0,0,2000) );
				SetAbsVelocity( Vector(0,0,0) );
			}
			else
			{
				CBaseEntity *pEnt = EntSelectSpawnPoint();
				Vector orig = pEnt->GetAbsOrigin();
				SetAbsOrigin( orig );
				SetAbsVelocity( Vector(0,0,0) );
			}
		}
	}
}