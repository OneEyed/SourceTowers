//----------------------------------------------------------------
// These functions are from CommandVGUI.h, seperated for convenience.
//----------------------------------------------------------------

#include "cbase.h"
#include "in_buttons.h"
#include "towers/CommandVGUI.h"
#include "c_sdk_player.h"
#include "towers/BaseClickable.h"
//#include "BaseTower.h"
#include <vgui/IInput.h>
#include "bitbuf.h"

//World to screen isn't perfect so offset it a bit.
#define WORLDSCREEN_OFFSET_Z	-75


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//------------------------------------------------------
// Purpose: Filter for Source Towers Game.
//------------------------------------------------------
class CSelectFilterSourceTowers : public CSelectFilter
{
public:
	CSelectFilterSourceTowers( C_SDKPlayer *player )
	{
		sdkPlayer = player;
	}

	//Should we run the expensive selection box test on this entity?
	bool CanTestEntity( CBaseClickable *pEnt )
	{
		if(!pEnt)
			return false;

		if(pEnt->GetClickType() == CLICK_UNIT)
			return true;

		if(pEnt->GetOwnerEntity() != sdkPlayer )
			return false;

		return true;
	}

	//Entity is within our selection box, do whatever!
	bool ShouldSelectEntity( CBaseClickable *pEnt )
	{
		int type = pEnt->GetClickType();

		//Save a list of buildings we attempted to select.
		//but don't allow to select them.
		if(type == CLICK_BUILDING)
		{
			m_pBuildings.AddToTail( pEnt );
			return false;
		}
		return true;
	}	

public:

	CUtlVector< CBaseClickable *> m_pBuildings;
	C_SDKPlayer *sdkPlayer;
};

//------------------------------------------------------
// Purpose: Define which entities are to be selected and sent to server,
//			and anything else you might want to capture along the way.
//------------------------------------------------------
bool CCommand::CustomSelection( CUtlVector<int> &pEnts )
{
	if(!C_BasePlayer::GetLocalPlayer())
			return false;

		m_sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();
		if(!m_sdkPlayer)
			return false;
	//if(!m_sdkPlayer)
	//	return false;

	bool isMulti = false;
	CSelectFilterSourceTowers filter(m_sdkPlayer);

	//Multi-Selected units
	if(FindUnitsInRect( pEnts, &filter ) > 0)
	{
		isMulti = true;
	}
	//Only found buildings, select a random one.
	else if(filter.m_pBuildings.Count() > 0)
	{
		int rand = 0;
		if(filter.m_pBuildings.Count() > 1)
			rand = random->RandomInt( 0, filter.m_pBuildings.Count()-1 );
		CBaseClickable *pEnt = filter.m_pBuildings[rand];

		if(pEnt)
		{
			m_sdkPlayer->m_pSelectedTargets.RemoveAll();
			m_sdkPlayer->m_pSelectedTargets.AddToTail( pEnt );

			isMulti = true;
			
			int id =  GetUniqueID(pEnt);
			if(id != -1)
				pEnts.AddToTail( id );
		}
	}

	m_sdkPlayer->m_bIsMultiSelect = isMulti;

	return isMulti;
}

//------------------------------------------------------
// Purpose: Creates and manages our selection box rectangle.
//------------------------------------------------------
void CCommand::CreateSelectionBox( C_SDKPlayer *localPlayer, int cx, int cy)
{
	if(m_iButtons & IN_ATTACK)
	{
		if(localPlayer->m_hSelected.Get())
			return;

		int width, height;
		width = ScreenWidth();
		height = ScreenHeight();

		if(cx < 0)			cx = 0;
		if(cy < 0)			cy = 0;
		if(cx > width)		cx = width;
		if(cy > height)		cy = height;

		//input()->SetCursorPos(cx, cy);
		//Begin multi selecting.
		if(!m_bMultiSelectStep)
		{
			m_vStart = Vector2D( cx, cy );
			m_bMultiSelectStep = 1;
		}

		m_vEnd = Vector2D( cx, cy );
		
		//Only draw rect if we're big enough!
		Vector2D test = m_vStart - m_vEnd;
		float len = test.x*test.x + test.y*test.y;
		if(len < 300)
			m_bDrawRect = false;
		else
			m_bDrawRect = true;
	}
	else
	{
		//Make sure we're about 20 pixels to qualify as a multi select
		if(m_bMultiSelectStep == 1) 
		{
			m_bDrawRect = false;

			Vector2D test = m_vStart - m_vEnd;
			float len = test.x*test.x + test.y*test.y;
			if(len < 400)
			{
				m_bMultiSelectStep = 0;
				return;
			}
			m_bMultiSelectStep = 2;
		}
	}
}

//------------------------------------------------------
// Purpose: When we're about to send data to server, add in our multi-select ent ids.
//------------------------------------------------------
void CCommand::OverrideBuffer( bf_write *buf )
{
	bool isMulti = false;
	CUtlVector< int > pEnts;

	if(m_bMultiSelectStep == 2)
	{
		isMulti = CustomSelection( pEnts );
		m_bMultiSelectStep = 0;
	}

	//If we're doing a multi select send our info!
	if( isMulti )
	{
		buf->WriteOneBit( 1 );
		buf->WriteShort( pEnts.Count() );
		
		int i, num;
		for (i = 0; i < pEnts.Count(); i++)
		{
			num = pEnts[i];
			buf->WriteUBitLong( num, 24 );
		}
	}
	else
		buf->WriteOneBit( 0 );
}

