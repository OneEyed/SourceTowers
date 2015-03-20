#ifndef TOWER_ENTITY_H
#define TOWER_ENTITY_H

#ifdef CLIENT_DLL
#define CBaseTower C_BaseTower
#define CSDKPlayer C_SDKPlayer
#endif

#include "towers/BaseClickable.h"
//#include "towers/BaseProjectile.h"
#include "towers/tower_shareddef.h"

class CSDKPlayer;

class CBaseTower : public CBaseClickable
{
public:
	CBaseTower();
	~CBaseTower(){};

	DECLARE_CLASS( CBaseTower, CBaseClickable );
	DECLARE_NETWORKCLASS();
	
#ifdef CLIENT_DLL
	void Spawn( void );
	void ClientThink( void );
	
	//Selection Drawing Helpers
	void DrawCircle( const char *szMaterial, const Vector &origin, int radius, Vector vColor=Vector(1,1,1), float alpha=1 );
	void DrawCircle( const Vector &origin, bool buildable );
	CBaseEntity *FindByGroup( CBaseEntity *pEnt, int collisionGroup );
	void DrawSurroundingTowerPalettes(const Vector &origin);

	inline bool ShouldCheck() { return (GetOwnerEntity() == C_BasePlayer::GetLocalPlayer()); };
	
	//Add ourselves to players list of their towers.
	virtual void		OnDataChanged( DataUpdateType_t updateType );

	//Predict our origin so we don't get laggy movements!
	virtual const Vector& GetRenderOrigin( void );

	Vector m_vecRadiusCenter;
	bool m_bHasOwner;
	bool m_bDrawPalette;

	//Draw our selection/hover displays
	virtual void PreDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered );
	virtual void PostDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered );

	virtual void OnHover( CSDKPlayer *sdkPlayer );
	virtual void Selected( CSDKPlayer *sdkPlayer );

	void ClearPalettes();

#else

	virtual void FireProjectile( CBaseEntity *pEnemy ) =0;
	virtual void Spawn( void );

	virtual bool Upgrade( void );
	virtual int Sell( void );
	virtual int Cost( void );

	virtual void AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle = QAngle(0,0,0));
	virtual void UpdateTower();
	virtual void SeekEnemy();

	static CBaseTower *CreateTower( CBaseEntity *pOwner, int type )//const char *weapName )
	{
		if(type < 0 || type > MAX_TOWERS)
			return NULL;

		CBaseTower *pObject = (CBaseTower *)CreateEntityByName( g_TowerTypes[type] );//static_cast< CMKSProjectile * > (CreateEntityByName( ItemWeaponNames[curweap] ) );
		
		if(!pObject)
			return NULL;
		
		pObject->Spawn();
		pObject->SetOwnerEntity( pOwner );

		return pObject;
	}
#endif

	void BuildingPlacement( CSDKPlayer *pPlayer );

	virtual void OnBuilt( CSDKPlayer *pPlayer );
	virtual int GetClickType() { return CLICK_BUILDING; };
	void	TraceBBox( const Vector& start, const Vector& end, ITraceFilter *pFilter, trace_t& pm );

//Accessors
public:
	bool IsSelected() { return m_bSelected; };

	bool IsBuilt() { return m_bBuilt; };
	void SetBuilt( bool built ) { m_bBuilt = built; };

	float GetNextAttack()	{ return m_flNextAttackTime; };

	int GetRadius()			{ return m_iRadius;			};
	int GetLevel()			{ return m_iLevel;			};
	int GetMaxLevel()		{ return m_iMaxLevel;		};
	bool IsMaxLevel()		{ return m_iMaxLevel == m_iLevel; }
	int GetTowerType()		{ return m_iTowerType;		};
	int GetUpgradeCost()	{ return m_iUpgradeCost;	};
	int GetProjDamage()		{ return m_iDamage;			};
	int GetProjSpeed()		{ return m_iSpeed;			};


