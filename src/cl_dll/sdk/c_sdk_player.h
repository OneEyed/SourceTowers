//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_SDK_PLAYER_H
#define C_SDK_PLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "sdk_playeranimstate.h"
#include "c_baseplayer.h"
#include "sdk_shareddefs.h"
#include "baseparticleentity.h"

class C_BaseTower;
class C_BaseClickable;

void Pre_DrawExtras();
void Post_DrawExtras();


class C_SDKPlayer : public C_BasePlayer, public ISDKPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( C_SDKPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_SDKPlayer();
	~C_SDKPlayer();

	static C_SDKPlayer* GetLocalSDKPlayer();

	virtual const QAngle& GetRenderAngles();
	virtual void UpdateClientSideAnimation();
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );


// Called by shared code.
public:
	
	// ISDKPlayerAnimState overrides.
	virtual CWeaponSDKBase* SDKAnim_GetActiveWeapon();
	virtual bool SDKAnim_CanMove();

	void DoAnimationEvent( PlayerAnimEvent_t event );
	bool ShouldDraw();

	ISDKPlayerAnimState *m_PlayerAnimState;

	QAngle	m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	EHANDLE	m_hRagdoll;

	CWeaponSDKBase *GetActiveSDKWeapon() const;

	C_BaseAnimating *BecomeRagdollOnClient( bool bCopyEntity);
	IRagdoll* C_SDKPlayer::GetRepresentativeRagdoll() const;

	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y );

//--------------------------
//TOWERS CODE
//--------------------------

//Misc.
public:
	void ItemPostFrame( void );
	virtual int DrawModel( int flags );

	bool HandleBuilding( C_BaseTower *pTower );
	void HandleSelection();

	void UpdateSelectionList();
	void RemoveCurrentSelection();

//Accessors
public:
	
	const Vector &GetMouseDirection() { return m_vMouseDirection; };

	int GetMoney()						{ return m_iMoney;			};
	void SetMoney( int money )			{ m_iMoney = money;			};
	void AddMoney( int money )			{ m_iMoney += money;		};
	void RemoveMoney( int money )		{ m_iMoney -= money;		};

	int GetLevel()						{ return m_iLevel;			};
	void SetLevel( int lvl )			{ m_iLevel = lvl;			};
	
	int GetCommanderMode()				{ return m_iCommanderMode;	};
	void SetCommanderMode( int mode )	{ m_iCommanderMode = mode;	};
	
	C_BaseClickable *GetSelected();
	void SetSelected( C_BaseClickable *pClick );

//Variables
public:

	CNetworkVar( int, m_iMoney );
	CNetworkVar( int, m_iLevel );
	CNetworkVar( int, m_iCommanderMode );
	
	CUtlVector< C_BaseTower *>m_pTowers; //Buildings we own.

	CHandle< C_BaseClickable > m_hSelected; //Defines the building we created.
	CHandle< C_BaseClickable > m_hHovered;
	CUtlVector< CHandle< C_BaseClickable > >m_pSelectedTargets;
	
	bool m_bIsMultiSelect;
	Vector m_vMouseDirection;

private:
	C_SDKPlayer( const C_SDKPlayer & );
};

inline C_SDKPlayer* ToSDKPlayer( CBaseEntity *pPlayer )
{
	Assert( dynamic_cast< C_SDKPlayer* >( pPlayer ) != NULL );
	return static_cast< C_SDKPlayer* >( pPlayer );
}


#endif // C_SDK_PLAYER_H
