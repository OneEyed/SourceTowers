#include "cbase.h"
//#include "CommandVGUI.h"
#include "towers/CommandInfo.h"
#include <vgui/ISurface.h>
#include "c_sdk_player.h"
#include "sdk_gamerules.h"
#include "towers/BaseTower.h"

CCommandInfo::CCommandInfo( vgui::Panel *parent, const char *panelName ): Panel(parent, panelName)
{
	SetParent( parent );

	m_pStartPause = new vgui::Button( parent, "StartGame", "Start" );
	m_pStartPause->SetCommand( "startgame" );

	m_Money = new vgui::Label( this, "MoneyLabel", "" );
	m_Level = new vgui::Label( this, "LevelLabel", "" );
	m_CastleLives = new vgui::Label( this, "LivesLabel", "" );
	//m_TowerStats = new vgui::Label( this, "StatsLabel", "" );

	m_TowerName = new vgui::Label( this, "StatsLabel", "" );
	m_TowerDamage = new vgui::Label( this, "StatsLabel", "" );
	m_TowerRadius = new vgui::Label( this, "StatsLabel", "" );
	m_TowerSpeed = new vgui::Label( this, "StatsLabel", "" );
	m_TowerDelay = new vgui::Label( this, "StatsLabel", "" );
	m_TowerLevel = new vgui::Label( this, "StatsLabel", "" );

	//Reset();
}

CCommandInfo::~CCommandInfo() 
{
	m_pStartPause->DeletePanel();

	m_Money->DeletePanel();
	m_Level->DeletePanel();
	m_CastleLives->DeletePanel();
	m_TowerName->DeletePanel();
	m_TowerDamage->DeletePanel();
	m_TowerRadius->DeletePanel();
	m_TowerSpeed->DeletePanel();
	m_TowerDelay->DeletePanel();
	m_TowerLevel->DeletePanel();
};

void CCommandInfo::VidInit()
{
	//Reset();
}
//Our parent panel has an image with empty space, 
//I used photoshop to see how much empty space was left, and cut it out
//using the ratio of our screensize and pixel size of empty space. ScreenWidth/640 and ScreenHeight/480
void CCommandInfo::Reset()
{
	SetMouseInputEnabled( false );;
	/*
	int width, height;
	int w, h;

	GetParent()->GetSize( width, height );

	int sw = ScreenWidth();
	int sh = ScreenHeight();
	float offsetW = (float)(sw/640.0f);
	float offsetH = (float)(sh/480.0f);

	w = ((int)(offsetW * 68.0f));;;;;
	h = ((int)(offsetH * 60.0f));
	
	SetSize( width-w, height-h );
	SetPos( w, h );
	*/

	SetSize( m_StartW, m_StartH );
	SetPos( m_StartX, m_StartY );

	int sw = ScreenWidth();
	int sh = ScreenHeight();;;
	float offsetW = (float)(sw/640.0f);
	float offsetH = (float)(sh/480.0f);

	int w = (int)(offsetW * 50.0f);
	int h = (int)(offsetH * 15.0f);

	m_pStartPause->SetSize( w, h );;
	m_pStartPause->SetPos( m_StartX+5, m_StartY+60 );
	m_pStartPause->SetFgColor( Color( 0,255,0,255 ) );
	m_pStartPause->SetBgColor( Color( 0,0, 255,0 ) );
	m_pStartPause->SetArmedColor( Color(255,210,0,255), Color(255,0,0,0) );
	m_pStartPause->SetVisible( true );

	m_Level->SetPos( 5, 5 );;;;;;;
	m_Level->SetSize( 200, 15 );
	m_Level->SetFgColor( Color( 0, 255, 0, 255 ) ); 
	m_Level->SetVisible( true );

	m_Money->SetSize( 200, 15 );
	m_Money->SetPos( 5, 20 );
	m_Money->SetFgColor( Color( 0, 255, 0, 255 ) ); 
	m_Money->SetVisible( true );

	m_CastleLives->SetSize( 200, 15 );
	m_CastleLives->SetPos( 5, 35 );
	m_CastleLives->SetFgColor( Color( 0, 255, 0, 255 ) ); 
	m_CastleLives->SetVisible( true );

	m_TowerName->SetSize( 200, 15 );;;;
	m_TowerName->SetPos( 150, 5 );
	m_TowerName->SetFgColor( Color( 255, 255, 0, 255 ) ); 
	m_TowerName->SetVisible( true );

	m_TowerLevel->SetSize( 200, 15 );;;;
	m_TowerLevel->SetPos( 150, 25 );
	m_TowerLevel->SetFgColor( Color( 255, 255, 0, 255 ) ); 
	m_TowerLevel->SetVisible( true );

	m_TowerDamage->SetSize( 200, 15 );;;
	m_TowerDamage->SetPos( 150, 45 );
	m_TowerDamage->SetFgColor( Color( 255, 255, 0, 255 ) ); 
	m_TowerDamage->SetVisible( true );

	m_TowerSpeed->SetSize( 200, 15 );;;
	m_TowerSpeed->SetPos( 150, 65 );
	m_TowerSpeed->SetFgColor( Color( 255, 255, 0, 255 ) ); 
	m_TowerSpeed->SetVisible( true );

	m_TowerRadius->SetSize( 200, 15 );;;
	m_TowerRadius->SetPos( 150, 85 );
	m_TowerRadius->SetFgColor( Color( 255, 255, 0, 255 ) ); 
	m_TowerRadius->SetVisible( true );

	SetPaintBackgroundEnabled(false);
	SetBgColor(Color(0,0,0,0));
}