//Variables
public:
	CUtlVector< char * >m_szModels;
	CNetworkVar( int, m_iMaxLevel );

	CNetworkVar( bool, m_bSelected );
	CNetworkVar( bool, m_bBuilt );

	float m_flNextAttackTime;

	CNetworkVar( int, m_iTowerType );
	CNetworkVar( int,  m_iUpgradeCost);

	CNetworkVar( int, m_iRadius );
	CNetworkVar( int, m_iLevel );
	CNetworkVar( int, m_iDamage );
	CNetworkVar( int, m_iSpeed );

	CNetworkVar( float, m_flAttackDelay );

	CUtlVector< LevelInfo_t >m_sLevelInfo;
	bool m_bCanBuild;
	char *m_szName;
	int m_iTowerID;
};

//Create a list of selectables in this filter, don't touch anything.
class CTraceFilterSELECTABLEMULTI : public CTraceFilter
{
	DECLARE_CLASS_NOBASE( CTraceFilterSELECTABLEMULTI );

public:
	CTraceFilterSELECTABLEMULTI() {};

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
		CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );
		if(pEntity)
			if ( pEntity->GetFlags() & FL_IS_SELECTABLE )
			{
				m_pSelected.AddToTail( pEntity );
				return false;
			}

		return false;
	}
	virtual TraceType_t GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}
private:
	CUtlVector< CBaseEntity * >m_pSelected;
};

//Single target selecting
class CTraceFilterSELECTABLE : public CTraceFilter
{
	DECLARE_CLASS_NOBASE( CTraceFilterSELECTABLE );

public:
	CTraceFilterSELECTABLE() {};

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
		CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );
		if(pEntity)
			if ( pEntity->GetFlags() & FL_IS_SELECTABLE )
				return true;

		return false;
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

//Used to find ground to place our entities, and nothing else.
class CTraceFilterWORLDSPECIAL : public CTraceFilter
{
	DECLARE_CLASS_NOBASE( CTraceFilterWORLDSPECIAL );

public:
	CTraceFilterWORLDSPECIAL( IHandleEntity *pEnt ) 
	{
		m_pOriginalEnt = pEnt;
	};

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
		if(!pHandleEntity)
			return false;
		if(m_pOriginalEnt == pHandleEntity)
			return false;

		CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );
		if(pEntity)
		{
			int collision = pEntity->GetCollisionGroup();
			if( collision == COLLISION_GROUP_TOWERS_PATH )
				return true;

			if(pEntity->IsBSPModel())
				return true;
		}
		return false;

	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
private:
	IHandleEntity *m_pOriginalEnt;

};

//Used to find if our tower is in a buildable location.
class CTraceFilterCHECKBUILD : public CTraceFilter
{
	DECLARE_CLASS_NOBASE( CTraceFilterCHECKBUILD );

public:
	CTraceFilterCHECKBUILD( IHandleEntity *pEnt ) 
	{
		m_pOriginalEnt = pEnt;
		m_bCanBuild = true;
	};

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
		if(pHandleEntity == m_pOriginalEnt)
			return false;

		CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );
		if(pEntity)
		{
			int collision = pEntity->GetCollisionGroup();
			if( collision == COLLISION_GROUP_TOWERS_NOBUILD || collision == COLLISION_GROUP_TOWER )
			{
				m_bCanBuild = false;
			}

			if(collision == COLLISION_GROUP_TOWERS_PATH )
			{
				m_bCanBuild = false;
				return true;
			}

			if(pEntity->IsBSPModel())
				return true;
		}
		return false;
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
public:
	bool m_bCanBuild;
	IHandleEntity *m_pOriginalEnt;

};

inline CBaseTower *ToBaseTower( CBaseEntity *pEntity )
{
	if ( !pEntity || pEntity->GetCollisionGroup() != COLLISION_GROUP_TOWER )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CBaseTower*>( pEntity ) != 0 );
#endif
	return static_cast< CBaseTower* >( pEntity );
}
#endif //TOWER_ENTITY_H