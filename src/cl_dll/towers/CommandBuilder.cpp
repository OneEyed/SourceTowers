#include "cbase.h"

#include <vgui/ISurface.h>
#include <vgui/IInput.h>
#include <vgui/IVgui.h>
#include "c_sdk_player.h"

#include "sdk_gamerules.h"
#include "towers/BaseTower.h"

#include "towers/CommandVGUI.h"
#include "towers/CommandBuilder.h"

CCommandBuilder::CCommandBuilder( vgui::Panel *parent, const char *panelName )
: BaseClass(parent, panelName)
{
	SetParent( parent );

	color32 tmp;
	tmp.r = 255;
	tmp.g = 255;
	tmp.b = 255;
	tmp.a = 255;

	m_Earth = new BuilderButton( parent, "EarthButton", "", EARTH_TOWER );
	m_Earth->SetCommand( "create_tower 0" );
	m_Earth->SetImage(CBitmapButton::BUTTON_ENABLED,"vgui/ST_Earth",tmp);

	m_Fire = new BuilderButton( parent, "FireButton", "", FIRE_TOWER);
	m_Fire->SetCommand( "create_tower 1" );
	m_Fire->SetImage(CBitmapButton::BUTTON_ENABLED,"vgui/ST_Fire",tmp);

	m_Ice = new BuilderButton( parent, "IceButton", "", ICE_TOWER);
	m_Ice->SetCommand( "create_tower 2" );
	m_Ice->SetImage(CBitmapButton::BUTTON_ENABLED,"vgui/ST_Water",tmp);

	m_Super = new BuilderButton( parent, "SuperButton", "", SUPER_TOWER);
	m_Super->SetCommand( "create_tower 3" );
	m_Super->SetImage(CBitmapButton::BUTTON_ENABLED,"vgui/ST_Super",tmp);
	
	m_Upgrade = new BuilderButton( parent, "UpgradeButton", "Upgrade", UPGRADE_TOWER );
	m_Upgrade->SetCommand( "upgrade_tower" );

	m_Sell = new BuilderButton( parent, "SellButton", "Sell", SELL_TOWER );
	m_Sell->SetCommand( "sell_tower" );

	ivgui()->AddTickSignal(GetVPanel(),100);
}

CCommandBuilder::~CCommandBuilder() 
{
	m_Earth->DeletePanel();
	m_Fire->DeletePanel();
	m_Ice->DeletePanel();
	m_Super->DeletePanel();

	m_Upgrade->DeletePanel();
	m_Sell->DeletePanel();
};


//Our parent panel has an image with empty space, 
//I used photoshop to see how much empty space was left, and cut it out
//using the ratio of our screensize and pixel size of empty space. ScreenWidth/640 and ScreenHeight/480
void CCommandBuilder::Reset()
{
	//SetMouseInputEnabled( false );;;

	SetSize( m_StartW, m_StartH );
	SetPos( m_StartX, m_StartY );

	int sw = ScreenWidth();
	int sh = ScreenHeight();
	float offsetW = (float)(sw/640.0f);
	float offsetH = (float)(sh/480.0f);

	int w = (int)(offsetW * 32.0f);
	int h = (int)(offsetH * 28.0f);

	int xoff = (int)(offsetW * 38.0f);
	int yoff = (int)(offsetH * 31.0f);

	m_Earth->SetPos( m_StartX, m_StartY );
	m_Earth->SetSize( w, h );
	m_Earth->SetVisible( true );
	m_Earth->SetFgColor( Color( 0,255,0,255 ) );
	m_Earth->SetBgColor( Color( 0,0, 255,0 ) );
	m_Earth->SetArmedColor( Color(0,0,255,255), Color(0,255,0,0) );
	m_Earth->SetMouseInputEnabled( true );

	m_Fire->SetPos( m_StartX+xoff, m_StartY );
	m_Fire->SetSize( w, h );
	m_Fire->SetVisible( true );
	m_Fire->SetFgColor( Color( 0,255,0,255 ) );
	m_Fire->SetBgColor( Color( 0,0, 255,0 ) );
	m_Fire->SetArmedColor( Color(0,0,255,255), Color(0,255,0,0) );
	m_Fire->SetMouseInputEnabled( true );

	m_Ice->SetPos( m_StartX+(xoff*2), m_StartY );
	m_Ice->SetSize( w, h );
	m_Ice->SetVisible( true );
	m_Ice->SetFgColor( Color( 0,255,0,255 ) );
	m_Ice->SetBgColor( Color( 0,0, 255,0 ) );
	m_Ice->SetArmedColor( Color(0,0,255,255), Color(0,255,0,0) );
	m_Ice->SetMouseInputEnabled( true );

	m_Super->SetPos( m_StartX+(xoff), m_StartY+(yoff) );
	m_Super->SetSize( w, h );
	m_Super->SetVisible( true );
	m_Super->SetFgColor( Color( 0,255,0,255 ) );
	m_Super->SetBgColor( Color( 0,0, 255,0 ) );
	m_Super->SetArmedColor( Color(0,0,255,255), Color(0,255,0,0) );
	m_Super->SetMouseInputEnabled( true );

	m_Upgrade->SetPos( m_StartX+xoff, m_StartY+(yoff*2) );
	m_Upgrade->SetSize( w, h);
	m_Upgrade->SetVisible( true );
	m_Upgrade->SetFgColor( Color( 255,0,0,255 ) );
	m_Upgrade->SetBgColor( Color( 0,0, 255,0 ) );
	m_Upgrade->SetArmedColor( Color(0,0,255,255), Color(255,0,0,0) );
	m_Upgrade->SetMouseInputEnabled( true );

	m_Sell->SetPos( m_StartX+(xoff*2), m_StartY+(yoff*2) );
	m_Sell->SetSize( w, h);
	m_Sell->SetVisible( true );
	m_Sell->SetFgColor( Color( 255,0,0,255 ) );
	m_Sell->SetBgColor( Color( 0,0, 255,0 ) );
	m_Sell->SetArmedColor( Color(0,0,255,255), Color(255,0,0,0) );
	m_Sell->SetMouseInputEnabled( true );


	int infox = (sw - ((sw - m_StartX)/2)) - 50;
	int infoy = m_StartY - 50;
	m_Earth->m_pInfo->SetPos( infox, infoy );
	m_Fire->m_pInfo->SetPos( infox, infoy );
	m_Ice->m_pInfo->SetPos( infox, infoy );
	m_Super->m_pInfo->SetPos( infox, infoy );

	m_Upgrade->m_pInfo->SetPos( infox, infoy );
	m_Sell->m_pInfo->SetPos( infox, infoy );

	SetPaintBackgroundEnabled(false);
	SetBgColor(Color(0,0,0,0));
}

