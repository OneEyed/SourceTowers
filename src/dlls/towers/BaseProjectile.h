#ifndef TOWER_PROJECTILE_H
#define TOWER_PROJECTILE_H

#include "towers/tower_shareddef.h"

void ConvergeNumber( float &cur, float goal, float speed );

class CBaseProjectile : public CBaseAnimating
{
public:

	DECLARE_CLASS( CBaseProjectile, CBaseAnimating );
	DECLARE_DATADESC();

	CBaseProjectile() 
	{	
		m_flSpeed = 0.0f;
		m_flDamage = 0.0f;
	};
	~CBaseProjectile () {};

	virtual void Spawn( void );
	void ProjTouch( CBaseEntity *pOther );
	//void ProjThink();

	void FollowTarget();
	virtual void FindNewTarget();
	void BlowUp();

	float GetDamage() { return m_flDamage; };

	static CBaseProjectile *CreateProjectile( CBaseEntity *pOwner, CBaseEntity *pTarget, CBaseProjectile *pObject, float dmg, float speed, int radius )//const char *weapName )
	{
		if(!pObject)
			return NULL;

		pObject->m_flDamage = dmg;
		pObject->m_flSpeed = speed;
		pObject->m_iRadius = radius;
		pObject->m_pTarget = pTarget;
		pObject->SetOwnerEntity( pOwner );

		pObject->Spawn();
		
		return pObject;
	}

	float m_flSpeed;
	float m_flDamage;
	int m_iRadius;
	float m_flTimeToLive;
	CHandle< CBaseEntity > m_pTarget;
	
};

inline CBaseProjectile *ToProjectile( CBaseEntity *pEntity )
{
	if ( !pEntity || pEntity->GetCollisionGroup() != COLLISION_GROUP_TOWER_PROJECTILE )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CBaseProjectile*>( pEntity ) != 0 );
#endif
	return static_cast< CBaseProjectile* >( pEntity );
}
#endif //TOWER_PROJECTILE_H