#ifndef TOWERS_ENEMY_H
#define TOWERS_ENEMY_H

#ifdef CLIENT_DLL
#define CTowersEnemy C_TowersEnemy
class C_SDKPlayer;
#endif

#include "BaseClickable.h"

class CTowersEnemy : public CBaseClickable
{
public:
	DECLARE_CLASS( CTowersEnemy, CBaseClickable );
	DECLARE_NETWORKCLASS();


	int GetClickType() { return CLICK_UNIT; };
	//Lets scale our enemy to make them look supreme!
#ifdef CLIENT_DLL

	virtual void ApplyBoneMatrixTransform( matrix3x4_t& transform );
	virtual void OnHover( C_SDKPlayer *sdkPlayer );
	virtual void Selected( C_SDKPlayer *sdkPlayer );
	void Spawn();
	void ClientThink();

	//void ConvergeNumber( float &cur, float goal, float speed );

#else

	DECLARE_DATADESC();

	CTowersEnemy() { m_iCurrentPos = 0; };
	~CTowersEnemy() {};

	void Spawn( void );
	void Precache( void );

	void EnemyThink( void );
	void EnemyTouch( CBaseEntity *pOther );

	void BlowUp( void );
	void TakeDamage(  CBaseEntity *pAttacker, int damage );
	static CTowersEnemy *CreateEnemy( int health )//const char *weapName )
	{
		CTowersEnemy *pObject = (CTowersEnemy *)CreateEntityByName( "towers_enemy" );//static_cast< CMKSProjectile * > (CreateEntityByName( ItemWeaponNames[curweap] ) );
		
		if(!pObject)
			return NULL;
		
		pObject->Spawn();
		pObject->m_iCurrentPos = 0;
		pObject->SetMaxHealth( health );
		pObject->SetHealth( health );
		return pObject;
	}
private:
	
	int m_iCurrentPos;
#endif
};

#endif //TOWERS_ENEMY_H