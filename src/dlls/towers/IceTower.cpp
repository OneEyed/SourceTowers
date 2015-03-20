#include "cbase.h"
#include "towers/BaseTower.h"
#include "towers/BaseProjectile.h"
#include "sdk_gamerules.h"

class CIceTower : public CBaseTower
{
public:
	DECLARE_CLASS( CIceTower, CBaseTower );

	CIceTower() {};
	~CIceTower() {};

	DECLARE_DATADESC();

	void Precache();
	void Spawn();
	void IceThink();
	void UpdateTower( void );
	virtual void FireProjectile( CBaseEntity *pEnt );
};

LINK_ENTITY_TO_CLASS( ice_tower, CIceTower );

BEGIN_DATADESC( CIceTower )
	DEFINE_THINKFUNC( IceThink ),
END_DATADESC()

PRECACHE_REGISTER( ice_tower );

void CIceTower::Precache(void)
{
	//AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle = QAngle(0,0,0));
	AddLevel( g_IceModels[0], 75,	800,	220,	2200, 0.57 );
	AddLevel( g_IceModels[1], 270,	950,	530,	2200, 0.565, QAngle(90,0,0) );
	AddLevel( g_IceModels[2], 580,	1200,	850,	2250, 0.56 );
	AddLevel( g_IceModels[3], 1100,	1800,	1500,	2000, 0.6 );
	AddLevel( g_IceModels[4], 1200,	2400,	3000,	1950, 0.65 );

	//Give our GameRules the data for Level 1
	SDKGameRules()->m_pTowerInfo[ICE_TOWER] = m_sLevelInfo[1];

	for(int i=0;i<MAX_ICE_MODELS;i++)
		CBaseEntity::PrecacheModel( g_IceModels[i] );

	BaseClass::Precache();
}
void CIceTower::Spawn()
{
	Precache();

	m_iTowerType = ICE_TOWER;

	BaseClass::Spawn();

	SetThink( &CIceTower::IceThink );
	SetNextThink( gpGlobals->curtime + 0.05f );
}

void CIceTower::UpdateTower( void )
{
	BaseClass::UpdateTower();;;;

	if(m_iLevel == m_iMaxLevel)
	{
		Vector origin = GetAbsOrigin();;
		Vector temp = origin - Vector(0,0,MAX_TRACE_LENGTH);
		trace_t trace;
		CTraceFilterWORLDSPECIAL filter( this );
		UTIL_TraceLine( origin, temp, MASK_SOLID_BRUSHONLY, &filter, &trace );

		SetAbsOrigin( trace.endpos - Vector(0,0,150) );// + Vector(0,0,500.0f) );
	}
}
//Do any extra movement types here!
void CIceTower::IceThink()
{
	SetNextThink( gpGlobals->curtime + 0.05f );;

	SeekEnemy();
}

void CIceTower::FireProjectile( CBaseEntity *pEnemy )
{
	CBaseProjectile *pProj = CBaseProjectile::CreateProjectile( GetOwnerEntity(), pEnemy, (CBaseProjectile*)CreateEntityByName( "proj_ice" ), (float)m_iDamage, (float)m_iSpeed, m_iRadius );
	pProj->SetAbsOrigin( GetAbsOrigin() + Vector(0,0,WorldAlignMaxs().z) );
}