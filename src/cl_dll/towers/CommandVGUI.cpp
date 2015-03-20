//======================================================================\\
// MKS-OneEyed: Displays the Character Selection screen.
// TODO:
//======================================================================//
#include "cbase.h"

#include <vgui_controls/ImagePanel.h>
#include "towers/CommandVGUI.h"
#include "towers/CommandInfo.h"
#include "towers/CommandBuilder.h"

#include <vgui/IInput.h>
#include "c_sdk_player.h"
#include "towers/BaseTower.h"
#include "kbutton.h"
#include "in_buttons.h"
#include "iclientmode.h"
#include <vgui/ISurface.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar command_moveup( "command_moveup", "500" );
ConVar command_screenmove( "command_screenmove", "1000" );
ConVar command_screenturn( "command_screenturn", "75" );

CCommand *g_CommandPanel = NULL;
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCommand::CCommand(IViewPort *pViewPort): Frame(NULL, PANEL_TOWERSCOMMAND)
{
	m_pViewPort = pViewPort;

	SetPos(	0, 0 );
	int width, height;
	width = ScreenWidth();
	height = ScreenHeight();

	SetSize( width, height );

	// load the new scheme early!!
	SetScheme("ClientScheme");
	SetProportional(true);
	
	m_pCommandInfo = new CCommandInfo( this, "CommandInfo" );
	m_pCommandBuild = new CCommandBuilder( this, "CommandBuild" );

	SetPaintBackgroundEnabled(false);
	SetBgColor(Color(0,0,0,0));

	g_CommandPanel = this;

	m_flUpMove = 0.0f;
	m_iButtons = 0;
	m_iMouseMove = 0;
	m_bMultiSelectStep = 0;
	m_bDrawRect = false;
	m_bDrawEnemyHealth = true;
	m_vStart.Init();
	m_vEnd.Init();
	m_sdkPlayer = NULL;
	InvalidateLayout();

	Reset();
}

//Resize and reset everything for when we change resolutions!
//This function seems to be the best place =/
void CCommand::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	//Reset();
	SetKeyBoardInputEnabled(false);
}

void CCommand::OnScreenSizeChanged( int iOldWide, int iOldTall )
{
	Reset();
}



void CCommand::VidInit()
{
	Reset();
	m_sdkPlayer = NULL; //So we reget our player pointer.
}
void CCommand::Reset()
{
	int width, height;
	int w, h;
	int startx, starty;
	int startw, starth;
	float offsetW, offsetH;

	width = ScreenWidth();
	height = ScreenHeight();

	SetSize( width, height );
	SetPos(	0, 0 );

	// load the new scheme early!!
	SetScheme("ClientScheme");
	SetProportional(true);
	SetPaintBackgroundEnabled(false);
	SetBgColor(Color(0,0,0,0));

	offsetW = ((float)width) / 640.0f;
	offsetH = ((float)height) / 480.0f;
	w = (int)(offsetW * 512.0f);
	h = (int)(offsetH * 128.0f);
	
	//Offset our panels to be in-line with our HUD interface
	startx = (width-w)+(int)(offsetW * 198.0f);
	starty = (height-h)+(int)(offsetH * 60.0f);
	startw = (int)(offsetW * 186.0f);
	starth = (int)(offsetH * 69.0f);
	m_pCommandInfo->SetOffsetPos( startx, starty, startw, starth );

	startx = (width-w)+(int)(offsetW * 402.0f);
	starty = (height-h)+(int)(offsetH * 36.0f);
	startw = (int)(offsetW * 108.0f);
	starth = (int)(offsetH * 88.0f);
	m_pCommandBuild->SetOffsetPos( startx, starty, startw, starth );

	if(m_pCommandInfo)
		m_pCommandInfo->Reset();
	if(m_pCommandBuild)
		m_pCommandBuild->Reset();

	HCursor blah = surface()->CreateCursorFromFile( "materials/overlay/cursor.ani" );
	SetCursor( blah );
}

