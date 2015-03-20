#include "cbase.h"
#include "BaseClickable.h"
#include "engine/ivdebugoverlay.h"

#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
#else
	#include "sdk_player.h"
#endif


IMPLEMENT_NETWORKCLASS_ALIASED( BaseClickable, DT_BaseClickable )

#ifdef GAME_DLL
//Copied from CBasePlayer, allowing 11 bits instead of 9
void SendProxy_CropFlagsBitLength( const SendProp *pProp, const void *pStruct, const void *pVarData, DVariant *pOut, int iElement, int objectID)
{
	int mask = (1<<11) - 1;
	int data = *(int *)pVarData;
	pOut->m_Int = ( data & mask );
}
#endif

BEGIN_NETWORK_TABLE( CBaseClickable, DT_BaseClickable )
#ifdef CLIENT_DLL 
	RecvPropInt( RECVINFO( m_iHealth ) ),
	RecvPropInt( RECVINFO( m_iMaxHealth ) ),
	RecvPropInt( RECVINFO( m_fFlags ) ),
#else
	SendPropInt( SENDINFO( m_iHealth ) ),
	SendPropInt( SENDINFO( m_iMaxHealth ) ),
	SendPropInt( SENDINFO(m_fFlags), 11, SPROP_UNSIGNED, SendProxy_CropFlagsBitLength ),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( base_clickable, CBaseClickable );

CUtlVector< CBaseClickable* > g_pClickables;

CBaseClickable::CBaseClickable()
{
	m_iClickID = g_pClickables.AddToTail( this );
#ifdef CLIENT_DLL
	m_bFakeOrigin = false;
	m_vecFakeOrigin.Init();
#endif
	m_bMarkedForDeletion = false;
	
}

CBaseClickable::~CBaseClickable()
{
	int i;
	int clients = gpGlobals->maxClients;
	int count;
	for (i = 1; i <= clients; i++ )
	{
		CSDKPlayer *sdkPlayer = (CSDKPlayer*)UTIL_PlayerByIndex( i );

		if (! sdkPlayer  )
			continue;

		count = sdkPlayer->m_pSelectedTargets.Count();
		sdkPlayer->m_pSelectedTargets.FindAndRemove( GetRefEHandle() );
		//for(x=count-1; x>=0; x--)
		//{
		//	if( sdkPlayer->m_pSelectedTargets[x].Get() == this )
		//		sdkPlayer->m_pSelectedTargets.Remove(x);
		//}
	}

	g_pClickables.FindAndRemove( this );
}

void CBaseClickable::Spawn( void )
{
	AddFlag( FL_IS_SELECTABLE );
/*
#ifdef CLIENT_DLL
	int iSerialNum = GetRefEHandle().GetSerialNumber() & (1 << NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS) - 1;
	int num = GetRefEHandle().GetEntryIndex() | (iSerialNum << MAX_EDICT_BITS);
		engine->Con_NPrintf( 25, "CLIENT = %i", num );
#else
	int iSerialNum = GetRefEHandle().GetSerialNumber() & (1 << NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS) - 1;
	int num = GetRefEHandle().GetEntryIndex() | (iSerialNum << MAX_EDICT_BITS);
	engine->Con_NPrintf( 27, "SERVER = %i", num );
#endif
*/
}

//--------------------------------------------
//Hover Forwards
//--------------------------------------------
void CBaseClickable::OnHoverEnter( CSDKPlayer *sdkPlayer ) 
{
	sdkPlayer->m_hHovered = this;
}
void CBaseClickable::OnHoverExit( CSDKPlayer *sdkPlayer ) 
{
	sdkPlayer->m_hHovered = (CBaseClickable*)NULL;
}
void CBaseClickable::OnHover( CSDKPlayer *sdkPlayer ) 
{ 
	//sdkPlayer->m_hHovered = this;//m_iClickID; 
}

//--------------------------------------------
//Selected Forwards
//--------------------------------------------
void CBaseClickable::OnSelected( CSDKPlayer *sdkPlayer ) 
{ 
	//sdkPlayer->SetSelected( this );//m_hSelected = tm_iClickID; 
}

//Runs all the time when selected.
void CBaseClickable::Selected( CSDKPlayer *sdkPlayer )
{

}
//--------------------------------------------
//Hover and Selected player checks
//--------------------------------------------
bool CBaseClickable::IsHovered( CSDKPlayer *sdkPlayer )
{
	return (sdkPlayer->m_hHovered == this );
	//int index = sdkPlayer->m_iHoveredEntIndex;
	//return IsIndex(index);
}
bool CBaseClickable::IsSelected( CSDKPlayer *sdkPlayer )
{
	return (sdkPlayer->GetSelected() == this );
	//int index = sdkPlayer->m_iSelectedEntIndex;
	//return IsIndex(index);
}

//--------------------------------------------
//Is index of this clickable?
//--------------------------------------------
inline bool CBaseClickable::IsIndex( int index )
{
	return (index == m_iClickID);
}

#ifdef CLIENT_DLL
const Vector& CBaseClickable::GetRenderOrigin( void )
{
	if(m_bFakeOrigin)
		return GetFakeOrigin();
	return GetAbsOrigin();
}
void CBaseClickable::DrawBoxOverlay( Color color )
{
	int r,g,b,a;
	color.GetColor( r,g,b,a );
	debugoverlay->AddBoxOverlay(GetRenderOrigin(), WorldAlignMins(), WorldAlignMaxs(), QAngle(0,0,0), r, g, b, a, -1);
}
#else

#endif