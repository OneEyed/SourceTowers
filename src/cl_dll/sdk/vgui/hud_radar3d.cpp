//======================================================================\\
// MKS-OneEyed: HUD indicator at bottom.
// TODO:
//======================================================================//

#include "cbase.h"


#include "c_sdk_player.h"
#include "hudelement.h"
#include "hud_macros.h"
#include <KeyValues.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include "vgui/ILocalize.h"
#include "iclientmode.h"
#include <igameevents.h>
#include <vgui/ISurface.h>
#include <vgui_controls/ImagePanel.h>
#include "in_buttons.h"

#include "view.h"
#include "view_shared.h"
#include "iviewrender.h"
#include "viewrender.h"
#include "BaseClickable.h"

#include "rendertexture.h"

#include "materialsystem/ITexture.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#define MATERIAL_RADAR_RT	"radarRT"

//-----------------------------------------------------------------------------
// Purpose: Radar panel
//-----------------------------------------------------------------------------
class CRadar3d : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE( CRadar3d, Panel );

public:
	CRadar3d( const char *pElementName );
	~CRadar3d();

	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnThink();

	void DrawToTarget();
	void DrawRadar();
	void DrawPlayers();

protected: 
	virtual void Paint();

private:
	// old variables
	bool	m_bIsActive;

	CMaterialReference m_RadarMaterial;
	CMaterialReference m_PlayerMaterial;

	Vector m_Origin;
	QAngle m_Angles;
};	

extern CHud gHUD;

DECLARE_HUDELEMENT( CRadar3d );

CRadar3d::~CRadar3d()
{
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRadar3d::CRadar3d( const char *pElementName ) : CHudElement( pElementName ), vgui::Panel(NULL, "HudRadar3d")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );	//Our parent is the screen itself.

	m_bIsActive = true;

	m_Angles.Init();
	m_Origin.Init();
	SetPaintBackgroundEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRadar3d::Init()
{
	Reset();
}

void CRadar3d::ApplySchemeSettings( vgui::IScheme *pScheme ) 
{
	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRadar3d::Reset()
{
	m_bActive = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRadar3d::VidInit()
{
	Reset();

	m_PlayerMaterial.Init( "radar/player.vmt", TEXTURE_GROUP_VGUI );
	m_RadarMaterial.Init( "radar/radar.vmt", TEXTURE_GROUP_VGUI );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------

ConVar minimap_tilt( "minimap_tilt", "90" );
ConVar minimap_tiltZ( "minimap_tiltZ", "0" );

#define MATERIAL_RADAR		"radar/radar"
#define MATERIAL_PLAYER		"radar/player.vmt"

ConVar minimap_zoom( "minimap_zoom", "50", FCVAR_ARCHIVE, "Zooms distance" );

void CRadar3d::OnThink()
{
}

void CRadar3d::Paint()
{
	CSDKPlayer *localPlayer =  CSDKPlayer::GetLocalSDKPlayer();
	if(!localPlayer)
		return;

	QAngle angle;;
	QAngle viewangle;
	engine->GetViewAngles( viewangle );;
	angle = viewangle;
	angle.x = minimap_tilt.GetFloat();
	angle.z = minimap_tiltZ.GetFloat();

	Vector origin = localPlayer->GetRenderOrigin();
	Vector forward, right, up;

	int wide,tall;
	int x,y;

	GetSize(wide,tall);
	GetPos(x,y);

	CViewSetup myView;
	myView.x = x;
	myView.y = y;
	myView.width = wide;
	myView.height = tall;
	myView.fov = 90;
	myView.zFar = 1000.0f;
	myView.zNear = 7.0f;
	myView.m_bOrtho = false;

	origin = Vector(0,0,0);
	QAngle tilt(minimap_tilt.GetFloat(),-90,minimap_tiltZ.GetFloat());
	AngleVectors( tilt, &forward, &right, &up );;
	m_Origin = origin - (forward * 140) + Vector(0,0,0);

	myView.origin = m_Origin;
	myView.angles = tilt;
  
	ITexture *radar_rt = GetRadarRT();
	Frustum dummyFrustum;

	//Draw our stuff to new view.
	render->Push3DView( myView, 0, false, NULL, dummyFrustum );
	DrawRadar();
	DrawPlayers();
	render->PopView( dummyFrustum );
	
	//Reset to old view.
	myView = *view->GetViewSetup();
	render->Push3DView( myView, 0, false, NULL, dummyFrustum );
	render->PopView( dummyFrustum );
}


void CRadar3d::DrawRadar()
{
	CSDKPlayer *localPlayer =  CSDKPlayer::GetLocalSDKPlayer();
	if(!localPlayer)
		return;

	int alpha = 1;
	Vector vColor(1,1,1);
	float radius = 100.0f;
	//origin = localPlayer->GetRenderOrigin() + (Vector(0,0, localPlayer->WorldAlignMins().z+1));


	IMaterial *m_pMaterial = NULL;;
	m_pMaterial = materials->FindMaterial( "radar/radar", TEXTURE_GROUP_CLIENT_EFFECTS );

	IMesh *pMesh = materials->GetDynamicMesh( true, 0, 0, m_pMaterial );

	CMeshBuilder builder;
	builder.Begin( pMesh, MATERIAL_QUADS, 1 );;
	
	
	//Vector vColor( 1, 0, 0 );
	Vector vPt, vBasePt, vRight, vUp;;
	vBasePt = Vector(0,0,0);
	vRight = Vector(radius,0,0);
	vUp = Vector(0,radius, 0 );

	vPt = vBasePt - vRight + vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 0, 1 );
	builder.AdvanceVertex();
	
	vPt = vBasePt + vRight + vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 1, 1 );
	builder.AdvanceVertex();
	
	vPt = vBasePt + vRight - vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );;
	builder.TexCoord2f( 0, 1, 0 );
	builder.AdvanceVertex();
	
	vPt = vBasePt - vRight - vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 0, 0 );
	builder.AdvanceVertex();
	
	builder.End( false, true );
}