void CCommandBuilder::ApplySchemeSettings( vgui::IScheme *pScheme ) 
{
	m_Font8 = pScheme->GetFont( "MKSBadaboom12" );;;

	m_Earth->SetFont( m_Font8 );
	m_Fire->SetFont( m_Font8 );
	m_Ice->SetFont( m_Font8 );
	m_Super->SetFont( m_Font8 );

	m_Upgrade->SetFont( m_Font8 );
	m_Sell->SetFont( m_Font8 );

	BaseClass::ApplySchemeSettings( pScheme );
}

void CCommand::OnCommand(const char *cmd) 
{
	if ( strnicmp( cmd, "create_tower", 12) == 0 )
	{
		engine->ClientCmd( cmd );
	}
	else if( stricmp( cmd, "upgrade_tower" ) == 0 )
	{
		engine->ClientCmd( cmd );
	}
	else if( stricmp( cmd, "sell_tower" ) == 0 )
	{
		engine->ClientCmd( cmd );
	}
	else if( stricmp( cmd, "startgame" ) == 0 )
	{
		engine->ClientCmd( cmd );
	}
	else if( stricmp( cmd, "pause" ) == 0 )
	{
		engine->ClientCmd( cmd );
	}
	BaseClass::OnCommand( cmd );
}

void CCommandBuilder::OnTick()
{
	if(!C_BasePlayer::GetLocalPlayer())
		return;

	C_SDKPlayer *sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if(!sdkPlayer)
		return;

	CBaseTower *pTower = ToBaseTower( sdkPlayer->GetSelected() );
	if(!pTower || !pTower->m_bBuilt)
	{
		m_Upgrade->SetVisible( false );
		m_Sell->SetVisible( false );
		return;
	}

	m_Upgrade->SetVisible(!pTower->IsMaxLevel());
	m_Sell->SetVisible(true);
}

bool CCommandBuilder::ShouldAllowMouseClick() 
{ 
	int cx, cy;
	input()->GetCursorPos( cx, cy );
	int leftside = m_StartX + m_StartW;
	int bottom = m_StartY + m_StartH;
	if( cx >= m_StartX && cx <= leftside && cy >= m_StartY && cy <= bottom )
		return false;
	return true; 
};

BuilderButton::BuilderButton(vgui::Panel *parent, const char *PanelName, const char *text, int towerid)
: BaseClass(parent, PanelName, text)
{
	m_pInfo = new BuilderButtonInfo( parent, towerid );
	m_pInfo->SetVisible(false);
	SetReleasedSound("UI/buttonclickrelease.wav");
}

BuilderButton::~BuilderButton( void )
{
	m_pInfo->DeletePanel();
}

extern ConVar tower_sellpercent;

