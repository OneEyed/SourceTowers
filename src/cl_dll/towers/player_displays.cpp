#include "cbase.h"
#include "c_sdk_player.h"
#include "towers/BaseTower.h"

//---------------------------------------------------------------
// Pre/Post Drawing - Forwarded to our CBaseClickable Entities.
//---------------------------------------------------------------

//Draws BEFORE models are drawn.
void Pre_DrawExtras()
{
	CSDKPlayer *localPlayer = CSDKPlayer::GetLocalSDKPlayer();
	if(!localPlayer)
		return;

	int i;
	int count = g_pClickables.Count();
	C_BaseClickable *pClick = NULL;
	C_BaseClickable *pSelected = localPlayer->GetSelected();
	C_BaseClickable *pHovered = localPlayer->m_hHovered.Get();
	bool bSelected = false;
	bool bHovered = false;

	for(i=0; i<count; i++)
	{
		pClick = g_pClickables[i];
		if(!pClick)
			continue;
		
		if(pSelected == NULL)	bSelected = false;
		else					bSelected = (pSelected==pClick);

		if(pHovered == NULL)	bHovered = false;
		else					bHovered = (pHovered==pClick);

		pClick->PreDraw(localPlayer, bSelected, bHovered);
	}
}

//Draws AFTER models are drawn.
void Post_DrawExtras()
{
	CSDKPlayer *localPlayer = CSDKPlayer::GetLocalSDKPlayer();
	if(!localPlayer)
		return;

	int i;
	int count = g_pClickables.Count();
	C_BaseClickable *pClick = NULL;
	C_BaseClickable *pSelected = localPlayer->GetSelected();
	C_BaseClickable *pHovered = localPlayer->m_hHovered.Get();;
	bool bSelected = false;
	bool bHovered = false;

	for(i=0; i<count; i++)
	{
		pClick = g_pClickables[i];
		if(!pClick)
			continue;
		
		if(pSelected == NULL)	bSelected = false;
		else					bSelected = (pSelected==pClick);

		if(pHovered == NULL)	bHovered = false;
		else					bHovered = (pHovered==pClick);

		pClick->PostDraw(localPlayer, bSelected, bHovered);
	}
}