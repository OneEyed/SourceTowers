#ifndef C_GENERIC_MENU_H
#define C_GENERIC_MENU_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/BuildGroup.h>
#include <UtlStack.h>
#include <UtlVector.h>

class GenericSubMenu;

class GenericMenu : public vgui::Panel
{
private:
	DECLARE_CLASS_SIMPLE( GenericMenu, vgui::Panel );

public:
	GenericMenu(vgui::Panel *parent, const char *panelName)
		: BaseClass(parent,panelName), m_CurrFrame()
		{ SetParent( parent ); }

	~GenericMenu() {};

	void Clear();

	GenericSubMenu* Create(const char* subpanel);
	void Destroy(const char* subpanel);

	bool Push(const char* subpanel);
	bool Pop();
	bool PushPop(const char* subpanel);

	bool ClearTop();
	bool RenderTop();

	int Depth() 
		{ return m_CurrFrame.Count(); }

protected:
	virtual void OnCommand( const char *command );

	CUtlStack < GenericSubMenu* > m_CurrFrame;
	CUtlVector < GenericSubMenu* > m_SubMenus;

//private:
};


class GenericSubMenu : public vgui::Panel
{
friend GenericMenu;

private:
	DECLARE_CLASS_SIMPLE( GenericSubMenu, vgui::Panel );

	GenericSubMenu(vgui::Panel *parent, const char *panelName)
		: BaseClass(parent,panelName)
		{ SetParent( parent ); }

	~GenericSubMenu() {};

	void AddButton(vgui::Panel* button)
	{ 
		button->SetParent(this); 
		button->SetAutoDelete(true); 
		button->SetVisible(false);

		button->AddActionSignalTarget(GetParent());
		m_CurrButtons.AddToTail(button); 
	}

	void RemoveButton(const char* button)
	{ 	
		vgui::Panel* Button = FindChildByName(button); 
		if(Button) { Button->DeletePanel(); } 
		m_CurrButtons.FastRemove(m_CurrButtons.Find( Button ) );
	}

protected:

	virtual void ActivateFrame( void )
	{
		SetVisible(true);
		MoveToFront();

		vgui::Panel* tmp;
		FOR_EACH_VEC( m_CurrButtons, i )
		{
			tmp = m_CurrButtons[i];
			if(tmp) 
			{
				tmp->SetVisible(true);
				MoveToFront();
			}
		}
	}

	virtual void DeactivateFrame( void )
	{
		vgui::Panel* tmp;
		FOR_EACH_VEC( m_CurrButtons, i )
		{
			tmp = m_CurrButtons[i];
			if(tmp) tmp->SetVisible(false);
		}
	}

	CUtlVector < vgui::Panel* > m_CurrButtons;
};


#endif