void BuilderButton::OnCursorEntered( void )
{
	HCursor blah = surface()->CreateCursorFromFile( "materials/overlay/cursor.ani" );
	SetCursor( blah );

	int towerid = m_pInfo->m_iTowerID;
	if(towerid > UPGRADE_TOWER)
	{
		char price[32];
		m_pInfo->m_Price->GetText( price, sizeof(price) );
		if(!price[0] && (towerid >= 0 && towerid < MAX_TOWERS))
		{
			Q_snprintf( price, sizeof( price ), "Price: $%i", SDKGameRules()->m_pTowerInfo[towerid].upgradecost );
			m_pInfo->m_TowerName->SetText( g_szTowerNames[towerid] );
			m_pInfo->m_Price->SetText( price );
		}
	}
	else if(towerid == UPGRADE_TOWER)
	{
		if(C_BasePlayer::GetLocalPlayer())
		{
			C_SDKPlayer *localPlayer = C_SDKPlayer::GetLocalSDKPlayer();;
			if(localPlayer)
			{
				C_BaseTower *pTower = ToBaseTower( localPlayer->GetSelected() );
				if(pTower)
				{ 
					m_pInfo->m_TowerName->SetText( "UPGRADE" );

					char info[32];
					int cost = pTower->m_iUpgradeCost;
					if(cost == -1)
						Q_snprintf( info, sizeof( info ), "MAXED" );
					else	
						Q_snprintf( info, sizeof( info ), "Cost: $%i", cost );
					m_pInfo->m_Price->SetText(info);
				}
			}
		}
	}
	else if(towerid == SELL_TOWER)
	{
		if(C_BasePlayer::GetLocalPlayer())
		{
			C_SDKPlayer *localPlayer = C_SDKPlayer::GetLocalSDKPlayer();;
			if(localPlayer)
			{
				C_BaseTower *pTower = ToBaseTower( localPlayer->GetSelected() );
				if(pTower)
				{ 
					m_pInfo->m_TowerName->SetText( "SELL" );

					char info[32];
					int cost = (int)( (float)pTower->m_iUpgradeCost * tower_sellpercent.GetFloat() );
					if(cost < 0)
						Q_snprintf( info, sizeof( info ), "MAXED" );
					else	
						Q_snprintf( info, sizeof( info ), "Cost: $%i", cost );
					m_pInfo->m_Price->SetText(info);
				}
			}
		}
	}
	m_pInfo->SetVisible(true);;
	m_pInfo->MoveToFront();
}

void BuilderButton::OnCursorExited( void )
{
	m_pInfo->SetVisible(false);
}

void BuilderButton::SetInfo( const char *szTowerName, const char *szPrice )
{
	HFont font = GetFont();

	m_pInfo->SetInfo( font, szTowerName, szPrice );
}

BuilderButtonInfo::BuilderButtonInfo(vgui::Panel *parent, int towerid)
{
	SetParent( parent );
	SetSize( 100, 40 );
	SetPos( 0, 0 );

	m_iTowerID = towerid;

	m_TowerName = new vgui::Label( this, "TowerName", "" );
	m_Price = new vgui::Label( this, "TowerPrice", "" );

	m_TowerName->SetPos( 0, 0 );
	m_TowerName->SetSize( 100, 15 );
	m_TowerName->SetFgColor( Color( 0, 255, 255, 255 ) ); 
	m_TowerName->SetBgColor( Color(0,0,0,255) );
	m_TowerName->SetVisible( true );

	m_Price->SetPos( 0, 16 );
	m_Price->SetSize( 100, 15 );
	m_Price->SetFgColor( Color( 0, 255, 255, 255 ) ); 
	m_Price->SetBgColor( Color(0,0,0,255) );
	m_Price->SetVisible( true );

	SetBgColor( Color(0,0,0,255) );
}

BuilderButtonInfo::~BuilderButtonInfo()
{
	m_TowerName->DeletePanel();
	m_Price->DeletePanel();
}

void BuilderButtonInfo::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	SetBorder( pScheme->GetBorder( "ToolTipBorder" ) );
}
void BuilderButtonInfo::Reset()
{
	SetSize( 100, 40 );

	m_TowerName->SetPos( 0, 0 );
	m_TowerName->SetSize( 100, 15 );
	m_TowerName->SetFgColor( Color( 0, 255, 255, 255 ) ); 
	m_TowerName->SetBgColor( Color(0,0,0,255) );
	m_TowerName->SetVisible( true );

	m_Price->SetPos( 0, 16 );
	m_Price->SetSize( 100, 15 );
	m_Price->SetFgColor( Color( 0, 255, 255, 255 ) ); 
	m_Price->SetBgColor( Color(0,0,0,255) );
	m_Price->SetVisible( true );

	//SetBgColor( Color(0,0,0,255) );
	
}

void BuilderButtonInfo::SetInfo( HFont font, const char *szTowerName, const char *szPrice )
{
	SetBgColor( Color(0,0,0,255) );;;;;
	SetPos( 0, 0 );
	m_TowerName->SetFont( font );
	m_Price->SetFont( font );

	m_TowerName->SetPos( 10, 0 );
	m_Price->SetPos( 15, 16 );

	m_TowerName->SetFgColor( Color( 0, 255, 255, 255 ) ); 
	m_Price->SetFgColor( Color( 0, 255, 255, 255 ) ); 

	m_TowerName->SetText( szTowerName );
	m_Price->SetText( szPrice );
}