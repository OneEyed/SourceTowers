//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "hud.h"
#include "clientmode_sdk.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include "vgui/isurface.h"
#include "vgui/ipanel.h"
#include <vgui_controls/AnimationController.h>
#include "ivmodemanager.h"
#include "BuyMenu.h"
#include "filesystem.h"
#include "vgui/ivgui.h"
#include "keydefs.h"
#include "hud_chat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <keyvalues.h>
#include "text_message.h"
#include "panelmetaclassmgr.h"

#include "c_sdk_player.h"
#include "in_buttons.h"
#include "towers/CommandVGUI.h"
#include "hud_macros.h"
#include "towers/tower_shareddef.h"
#include "sdk_gamerules.h"

ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

IClientMode *g_pClientMode = NULL;


// --------------------------------------------------------------------------------- //
// CSDKModeManager.
// --------------------------------------------------------------------------------- //

class CSDKModeManager : public IVModeManager
{
public:
	virtual void	Init();
	virtual void	SwitchMode( bool commander, bool force ) {}
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	ActivateMouse( bool isactive ) {}
};

static CSDKModeManager g_ModeManager;
IVModeManager *modemanager = ( IVModeManager * )&g_ModeManager;

// --------------------------------------------------------------------------------- //
// CSDKModeManager implementation.
// --------------------------------------------------------------------------------- //

#define SCREEN_FILE		"scripts/vgui_screens.txt"

void CSDKModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();
	
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CSDKModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
	//g_CommandPanel->VidInit();
}

void CSDKModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeSDKNormal::ClientModeSDKNormal()
{
}

//-----------------------------------------------------------------------------
// Purpose: If you don't know what a destructor is by now, you are probably going to get fired
//-----------------------------------------------------------------------------
ClientModeSDKNormal::~ClientModeSDKNormal()
{
}


void ClientModeSDKNormal::InitViewport()
{
	m_pViewport = new SDKViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

ClientModeSDKNormal g_ClientModeNormal;

IClientMode *GetClientModeNormal()
{
	return &g_ClientModeNormal;
}


ClientModeSDKNormal* GetClientModeSDKNormal()
{
	Assert( dynamic_cast< ClientModeSDKNormal* >( GetClientModeNormal() ) );

	return static_cast< ClientModeSDKNormal* >( GetClientModeNormal() );
}

float ClientModeSDKNormal::GetViewModelFOV( void )
{
	return 74.0f;
}

int ClientModeSDKNormal::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

void ClientModeSDKNormal::PostRenderVGui()
{
}

//TOWERS CODE - Forward our cmds to our CommandVGUI so we can alter them (for mouse movements etc.)
//For our view, if we're in commandermode 2 , face it downwards instantly..
void ClientModeSDKNormal::CreateMove( float flInputSampleTime, CUserCmd *cmd )
{
	//Override to our CommanderVGUI panel.
	g_CommandPanel->CreateMove( flInputSampleTime, cmd );
	return;
}

void ClientModeSDKNormal::OverrideView( CViewSetup *pSetup )
{
	return;
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if(!player)
		return;

	C_SDKPlayer *pPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if(!pPlayer)
		return;

	if(pPlayer->m_iCommanderMode != 2)
		return;
	
	pSetup->origin = pPlayer->GetAbsOrigin();
	VectorCopy( QAngle( 90, 90, 0 ), pSetup->angles );
}

//Save data to gamerules object.
static void __MsgFunc_TowerPrices( bf_read &msg )
{
	//IViewPortPanel *viewport = gViewPortInterface->FindPanelByName( PANEL_TOWERSCOMMAND ); //Name of our build panel
	//if ( !viewport )
	//	return;

	int count = msg.ReadByte();
	if ( count > 0 )
	{
		//KeyValues *keys = new KeyValues("data");
		//char name[255];
		int data;

		for ( int i=0; i<count; i++)
		{
			//msg.ReadString( name, sizeof(name) );
			data = msg.ReadShort();
			
			SDKGameRules()->m_pTowerInfo[i].upgradecost = data;
			//keys->SetInt( name, data );
		}

		//viewport->SetData( keys );
		//keys->deleteThis();
	}
}

void ClientModeSDKNormal::Init()
{
	BaseClass::Init();

	HOOK_MESSAGE( TowerPrices );
}