//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "kbutton.h"
#include "input.h"

#include "towers/CommandVGUI.h"

//-----------------------------------------------------------------------------
// Purpose: TF Input interface
//-----------------------------------------------------------------------------
class CSDKInput : public CInput
{
public:
	virtual	bool WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand );
};

static CSDKInput g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;


bool CSDKInput::WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand )
{
	bool wrote = CInput::WriteUsercmdDeltaToBuffer( buf, from, to, isnewcommand );

	g_CommandPanel->OverrideBuffer( buf );
	return wrote;
}