#include "cbase.h"
#include "towers/BaseTower.h"
#include "beam_shared.h"
#include "towers/towers_enemy.h"
#include "sdk_gamerules.h"

#define PHYSBEAM_SPRITE "sprites/physbeam.vmt"
#define LIGHTNING_SPRITE "sprites/lgtning.vmt"
class CSuperTower : public CBaseTower
{
public:
	DECLARE_CLASS( CSuperTower, CBaseTower );

	CSuperTower() {};
	~CSuperTower() {};

	DECLARE_DATADESC();

	void Precache();
	void Spawn();
	void SuperThink();
	virtual void FireProjectile( CBaseEntity *pEnt );
};

LINK_ENTITY_TO_CLASS( super_tower, CSuperTower );

BEGIN_DATADESC( CSuperTower )
	DEFINE_THINKFUNC( SuperThink ),
END_DATADESC()

PRECACHE_REGISTER( super_tower );

void CSuperTower::Precache(void)
{
	//AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle = QAngle(0,0,0));
	AddLevel( g_SuperModels[0], 8000,	700,	8000,	1800, 0.9f );
	AddLevel( g_SuperModels[1], 13000,	800,	45000,	1800, 0.9f );
	AddLevel( g_SuperModels[2], 30000,	1000,	120000,	1800, 0.9f );

	//Give our GameRules the data for Level 1
	SDKGameRules()->m_pTowerInfo[SUPER_TOWER] = m_sLevelInfo[1];

	for(int i=0;i<MAX_SUPER_MODELS;i++)
		CBaseEntity::PrecacheModel( g_SuperModels[i] );

	BaseClass::Precache();

	PrecacheModel( PHYSBEAM_SPRITE );
	PrecacheModel( LIGHTNING_SPRITE );;
}

void CSuperTower::Spawn()
{
	Precache();	

	m_iTowerType = SUPER_TOWER;
	//m_iProjectileType = (int)PROJ_EARTH;

	BaseClass::Spawn();

	SetThink( &CSuperTower::SuperThink );
	SetNextThink( gpGlobals->curtime + 0.05f );
}

//Do any extra movement types here!
void CSuperTower::SuperThink()
{
	SetNextThink( gpGlobals->curtime + 0.05f );

	SeekEnemy();
}

void CSuperTower::FireProjectile( CBaseEntity *pEnt )
{
	float maxsz = WorldAlignMaxs().z;
	Vector myOrig = GetAbsOrigin() + Vector(0,0,maxsz);// + Vector(0,0,WorldAlignMaxs().z - );;;
	Vector enemyOrig = pEnt->GetAbsOrigin();

	//Draw the main beam shaft
	CBeam *pBeam = CBeam::BeamCreate( PHYSBEAM_SPRITE, 30 );
	
	pBeam->SetStartPos( myOrig );
	pBeam->SetEndPos( enemyOrig );
	//pBeam->SetEndAttachment( LookupAttachment("Muzzle") );
	//pBeam->SetWidth( 25 );
	//pBeam->SetEndWidth( 5.0f );;
	pBeam->SetBrightness( 255 );
	pBeam->SetColor( 150, 180, 255 );
	pBeam->RelinkBeam();
	pBeam->SetNoise( 10.0f );
	pBeam->LiveForTime( 0.3f );

	//Draw electric bolts along shaft
	pBeam = CBeam::BeamCreate( LIGHTNING_SPRITE, 10.0f );
	
	pBeam->SetStartPos( myOrig );
	pBeam->SetEndPos( enemyOrig );
	//pBeam->SetEndAttachment( LookupAttachment("Muzzle") );
	//pBeam->SetWidth( 25 );
	//pBeam->SetEndWidth( 5.0f );;
	pBeam->SetBrightness( 255 );
	pBeam->SetColor( 150, 180, 255 );
	pBeam->RelinkBeam();
	pBeam->SetNoise( 15.0f );
	pBeam->LiveForTime( 0.3f );

	if(pEnt->GetCollisionGroup() == COLLISION_GROUP_TOWERS_ENEMY)
	{
		CTowersEnemy *pEnemy = static_cast< CTowersEnemy *>(pEnt);
		pEnemy->TakeDamage( GetOwnerEntity(), m_iDamage );
	}
}