void CRadar3d::DrawPlayers()
{
	CSDKPlayer *localPlayer =  CSDKPlayer::GetLocalSDKPlayer();
	if(!localPlayer)
		return;

	int alpha = 1;
	Vector vColor(1,1,1);
	float radius = 2.0f;
	Vector origin;

	QAngle angle;
	QAngle viewAngle;
	engine->GetViewAngles( angle );

	viewAngle = angle;
	angle.y += 90.0f;
	angle.x = 0.0f;

	matrix3x4_t worldToEntity;
	AngleMatrix( angle, localPlayer->GetAbsOrigin(), worldToEntity );;

	int i;
	int count = g_pClickables.Count();
	C_BaseClickable *pClick = NULL;

	Vector vPt, vBasePt;
	Vector vForward, vRight, vUp;
	
	AngleVectors( viewAngle, &vForward, &vRight, &vUp );
	
	//Icons sizes
	float flHeight = 15.0f;
	float flWidth = 10.0f;

	Vector point;
	Vector fwd, right( 1, 0, 0 ), up( 0, 1, 0 );
	IMesh *pMesh;
	IMaterial *m_pMaterial;;

	for(i=0; i<count; i++)
	{
		pClick = g_pClickables[i];
		if(!pClick)
			continue;


		VectorITransform( pClick->GetAbsOrigin(), worldToEntity, origin );;

		origin /= minimap_zoom.GetInt();//100.0f;
		origin.z = flHeight;

		float len = origin.Length2D();
		if(len > 95.0f)
			continue;

		if(pClick->GetOwnerEntity() == localPlayer)
			vColor = Vector(0,0,255);
		else
			vColor = Vector(255,0,0);

		unsigned char pColor[4] = { vColor.x, vColor.y, vColor.z, 1 };

		materials->Bind( m_PlayerMaterial );
		pMesh = materials->GetDynamicMesh();

		CMeshBuilder meshBuilder;
		meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );
	
		up = Vector(0,0,1);
		right = Vector(-1,0,0);

		meshBuilder.Color4ubv (pColor);
		meshBuilder.TexCoord2f (0, 0, 1);
		VectorMA (origin, -flHeight, up, point);
		VectorMA (point, -flWidth, right, point);
		meshBuilder.Position3fv (point.Base());
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv (pColor);
		meshBuilder.TexCoord2f (0, 0, 0);
		VectorMA (origin, flHeight, up, point);
		VectorMA (point, -flWidth, right, point);
		meshBuilder.Position3fv (point.Base());
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv (pColor);
		meshBuilder.TexCoord2f (0, 1, 0);
		VectorMA (origin, flHeight, up, point);
		VectorMA (point, flWidth, right, point);
		meshBuilder.Position3fv (point.Base());
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv (pColor);
		meshBuilder.TexCoord2f (0, 1, 1);
		VectorMA (origin, -flHeight, up, point);
		VectorMA (point, flWidth, right, point);
		meshBuilder.Position3fv (point.Base());
		meshBuilder.AdvanceVertex();
		
		meshBuilder.End( false, true );

	}
}

void CRadar3d::DrawToTarget()
{
	
}

CON_COMMAND(radar_tilt_z_up, "")
{
	float tilt = minimap_tiltZ.GetFloat();
	minimap_tiltZ.SetValue( tilt + 3.0f );

}

CON_COMMAND(radar_tilt_z_down, "")
{
	float tilt = minimap_tiltZ.GetFloat();
	minimap_tiltZ.SetValue( tilt - 3.0f );

}

CON_COMMAND(radar_tilt_x_up, "")
{
	float tilt = minimap_tilt.GetFloat();
	minimap_tilt.SetValue( tilt + 3.0f );

}

CON_COMMAND(radar_tilt_x_down, "")
{
	float tilt = minimap_tilt.GetFloat();
	minimap_tilt.SetValue( tilt - 3.0f );
}

CON_COMMAND( radar_zoom, "" )
{
	static int zoom = 1;

	minimap_zoom.SetValue( zoom * 5 );
	zoom++;
	if(zoom > 10)
		zoom = 1;
}