//------------------------------------------------------
// Purpose: Find entities in selection box and list them by unique EHANDLE ids.
//------------------------------------------------------
int CCommand::FindUnitsInRect( CUtlVector<int> &pEnts, ISelectFilter *filter )
{
	if(!C_BasePlayer::GetLocalPlayer())
			return 0;

		m_sdkPlayer = C_SDKPlayer::GetLocalSDKPlayer();
		if(!m_sdkPlayer)
			return 0;
	//if(!m_sdkPlayer)
	//	return 0;

	bool up, left, right, down;
	int clipRect[4];
	GetSelectionBounds( clipRect[0], clipRect[1], clipRect[2], clipRect[3] );
	
	C_BaseClickable *pEnt = NULL;
	Vector origin = m_sdkPlayer->GetLocalOrigin();

	int x, y;
	int i, entcount = 0;
	int id;

	for(i=0;i<g_pClickables.Count();i++)
	{
		pEnt = g_pClickables[i];
		if(!pEnt)
			continue;

		if( filter && !filter->CanTestEntity(pEnt))
			continue;

		WorldToScreen( origin, pEnt->GetAbsOrigin(), x, y );

		up = (y <= clipRect[0]);
		down = (y >= clipRect[1]);
		left = (x <= clipRect[2]);
		right = (x >= clipRect[3]);
		
		if(!up && !down && !left && !right)
		{
			if( filter && !filter->ShouldSelectEntity( pEnt ) )
				continue;

			if(pEnts.Count() == 0)
				m_sdkPlayer->m_pSelectedTargets.RemoveAll();

			m_sdkPlayer->m_pSelectedTargets.AddToTail( pEnt );

			id = GetUniqueID( pEnt );
			if(id != -1)
				entcount = pEnts.AddToTail( id);
	
			if(entcount >= MAX_SELECTABLE_UNITS)
				break;
		}
	}
	return pEnts.Count();
}

//------------------------------------------------------
// Purpose: Retrieve selection box bounds.
//------------------------------------------------------
void CCommand::GetSelectionBounds( int &up, int &down, int &left, int &right )
{
	if(m_vStart.y < m_vEnd.y)
	{
		up = m_vStart.y;	down = m_vEnd.y;
	}
	else
	{
		up = m_vEnd.y;		down = m_vStart.y;
	}

	if(m_vStart.x < m_vEnd.x)
	{
		left = m_vStart.x;	right = m_vEnd.x;
	}
	else
	{
		left = m_vEnd.x;	right = m_vStart.x;
	}
}

//------------------------------------------------------
// Purpose: Transform World Position to Screen.
//------------------------------------------------------
void WorldToScreen( const Vector &myOrig, const Vector& pos, int &x, int &y, Vector &offset )
{
	//Vector line, offset; 
	//float distance;
    //float offsetZ;;

	//line = myOrig - pos;
	//distance = line.Length();

    //offsetZ = ((distance - 80)/12.6) + WORLDSCREEN_OFFSET_Z; 
    //offset = Vector(0,0,offsetZ);
	//offset = Vector(0,0,0);
	//Vector offset = 
	GetVectorInScreenSpace( pos, x, y, &offset );
}

//------------------------------------------------------
// Purpose: Transform Screen Position to World.
//------------------------------------------------------
void ScreenToWorld( const Vector& screen, Vector& point )
{
	const VMatrix &worldToScreen = engine->WorldToScreenMatrix();
	int x = screen.x;
	int y = screen.y;
	int w = ScreenWidth();
	int h = ScreenHeight();

	// Remap x and y into -1 to 1 normalized space
	float xf, yf;
	xf = ( 2.0f * x / (float)(w-1) ) - 1.0f;
	yf = ( 2.0f * y / (float)(h-1) ) - 1.0f;

	// Flip y axis
	yf = -yf;

	VMatrix screenToWorld;
	MatrixInverseGeneral( worldToScreen, screenToWorld );

	// Create two points at the normalized mouse x, y pos and at the near and far z planes (0 and 1 depth)
	Vector v1, v2;
	v1.Init( xf, yf, 0.0f );
	v2.Init( xf, yf, 1.0f );

	Vector o2, org;
	// Transform the two points by the screen to world matrix
	screenToWorld.V3Mul( v1, org ); // ray start origin
	screenToWorld.V3Mul( v2, o2 );  // ray end origin
	VectorSubtract( o2, org, point );
	point.NormalizeInPlace();
}

//------------------------------------------------------
// Purpose: Gets the entities unique id (what EHANDLEs send through network)
//------------------------------------------------------
int GetUniqueID( CBaseEntity *pEnt )
{
	if(!pEnt)
		return -1;

	int iSerialNum = pEnt->GetRefEHandle().GetSerialNumber() & (1 << NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS) - 1;
	int num = pEnt->GetRefEHandle().GetEntryIndex() | (iSerialNum << MAX_EDICT_BITS);
	return num;
}