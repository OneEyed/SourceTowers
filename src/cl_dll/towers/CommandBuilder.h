#ifndef COMMAND_BUILDER_H
#define COMMAND_BUILDER_H

#include "sdkviewport.h"

class BuilderButton;
#include <vgui_bitmapbutton.h>

class CCommandBuilder : public vgui::Panel
{
private:
	DECLARE_CLASS_SIMPLE( CCommandBuilder, vgui::Panel );

public:
	CCommandBuilder(vgui::Panel *parent, const char *panelName );
	virtual ~CCommandBuilder();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Reset();
	virtual void OnTick();

	bool ShouldAllowMouseClick();
	void SetOffsetPos( int x, int y, int width, int height )
	{
		m_StartX = x;
		m_StartY = y;
		m_StartW = width;
		m_StartH = height;
	}

	int m_StartW;
	int m_StartH;
	int m_StartX;
	int m_StartY;

	HFont m_Font8;

	BuilderButton *m_Earth;
	BuilderButton *m_Fire;
	BuilderButton *m_Ice;
	BuilderButton *m_Super;
	BuilderButton *m_Upgrade;
	BuilderButton *m_Sell;
};

class BuilderButtonInfo;
class BuilderButton : public CBitmapButton
{
private:
		DECLARE_CLASS_SIMPLE( CCommandBuilder, CBitmapButton );
public:
	BuilderButton(vgui::Panel *parent, const char *PanelName, const char *text, int towerid);
	~BuilderButton( void );

	virtual void OnCursorEntered( void );
	virtual void OnCursorExited( void );

	void SetInfo( const char *szTowerName, const char *szPrice );
	BuilderButtonInfo *m_pInfo;
};

class BuilderButtonInfo : public vgui::Panel 
{
public:
	BuilderButtonInfo(vgui::Panel *parent, int towerid);
	~BuilderButtonInfo();

	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);
	void SetInfo( HFont font, const char *szTowerName, const char *szPrice );
	void Reset();

	int m_iTowerID;
	vgui::Label *m_TowerName;
	vgui::Label *m_Price;
};

#endif //COMMAND_BUILDER_H