//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple logical entity that counts up to a threshold value, then
//			fires an output when reached.
//
//=============================================================================//

#include "cbase.h"
#include "towers_move.h"

CUtlVector< CTowersMove *> g_MoveEnt;

LINK_ENTITY_TO_CLASS( towers_move, CTowersMove  );
LINK_ENTITY_TO_CLASS( towers_spawn, CPointEntity );

// Start of our data description for the class
BEGIN_DATADESC( CTowersMove  )
	// Links our member variable to our keyvalue from Hammer
	DEFINE_KEYFIELD( m_iPos, FIELD_INTEGER, "pos" ),
END_DATADESC()