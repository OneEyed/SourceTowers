//======================================================================\\
// MKS-OneEyed: HUD item selection.
// TODO:
//======================================================================//

#include "cbase.h"
#include "c_sdk_player.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>


using namespace vgui;

#define COMMANDUI "command"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CCommandHUD : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE( CCommandHUD, Panel );

public:
	CCommandHUD( const char *pElementName );
	~CCommandHUD();

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	
	virtual void	ApplySchemeSettings(IScheme *pScheme);
	virtual void OnThink();
	
protected: 
	virtual void Paint();
	
private:

	ImagePanel *m_CommandUI;

};	

extern CHud gHUD;

DECLARE_HUDELEMENT( CCommandHUD );

CCommandHUD::~CCommandHUD()
{
	if(m_CommandUI != NULL)
		m_CommandUI->DeletePanel();
}
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCommandHUD::CCommandHUD( const char *pElementName ) : CHudElement( pElementName ), Panel(NULL, "RoundResetTime")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );	//Our parent is the screen itself.

	//SET_HIDDEN_BITS( 0 );

	m_CommandUI = new vgui::ImagePanel( this, "CommandInterface" );
	m_CommandUI->SetImage( COMMANDUI );
	m_CommandUI->SetVisible(true);
	m_CommandUI->SetShouldScaleImage( true );

	SetPaintBackgroundEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCommandHUD::Init()
{
	Reset();
}

void CCommandHUD::ApplySchemeSettings( vgui::IScheme *pScheme ) 
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetPos(	0, 0 );
	int width, height;
	width = ScreenWidth();
	height = ScreenHeight();

	SetSize( width, height );

	int w, h;
	float offsetW, offsetH;
	offsetW = ((float)width) / 640.0f;;
	offsetH = ((float)height) / 480.0f;
	w = (int)(offsetW * 512.0f);
	h = (int)(offsetH * 128.0f);

	m_CommandUI->SetShouldScaleImage( true );
	m_CommandUI->SetSize( w, h );
	m_CommandUI->SetPos(width-w,height-h);
	m_CommandUI->SetVisible( true );
}

void CCommandHUD::Paint() 
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCommandHUD::Reset()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCommandHUD::VidInit()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCommandHUD::OnThink()
{
}