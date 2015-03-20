#ifndef COMMAND_INFO_H
#define COMMAND_INFO_H

#include "sdkviewport.h"

class CCommandInfo : public vgui::Panel
{
private:
	DECLARE_CLASS_SIMPLE( CCommandInfo, vgui::Frame );

public:
	CCommandInfo(vgui::Panel *parent, const char *panelName);

	virtual ~CCommandInfo();

	virtual void VidInit();
	virtual void Reset();
	//virtual void Update();

	virtual void Paint();
	virtual void OnThink();

	void RemoveInfoText();

	void SetOffsetPos( int x, int y, int width, int height )
	{
		m_StartX = x;
		m_StartY = y;
		m_StartW = width;
		m_StartH = height;
	}
	
	vgui::Button *m_pStartPause;

	int m_StartW;
	int m_StartH;
	int m_StartX;
	int m_StartY;

	vgui::Label *m_Money;
	vgui::Label *m_Level;
	vgui::Label *m_CastleLives;

	vgui::Label *m_TowerName;
	vgui::Label *m_TowerDamage;
	vgui::Label *m_TowerRadius;
	vgui::Label *m_TowerSpeed;
	vgui::Label *m_TowerDelay;
	vgui::Label *m_TowerLevel;

};

#endif //COMMAND_INFO_H