#define MAX_ID_STRING 256
void CCommandInfo::Paint()
{
	
}

void CCommandInfo::OnThink()
{
	if(!C_BasePlayer::GetLocalPlayer())
		return;

	C_SDKPlayer *sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();// C_BasePlayer::GetLocalPlayer() );
	if(!sdkPlayer)
		return;

	char buffer[32];;
	CSDKGameRules *pRules = SDKGameRules();
	if(!pRules)
		return;

	if(!pRules->m_bGameStarted)
	{
		m_pStartPause->SetText( "Start" );
		m_pStartPause->SetCommand( "startgame" );
	}
	else
	{
		m_pStartPause->SetText( "Pause" );
		m_pStartPause->SetCommand( "pause" );
	}

	Q_snprintf( buffer, 31, "Money: $%i", sdkPlayer->m_iMoney );;
	m_Money->SetText( buffer );

	Q_snprintf( buffer, 31, "Wave: %i", sdkPlayer->m_iLevel );
	m_Level->SetText( buffer );

	Q_snprintf( buffer, 31, "Castle Lives: %i", pRules->m_iLives );
	m_CastleLives->SetText( buffer );
	

	CBaseTower *pTower = ToBaseTower( sdkPlayer->GetSelected() );
	if(!pTower)
	{
		RemoveInfoText();
		return;

	}

	int type = pTower->m_iTowerType;

	Q_snprintf( buffer, 31, "%s", g_szTowerNames[type] );;
	m_TowerName->SetText( buffer );

	Q_snprintf( buffer, 31, "Level: %i", pTower->m_iLevel );
	m_TowerLevel->SetText( buffer );

	Q_snprintf( buffer, 31, "Damage: %i", pTower->m_iDamage );
	m_TowerDamage->SetText( buffer );

	Q_snprintf( buffer, 31, "Speed: %i", pTower->m_iSpeed );
	m_TowerSpeed->SetText( buffer );

	Q_snprintf( buffer, 31, "Radius: %i", pTower->m_iRadius );
	m_TowerRadius->SetText( buffer );
}

void CCommandInfo::RemoveInfoText()
{
	m_TowerName->SetText("");
	m_TowerLevel->SetText("");
	m_TowerDamage->SetText("");
	m_TowerSpeed->SetText("");
	m_TowerRadius->SetText("");
}