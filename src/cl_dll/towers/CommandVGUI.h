#ifndef CHAR_SELECT_H
#define CHAR_SELECT_H

#ifdef _WIN32
#pragma once
#endif

#include "sdkviewport.h"
#include "towers/SelectionFilter.h"

class CCommandInfo;
class CCommandBuilder;
class C_SDKPlayer;

struct EntHealth_t
{
	Vector2D topLeft;
	Vector2D bottomRight;
	float pct;
	bool bEnabled;
};


int GetUniqueID( CBaseEntity *pEnt );

//Transforms our mouse x/y to a normalized direction vector (from player position)
void ScreenToWorld( const Vector& screen, Vector& point );
//Transform World (x,y,z) Coordinates to Screen (x,y)
void WorldToScreen( const Vector &myOrig, const Vector& pos, int &x, int &y, Vector &offset=Vector(0,0,0) );



//-----------------------------------------------------------------------------
// Purpose: Creates our frame for mouse inputs
//-----------------------------------------------------------------------------
class CCommand : public vgui::Frame, public IViewPortPanel
{
private:
	DECLARE_CLASS_SIMPLE( CCommand, vgui::Frame );

public:
	CCommand(IViewPort *pViewPort);
	virtual ~CCommand();

	virtual const char *GetName( void ) { return PANEL_TOWERSCOMMAND; }

	virtual void SetData(KeyValues *data) {};
	virtual void Reset();
	virtual void Update() {};
	virtual bool NeedsUpdate( void ) { return false; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Paint();
	virtual void OnThink();
	virtual void VidInit();

	// vgui overrides
	virtual void OnCommand( const char *command);

	virtual void OnScreenSizeChanged(int iOldWide, int iOldTall);
	//virtual void PerformLayout();

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);
	virtual void OnMouseWheeled(int delta);

	void DrawEnemyHealth();
	void DrawSelectionBox();
	//So we can control movements with mouse!
	void CreateMove( float flInputSampleTime, CUserCmd *cmd );
	void OverrideBuffer( bf_write *buf );


	bool CustomSelection( CUtlVector<int> &pEnts );
	void GetSelectionBounds( int &up, int &down, int &left, int &right );
	void CreateSelectionBox( C_SDKPlayer *localPlayer, int cx, int cy);
	int FindUnitsInRect( CUtlVector<int> &pEnts, ISelectFilter *filter = NULL ); //returns amount found

	
protected:

	IViewPort	*m_pViewPort;

	int			m_iScreenWidth;
	int			m_iScreenHeight;

	int			m_iMouseMove;
	int			m_iButtons;
	float		m_flUpMove;

	int			m_bMultiSelectStep;
	bool		m_bDrawRect;
	bool		m_bDrawEnemyHealth;

	C_SDKPlayer *m_sdkPlayer;

	//Selection box start/end points.
	Vector2D	m_vStart;
	Vector2D	m_vEnd;

	//Enemy health stuff to forward to Paint()
	CUtlVector< EntHealth_t > m_pEnemyHealth;

	//Info and Building create buttons
	CCommandBuilder	*m_pCommandBuild;
	CCommandInfo	*m_pCommandInfo;
};

extern CCommand *g_CommandPanel;

#endif // CLASSMENU_H
