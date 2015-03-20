#include "cbase.h"
#include "towers/BaseTower.h"
#include "towers/BaseProjectile.h"
#include "sdk_gamerules.h"


class CEarthTower : public CBaseTower
{
public:
	DECLARE_CLASS( CEarthTower, CBaseTower );

	CEarthTower() {};
	~CEarthTower() {};

	DECLARE_DATADESC();

	void Precache();
	void Spawn();
	void EarthThink();
	virtual void FireProjectile( CBaseEntity *pEnt );
};

LINK_ENTITY_TO_CLASS( earth_tower, CEarthTower );

BEGIN_DATADESC( CEarthTower )
	DEFINE_THINKFUNC( EarthThink ),
END_DATADESC()

PRECACHE_REGISTER( earth_tower );

void CEarthTower::Precache(void)
{
	//AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle = QAngle(0,0,0));
	AddLevel( g_EarthModels[0], 85,		750,	200,	2200, 0.50 );;;
	AddLevel( g_EarthModels[1], 290,	850,	500,	2200, 0.48, QAngle(0,0,90) );
	AddLevel( g_EarthModels[2], 590,	1100,	800,	2200, 0.45, QAngle(0,90,0) );
	AddLevel( g_EarthModels[3], 1000,	1550,	1200,	2200, 0.40, QAngle(0,0,0) );
	AddLevel( g_EarthModels[4], 1300,	2000,	1500,	2200, 0.25, QAngle(0,90,0) );;

	//Give our GameRules the data for Level 1
	SDKGameRules()->m_pTowerInfo[EARTH_TOWER] = m_sLevelInfo[1];

	for(int i=0;i<MAX_EARTH_MODELS;i++)
		CBaseEntity::PrecacheModel( g_EarthModels[i] );

	BaseClass::Precache();
}
void CEarthTower::Spawn()
{
	Precache();

	m_iTowerType = EARTH_TOWER;

	BaseClass::Spawn();

	SetThink( &CEarthTower::EarthThink );
	SetNextThink( gpGlobals->curtime + 0.05f );
}

//Do any extra effects here!
void CEarthTower::EarthThink()
{
	SetNextThink( gpGlobals->curtime + 0.05f );;;
	CBaseHandle index = GetRefEHandle();
	Assert( (1<<12) == 4096 );
	SeekEnemy();;;
}

void CEarthTower::FireProjectile( CBaseEntity *pEnemy )
{
	CBaseProjectile *pProj = CBaseProjectile::CreateProjectile( GetOwnerEntity(), pEnemy, (CBaseProjectile*)CreateEntityByName( "proj_earth" ), (float)m_iDamage, (float)m_iSpeed, m_iRadius );
	pProj->SetAbsOrigin( GetAbsOrigin() + Vector(0,0,WorldAlignMaxs().z) );
}