CCommand::~CCommand()
{
	m_pCommandInfo->DeletePanel();
	m_pCommandBuild->DeletePanel();
}

void CCommand::Paint()
{
	if(m_bDrawRect)
	{
		DrawSelectionBox();
	}

	if(m_bDrawEnemyHealth)
	{
		DrawEnemyHealth();
	}
}

void CCommand::DrawEnemyHealth(void)
{
	int i;
	for(i=0;i<m_pEnemyHealth.Count();i++)
	{
		if(!m_pEnemyHealth[i].bEnabled)
			continue;
		float pct = m_pEnemyHealth[i].pct;;
		int green = (int)(510.0f*pct);
		int red = (int)(510.0f*(1.0f-pct));

		if(green > 255)
			green = 255;
		if(red > 255)
			red = 255;

		surface()->DrawSetColor(red, green, 0, 175);;;;
		surface()->DrawFilledRect(m_pEnemyHealth[i].topLeft.x, m_pEnemyHealth[i].topLeft.y, m_pEnemyHealth[i].bottomRight.x, m_pEnemyHealth[i].bottomRight.y);

		surface()->DrawSetColor(255, 255, 255, 175);;
		surface()->DrawLine(m_pEnemyHealth[i].topLeft.x,	m_pEnemyHealth[i].topLeft.y,	m_pEnemyHealth[i].topLeft.x+40,	m_pEnemyHealth[i].topLeft.y);;
		surface()->DrawLine(m_pEnemyHealth[i].topLeft.x+40,	m_pEnemyHealth[i].topLeft.y,	m_pEnemyHealth[i].topLeft.x+40,	m_pEnemyHealth[i].topLeft.y+10);
		surface()->DrawLine(m_pEnemyHealth[i].topLeft.x+40,	m_pEnemyHealth[i].topLeft.y+10,	m_pEnemyHealth[i].topLeft.x,	m_pEnemyHealth[i].topLeft.y+10);
		surface()->DrawLine(m_pEnemyHealth[i].topLeft.x,	m_pEnemyHealth[i].topLeft.y+10,	m_pEnemyHealth[i].topLeft.x,	m_pEnemyHealth[i].topLeft.y);
	}
}

