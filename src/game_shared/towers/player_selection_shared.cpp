#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_sdk_player.h"
#else
	#include "sdk_player.h"
	#include "ilagcompensationmanager.h"
#endif

#include "BaseClickable.h"
#include "BaseTower.h"
#include "in_buttons.h"


void CSDKPlayer::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();

	//If our client sent us a multi-selection list. Update it on server.
	if(m_bIsMultiSelect)
	{
		m_bIsMultiSelect = false;
		UpdateSelectionList();
		return;
	}

	int count = m_pSelectedTargets.Count();

	//Handle our selection as a group.
	if(count > 1 )
	{
		int i;
		CBaseClickable *pClick = NULL;

		for(i=m_pSelectedTargets.Count()-1; i>=0; i--)
		{
			pClick = (CBaseClickable*)m_pSelectedTargets[i].Get();
			if(!pClick)
				continue;

			pClick->Selected( this );
		}
	}
	//Handle our single selection.
	else if( count == 1)
	{
		CBaseClickable *pClick = GetSelected();
		if( pClick )//IsValidClickable() )
		{
			pClick->Selected( this );

			int type = pClick->GetClickType();
			if(type == CLICK_BUILDING)
			{
				CBaseTower *pTower = ToBaseTower( pClick );
				if(!pTower)
					return;

				if( HandleBuilding( pTower ) )
					return;
			}
			else if(type == CLICK_UNIT)
			{
				pClick->Selected( this );
			}
		}
		
	}
	else
		RemoveCurrentSelection();

#if !defined (CLIENT_DLL)
	lagcompensation->StartLagCompensation( this, GetCurrentCommand() );
#endif

	HandleSelection();

#if !defined (CLIENT_DLL)
	lagcompensation->FinishLagCompensation( this );
#endif

}

void CSDKPlayer::HandleSelection( void )
{
	//Released mouse2, clear our selections.
	if(!(m_nButtons & IN_ATTACK) && (m_afButtonReleased & IN_ATTACK2))
	{
		RemoveCurrentSelection();
		return;
	}

	trace_t trace;
	Vector origin = EyePosition();
	Vector forward = m_vMouseDirection;
	Vector end = origin + (forward * MAX_TRACE_LENGTH);

	//Trace straight down our mouse direction.
	CTraceFilterSELECTABLE filter;
	UTIL_TraceLine( origin, end, MASK_SOLID, &filter, &trace );

	//Hit world?
	if( !trace.DidHitNonWorldEntity() )
	{
		//Clicked on world, remove selection.
		if((m_afButtonReleased & IN_ATTACK))
			RemoveCurrentSelection();

		//We're hitting world, exit hover.
		CBaseClickable *pOld = (CBaseClickable*)m_hHovered.Get();
		if(pOld)
			pOld->OnHoverExit( this );

		return;
	}

	//Hit Entity?
	if( (trace.m_pEnt->GetFlags() & FL_IS_SELECTABLE) > 0 )
	{
		CBaseClickable *pTemp = ToClickable( trace.m_pEnt );
		CBaseClickable *pHoverOld = m_hHovered.Get();
		
		//Entered Hover state on entity.
		if(pTemp != pHoverOld )
		{
			if( pHoverOld )
				pHoverOld->OnHoverExit( this );

			pTemp->OnHoverEnter( this );
		}

		//Currently Hovering entity.
		pTemp->OnHover( this );

		//Did we select entity?
		if((m_afButtonReleased & IN_ATTACK) && pTemp != GetSelected() )
		{
			pTemp->OnSelected( this );
			SetSelected( pTemp );
		}
		
	}
}

//Update our multiple selections and notify entity.
void CSDKPlayer::UpdateSelectionList( void )
{
	CBaseClickable *pClick = NULL;
	int i;

	for(i=m_pSelectedTargets.Count()-1; i>=0; i--)
	{
		pClick = (CBaseClickable*)m_pSelectedTargets[i].Get();
		if(!pClick)
		{
			m_pSelectedTargets.Remove(i);
			continue;
		}
		pClick->OnSelected( this );
	}
}

//Some AI for what to do with a building.
bool CSDKPlayer::HandleBuilding( CBaseTower *pTower )
{
	CBaseEntity *pOwner = pTower->GetOwnerEntity();
	if(pOwner == (CBaseEntity*)this)
	{
		if(!pTower->IsBuilt())
		{
			pTower->BuildingPlacement( this );
			return true;
		}
	}
	return false;
}


//Clear our selection list.
void CSDKPlayer::RemoveCurrentSelection(void)
{
	if( m_pSelectedTargets.Count() > 0 )
		m_pSelectedTargets.RemoveAll();
}

//Get our single selection. (NULL if we have a multiple selection)
CBaseClickable *CSDKPlayer::GetSelected() 
{ 
#ifdef CLIENT_DLL
	if( m_hSelected.Get() )
		SetSelected( (CBaseClickable*)m_hSelected.Get() );
#endif

	if(m_pSelectedTargets.Count() != 1)
		return NULL;

	CBaseClickable *pClick = (CBaseClickable*)m_pSelectedTargets[0].Get();

	if( !pClick || pClick->m_bMarkedForDeletion )
		return NULL;


	return pClick; 
}

//Set a single selection
void CSDKPlayer::SetSelected( CBaseClickable *pClick ) 
{ 
	RemoveCurrentSelection();
	m_pSelectedTargets.AddToTail( pClick );
}
