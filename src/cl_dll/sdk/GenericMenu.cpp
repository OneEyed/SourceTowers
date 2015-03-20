#include "cbase.h"
#include "GenericMenu.h"

void GenericMenu::Clear()
{
	while(Pop());

	GenericSubMenu* tmp;
	FOR_EACH_VEC( m_SubMenus, i )
	{
		tmp = m_SubMenus[i];
		if(tmp) tmp->DeactivateFrame();
	}
}

GenericSubMenu* GenericMenu::Create(const char* name)
{
	GenericSubMenu* newchild = new GenericSubMenu(this,name);
	newchild->SetAutoDelete(true);
	//newchild->AddActionSignalTarget(this);

	m_SubMenus.AddToTail(newchild); 
	return newchild;
}

void GenericMenu::Destroy(const char* subpanel)
{
	vgui::Panel* SubMenu = FindChildByName(subpanel);
	if(!SubMenu) return;

	SubMenu->DeletePanel();

	GenericSubMenu* TruePanel = dynamic_cast<GenericSubMenu*>(SubMenu);
	if(!TruePanel) return;

	m_SubMenus.FastRemove(m_SubMenus.Find( TruePanel ) );
}

bool GenericMenu::Push(const char* subpanel)
{
	vgui::Panel* SubMenu = FindChildByName(subpanel);
	if(!SubMenu) return false;

	GenericSubMenu* TruePanel = dynamic_cast<GenericSubMenu*>(SubMenu);
	if(!TruePanel) return false;

	TruePanel->ActivateFrame();
	m_CurrFrame.Push(TruePanel);

	return true;
}

bool GenericMenu::Pop()
{
	if(m_CurrFrame.Count() < 1) return false;
	GenericSubMenu* TruePanel = m_CurrFrame.Top();

	TruePanel->DeactivateFrame();
	m_CurrFrame.Pop();

	return true;
}

bool GenericMenu::PushPop(const char* subpanel)
{
	ClearTop();
	return Push(subpanel);
}

bool GenericMenu::ClearTop()
{
	if(m_CurrFrame.Count() < 1) return false;
	GenericSubMenu* TruePanel = m_CurrFrame.Top();

	TruePanel->DeactivateFrame();
	return true;
}

bool GenericMenu::RenderTop()
{
	if(m_CurrFrame.Count() < 1) return false;
	GenericSubMenu* TruePanel = m_CurrFrame.Top();

	TruePanel->ActivateFrame();
	return true;
}


void GenericMenu::OnCommand( const char *command )
{
	if ( Q_strncmp( command, "frameshiftw", 11 ) == 0 )
	{
		command += 12;
		Push(command);
	}
	else if ( Q_strncmp( command, "frameshiftp", 11 ) == 0)
	{
		command += 12;
		PushPop(command);
	}
	else if ( Q_strncmp( command, "frameshiftb", 11 ) == 0)
	{
		if(Pop()) RenderTop();
	}
	else engine->ClientCmd( const_cast<char *>( command ) );
	//else BaseClass::OnCommand(command);
}