void CCommand::DrawSelectionBox(void)
{
	surface()->DrawSetColor(0, 255, 255, 255);;

	//LINE ORDER: TopLeft -> TopRight -> BottomRight -> BottomLeft -> TopLeft
	surface()->DrawLine(m_vStart.x, m_vStart.y, m_vEnd.x, m_vStart.y);;
	surface()->DrawLine(m_vEnd.x, m_vStart.y, m_vEnd.x, m_vEnd.y);
	surface()->DrawLine(m_vEnd.x, m_vEnd.y, m_vStart.x, m_vEnd.y);
	surface()->DrawLine(m_vStart.x, m_vEnd.y, m_vStart.x, m_vStart.y);

	Vector2D topleft, bottomright;;;
	if(m_vStart.x < m_vEnd.x)
	{
		if(m_vStart.y < m_vEnd.y)
		{	
			topleft = m_vStart;			
			bottomright = m_vEnd;	
		}
		else					
		{	
			topleft.x = m_vStart.x;		
			topleft.y = m_vEnd.y;	
			bottomright.x = m_vEnd.x;
			bottomright.y = m_vStart.y;
		}
	}
	else
	{
		if(m_vEnd.y < m_vStart.y)
		{	
			topleft = m_vEnd;			
			bottomright = m_vStart;	
		}
		else					
		{	
			topleft.x = m_vEnd.x;		
			topleft.y = m_vStart.y;	
			bottomright.x = m_vStart.x;
			bottomright.y = m_vEnd.y;
		}
	}

	surface()->DrawSetColor(0, 0, 50, 25);;
	surface()->DrawFilledRect(topleft.x, topleft.y, bottomright.x, bottomright.y);
}
//------------------------------------------------------
// Purpose: Control mouse screen movements and selection box points.
//------------------------------------------------------
void CCommand::OnThink()
{
	//vgui::Frame doesn't handle resolution changes. SO HANDLE IT HERE!
	int sw, sh;
	surface()->GetScreenSize(sw, sh);
	if( sw != m_iScreenWidth && sh != m_iScreenHeight )
	{
		m_iScreenWidth = sw;
		m_iScreenHeight = sh;
		VidInit();
	}

	if(!C_BasePlayer::GetLocalPlayer())
		return;

	m_sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();
	if(!m_sdkPlayer)
		return;
	
	CBaseClickable *pEnt = m_sdkPlayer->m_hHovered.Get();
	EntHealth_t temp;

	//This sets the position for the health bars of enemy and which to draw.
	m_pEnemyHealth.RemoveAll();
	int i;
	for(i=0;i<g_pClickables.Count();i++)
	{
		pEnt = g_pClickables[i];
		if(pEnt && pEnt->GetClickType() == CLICK_UNIT && pEnt->GetCollisionGroup() == COLLISION_GROUP_TOWERS_ENEMY)
		{
			Vector origin = m_sdkPlayer->GetLocalOrigin();
			Vector2D topleft, bottomright, screenpos;
			int hp, maxhp;
			float pct;
			int x, y;;

			WorldToScreen( origin, pEnt->GetFakeOrigin(), x, y );;

			screenpos = Vector2D( x, y );
			hp = pEnt->GetHealth();
			maxhp = pEnt->GetMaxHealth();
			pct = ((float)hp / (float)maxhp);

			topleft = screenpos - Vector2D( 20, 5 );
			bottomright = Vector2D( topleft.x + (int)(40.0f*pct), screenpos.y + 5 );
			
			temp.topLeft = topleft;
			temp.bottomRight = bottomright;
			temp.pct = pct;
			temp.bEnabled = true;
			m_pEnemyHealth.AddToTail( temp );
		}
	}


	if(m_sdkPlayer->m_iCommanderMode == 0)
	{
		SetMouseInputEnabled( false );
		m_bDrawRect = false;
		m_bMultiSelectStep = 0;
		m_iButtons = 0;
		return;
	}

	SetMouseInputEnabled( true );

	int cx, cy;
	int clipRect[4];

	input()->GetCursorPos( cx, cy );
	ipanel()->GetClipRect(GetVPanel(), clipRect[0], clipRect[1], clipRect[2], clipRect[3]);;

	bool left	= (cx <= clipRect[0]+5);
	bool up		= (cy <= clipRect[1]+5);;
	bool right	= (cx >= clipRect[2]-5);
	bool down	= (cy >= clipRect[3]-5);

	if( left )		m_iMouseMove |= IN_MOVELEFT;
	else			m_iMouseMove &= ~IN_MOVELEFT;

	if( up )		m_iMouseMove |= IN_FORWARD;
	else			m_iMouseMove &= ~IN_FORWARD;

	if( right )		m_iMouseMove |= IN_MOVERIGHT;
	else			m_iMouseMove &= ~IN_MOVERIGHT;

	if( down )		m_iMouseMove |= IN_BACK;
	else			m_iMouseMove &= ~IN_BACK;
	
	//Multiple Selection Testing.
	//if(!left && !right && !up && !down)
	CreateSelectionBox(m_sdkPlayer, cx, cy);;
	
	bool leftbutton = input()->IsMouseDown(MOUSE_LEFT);
	bool rightbutton = input()->IsMouseDown(MOUSE_RIGHT);

	if(leftbutton)		m_iButtons |= IN_ATTACK;
	else				m_iButtons &= ~IN_ATTACK;

	if(rightbutton)		m_iButtons |= IN_ATTACK2;
	else				m_iButtons &= ~IN_ATTACK2;

	//Get a direction in the world from our mouse location.
	Vector screen(cx , cy, 0 );;
	ScreenToWorld( screen, m_sdkPlayer->m_vMouseDirection );
}

