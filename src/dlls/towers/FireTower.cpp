#include "cbase.h"
#include "towers/BaseTower.h"
#include "towers/BaseProjectile.h"
#include "sdk_gamerules.h"

class CFireTower : public CBaseTower
{
public:
	DECLARE_CLASS( CFireTower, CBaseTower );

	CFireTower() {};
	~CFireTower() {};

	DECLARE_DATADESC();

	void Precache();
	void Spawn();
	void FireThink();
	virtual void FireProjectile( CBaseEntity *pEnt );
};

LINK_ENTITY_TO_CLASS( fire_tower, CFireTower );

BEGIN_DATADESC( CFireTower )
	DEFINE_THINKFUNC( FireThink ),
END_DATADESC()

PRECACHE_REGISTER( fire_tower );

void CFireTower::Precache(void)
{
	//AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle = QAngle(0,0,0));
	AddLevel( g_FireModels[0], 100,		750,	300,		1900, 0.7 );
	AddLevel( g_FireModels[1], 300,		800,	700,	1900, 0.68, QAngle(0,0,90) );
	AddLevel( g_FireModels[2], 600,		850,	1100,	1900, 0.66, QAngle(0,90,0) );
	AddLevel( g_FireModels[3], 1400,	900,	3200,	1900, 0.64, QAngle(0,0,90) );
	AddLevel( g_FireModels[4], 1600,	1100,	6200,	1900, 0.62, QAngle(0,90,0) );

	//Give our GameRules the data for Level 1
	SDKGameRules()->m_pTowerInfo[FIRE_TOWER] = m_sLevelInfo[1];

	for(int i=0;i<MAX_FIRE_MODELS;i++)
		CBaseEntity::PrecacheModel( g_FireModels[i] );

	BaseClass::Precache();
}

void CFireTower::Spawn()
{
	Precache();

	m_iTowerType = FIRE_TOWER;

	BaseClass::Spawn();

	SetThink( &CFireTower::FireThink );
	SetNextThink( gpGlobals->curtime + 0.05f );
}

//Do any extra movement types here!
void CFireTower::FireThink()
{
	SetNextThink( gpGlobals->curtime + 0.05f );;

	SeekEnemy();
}

void CFireTower::FireProjectile( CBaseEntity *pEnemy )
{
	CBaseProjectile *pProj = CBaseProjectile::CreateProjectile( GetOwnerEntity(), pEnemy, (CBaseProjectile*)CreateEntityByName( "proj_fire" ), (float)m_iDamage, (float)m_iSpeed, m_iRadius );
	pProj->SetAbsOrigin( GetAbsOrigin() + Vector(0,0,WorldAlignMaxs().z) );
}