//------------------------------------------------------
// Purpose: Mouse button stuff
//------------------------------------------------------
void CCommand::OnMousePressed(MouseCode code)
{
	//if(code == MOUSE_LEFT)		m_iButtons |= IN_ATTACK;
	//if(code == MOUSE_RIGHT)		m_iButtons |= IN_ATTACK2;
}
void CCommand::OnMouseReleased(MouseCode code)
{
	//if(code == MOUSE_LEFT)		m_iButtons &= ~IN_ATTACK;
	//if(code == MOUSE_RIGHT)		m_iButtons &= ~IN_ATTACK2;
}

void CCommand::OnMouseWheeled(int delta)
{
	if ( !GetParent() )
		return;

	m_flUpMove += delta * command_moveup.GetFloat();;;;
}

//------------------------------------------------------
// Purpose: Forward our movements and selections to server.
//------------------------------------------------------
void CCommand::CreateMove( float flInputSampleTime, CUserCmd *cmd )
{
	if(!C_BasePlayer::GetLocalPlayer())
			return;

		m_sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();
		if(!m_sdkPlayer)
			return;
	//if(!m_sdkPlayer)
	//	return;

	int mode = m_sdkPlayer->m_iCommanderMode;

	QAngle viewangles;
	QAngle angle;
	engine->GetViewAngles( viewangles );

	//Allow regular mousemovement if mode is 0
	if(mode == 0)
	{
		Vector dir;
		AngleVectors( viewangles, &dir );
		m_sdkPlayer->m_vMouseDirection = dir;
		cmd->mousedirection = viewangles;
		return;
	}

	//Update our screen mouse movements
	VectorAngles( m_sdkPlayer->m_vMouseDirection, angle );
	//angle.z = 0;
	cmd->mousedirection = angle;//m_sdkPlayer->m_vMouseDirection;;;
	cmd->buttons |= m_iButtons;
	
	//Don't forward our clicks to server if we are over our builder panel!!!!!!
	if( !m_pCommandBuild->ShouldAllowMouseClick() )
		cmd->buttons &= ~IN_ATTACK;

	//Handle Turning/Movement with mouse to screen edges
	const float frametime = gpGlobals->frametime;
	const float turnspeed = command_screenturn.GetFloat() * frametime;
	const float screenmove = command_screenmove.GetFloat();
	
	if(m_iMouseMove & IN_FORWARD)
	{
		if(mode == 1)	viewangles.x -= turnspeed;
		else			cmd->forwardmove += screenmove;
	}
	if( m_iMouseMove & IN_BACK)
	{
		if(mode == 1)	viewangles.x += turnspeed;
		else			cmd->forwardmove -= screenmove;
	}
	if(m_iMouseMove & IN_MOVELEFT)
	{
		if(mode == 1)	viewangles.y += turnspeed;
		else			cmd->sidemove -= screenmove;
	}
	if(m_iMouseMove & IN_MOVERIGHT)
	{
		if(mode == 1)	viewangles.y -= turnspeed;
		else			cmd->sidemove += screenmove;
	}

	if(mode == 1)
		cmd->viewangles = viewangles;
	else
	{
		cmd->viewangles = QAngle(90,90,0);
		engine->SetViewAngles( QAngle(90,90,0) );;
	}

	//Handle up/down movement for mouse scroll.
	if(mode == 1)
		cmd->upmove = -m_flUpMove;
	else
		cmd->upmove = m_flUpMove;

	//Reduce our up movement slowly
	if(fabs(m_flUpMove) > 30.0f)	m_flUpMove *= 0.9f;
	else							m_flUpMove = 0;

}

//------------------------------------------------------
// Purpose: Things to do when panel is called.
//------------------------------------------------------
void CCommand::ShowPanel(bool bShow)
{
	if ( bShow )
		Activate();
	else
		SetVisible( false );

	m_pViewPort->ShowBackGround( false );;;
}