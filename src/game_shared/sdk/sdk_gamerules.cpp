//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The TF Game rules 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "sdk_gamerules.h"
#include "ammodef.h"
#include "KeyValues.h"
#include "weapon_sdkbase.h"

#include "towers/BaseTower.h"

#ifdef GAME_DLL
	#include "voice_gamemgr.h"
	#include "team.h"
	#include "towers/towers_enemy.h"
	#include "sdk_player.h"
	#include "entitylist.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(info_player_terrorist, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_counterterrorist,CPointEntity);
#endif

REGISTER_GAMERULES_CLASS( CSDKGameRules );


BEGIN_NETWORK_TABLE_NOBASE( CSDKGameRules, DT_SDKGameRules )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iLevel ) ),
	RecvPropInt( RECVINFO( m_iLives ) ),
	RecvPropBool( RECVINFO( m_bGameStarted ) ),
#else
	SendPropBool( SENDINFO( m_bGameStarted ) ),
	SendPropInt( SENDINFO( m_iLevel ) ),
	SendPropInt( SENDINFO( m_iLives ) ),
#endif
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( sdk_gamerules, CSDKGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( SDKGameRulesProxy, DT_SDKGameRulesProxy )


#ifdef CLIENT_DLL
	void RecvProxy_SDKGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CSDKGameRules *pRules = SDKGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CSDKGameRulesProxy, DT_SDKGameRulesProxy )
		RecvPropDataTable( "sdk_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_SDKGameRules ), RecvProxy_SDKGameRules )
	END_RECV_TABLE()
#else
	void *SendProxy_SDKGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CSDKGameRules *pRules = SDKGameRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE( CSDKGameRulesProxy, DT_SDKGameRulesProxy )
		SendPropDataTable( "sdk_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_SDKGameRules ), SendProxy_SDKGameRules )
	END_SEND_TABLE()
#endif


#ifdef CLIENT_DLL
	CSDKGameRules::CSDKGameRules()
	{
		m_iLevel = 1;
		m_iLives = 10;

		int i=0;
		LevelInfo_t temp;
		for(i=0;i<MAX_TOWERS;i++)
		{
			m_pTowerInfo.AddToTail(temp);
		}
	}
#else

	// --------------------------------------------------------------------------------------------------- //
	// Voice helper
	// --------------------------------------------------------------------------------------------------- //

	class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
	{
	public:
		virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker )
		{
			// Dead players can only be heard by other dead team mates
			if ( pTalker->IsAlive() == false )
			{
				if ( pListener->IsAlive() == false )
					return ( pListener->InSameTeam( pTalker ) );

				return false;
			}

			return ( pListener->InSameTeam( pTalker ) );
		}
	};
	CVoiceGameMgrHelper g_VoiceGameMgrHelper;
	IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;



	// --------------------------------------------------------------------------------------------------- //
	// Globals.
	// --------------------------------------------------------------------------------------------------- //

	// NOTE: the indices here must match TEAM_TERRORIST, TEAM_CT, TEAM_SPECTATOR, etc.
	char *sTeamNames[] =
	{
		"Unassigned",
		"Spectator",
		"Terrorist",
		"Counter-Terrorist"
	};


	// --------------------------------------------------------------------------------------------------- //
	// Global helper functions.
	// --------------------------------------------------------------------------------------------------- //

	// Helper function to parse arguments to player commands.
	const char* FindEngineArg( const char *pName )
	{
		int nArgs = engine->Cmd_Argc();
		for ( int i=1; i < nArgs; i++ )
		{
			if ( stricmp( engine->Cmd_Argv(i), pName ) == 0 )
				return (i+1) < nArgs ? engine->Cmd_Argv(i+1) : "";
		}
		return 0;
	}


	int FindEngineArgInt( const char *pName, int defaultVal )
	{
		const char *pVal = FindEngineArg( pName );
		if ( pVal )
			return atoi( pVal );
		else
			return defaultVal;
	}

	// FindInList searches the provided array for the compare string
	// we use it to check against our list of preserved entities (s_PreserveEnts)
	bool FindInList(const char *s_List[], const char *compare)
	{
	int index = 0;

	while(s_List[index])
	{
		if ( Q_strcmp(s_List[index], compare) == 0 )
			return true;

			index++;
	}

	return false;
	}
	
	// World.cpp calls this but we don't use it in SDK.
	void InitBodyQue()
	{
	}


	// --------------------------------------------------------------------------------------------------- //
	// CSDKGameRules implementation.
	// --------------------------------------------------------------------------------------------------- //

	CSDKGameRules::CSDKGameRules()
	{
		//CSDKGameRulesProxy *pTemp = new CSDKGameRulesProxy();
		// Create the team managers
		for ( int i = 0; i < ARRAYSIZE( sTeamNames ); i++ )
		{
			CTeam *pTeam = static_cast<CTeam*>(CreateEntityByName( "sdk_team_manager" ));
			pTeam->Init( sTeamNames[i], i );

			g_Teams.AddToTail( pTeam );
		}

		m_bGameStarted = false;
		m_bNewLevel = false;
		m_iEnemyMaxCount = 8;
		m_iEnemyAmount = 0;
		m_iEnemySpawnCount = 0;
		m_flEnemyIntervalDelay = 1.3f;
		m_flEnemyRespawnDelay = 4.0f;
		m_flEnemyIntervalCurrent = 0.0f;
		m_flEnemyRespawnCurrent = 0.0f;
		m_iLevel = 1;
		m_iLives = 10;

		m_pTowerInfo.SetCount( MAX_TOWERS );
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	CSDKGameRules::~CSDKGameRules()
	{
		// Note, don't delete each team since they are in the gEntList and will 
		// automatically be deleted from there, instead.
		g_Teams.Purge();
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  :
	// Output :
	//-----------------------------------------------------------------------------
	bool CSDKGameRules::ClientCommand( const char *pcmd, CBaseEntity *pEdict )
	{
		// Is the client spawned yet?
		if ( !pEdict )
			return true;
		
		if ( FStrEq( pcmd, "startgame" ) )
		{
			BeginGame();
			return true;
		}
		else
		{
			CSDKPlayer *sdkPlayer = ToSDKPlayer( pEdict );
			if(sdkPlayer)
				return sdkPlayer->ClientCommand( pcmd );
		}
		return BaseClass::ClientCommand( pcmd, pEdict );
	}

	//-----------------------------------------------------------------------------
	// Purpose: Player has just spawned. Equip them.
	//-----------------------------------------------------------------------------

	void CSDKGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore )
	{
		RadiusDamage( info, vecSrcIn, flRadius, iClassIgnore, false );
	}

	// Add the ability to ignore the world trace
	void CSDKGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld )
	{
		CBaseEntity *pEntity = NULL;
		trace_t		tr;
		float		flAdjustedDamage, falloff;
		Vector		vecSpot;
		Vector		vecToTarget;
		Vector		vecEndPos;

		Vector vecSrc = vecSrcIn;

		if ( flRadius )
			falloff = info.GetDamage() / flRadius;
		else
			falloff = 1.0;

		int bInWater = (UTIL_PointContents ( vecSrc ) & MASK_WATER) ? true : false;
		
		vecSrc.z += 1;// in case grenade is lying on the ground

		// iterate on all entities in the vicinity.
		for ( CEntitySphereQuery sphere( vecSrc, flRadius ); ( pEntity = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
		{
			if ( pEntity->m_takedamage != DAMAGE_NO )
			{
				// UNDONE: this should check a damage mask, not an ignore
				if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
				{// houndeyes don't hurt other houndeyes with their attack
					continue;
				}

				// blast's don't tavel into or out of water
				if (bInWater && pEntity->GetWaterLevel() == 0)
					continue;
				if (!bInWater && pEntity->GetWaterLevel() == 3)
					continue;

				// radius damage can only be blocked by the world
				vecSpot = pEntity->BodyTarget( vecSrc );



				bool bHit = false;

				if( bIgnoreWorld )
				{
					vecEndPos = vecSpot;
					bHit = true;
				}
				else
				{
					UTIL_TraceLine( vecSrc, vecSpot, MASK_SOLID_BRUSHONLY, info.GetInflictor(), COLLISION_GROUP_NONE, &tr );

					if (tr.startsolid)
					{
						// if we're stuck inside them, fixup the position and distance
						tr.endpos = vecSrc;
						tr.fraction = 0.0;
					}

					vecEndPos = tr.endpos;

					if( tr.fraction == 1.0 || tr.m_pEnt == pEntity )
					{
						bHit = true;
					}
				}

				if ( bHit )
				{
					// the explosion can 'see' this entity, so hurt them!
					//vecToTarget = ( vecSrc - vecEndPos );
					vecToTarget = ( vecEndPos - vecSrc );

					// decrease damage for an ent that's farther from the bomb.
					flAdjustedDamage = vecToTarget.Length() * falloff;
					flAdjustedDamage = info.GetDamage() - flAdjustedDamage;
				
					if ( flAdjustedDamage > 0 )
					{
						CTakeDamageInfo adjustedInfo = info;
						adjustedInfo.SetDamage( flAdjustedDamage );

						Vector dir = vecToTarget;
						VectorNormalize( dir );

						// If we don't have a damage force, manufacture one
						if ( adjustedInfo.GetDamagePosition() == vec3_origin || adjustedInfo.GetDamageForce() == vec3_origin )
						{
							CalculateExplosiveDamageForce( &adjustedInfo, dir, vecSrc, 1.5	/* explosion scale! */ );
						}
						else
						{
							// Assume the force passed in is the maximum force. Decay it based on falloff.
							float flForce = adjustedInfo.GetDamageForce().Length() * falloff;
							adjustedInfo.SetDamageForce( dir * flForce );
							adjustedInfo.SetDamagePosition( vecSrc );
						}

						pEntity->TakeDamage( adjustedInfo );
			
						// Now hit all triggers along the way that respond to damage... 
						pEntity->TraceAttackToTriggers( adjustedInfo, vecSrc, vecEndPos, dir );
					}
				}
			}
		}
	}

	void CSDKGameRules::CreateStandardEntities()
	{
		BaseClass::CreateStandardEntities();
		CBaseEntity::Create( "sdk_gamerules", vec3_origin, vec3_angle );
	}

	void CSDKGameRules::Precache( void )
	{
		int i;
		
		
		for(i=0;i<MAX_FIRE_MODELS;i++)
			CBaseEntity::PrecacheModel( g_FireModels[i] );

		for(i=0;i<MAX_ICE_MODELS;i++)
			CBaseEntity::PrecacheModel( g_IceModels[i] );

		for(i=0;i<MAX_SUPER_MODELS;i++)
			CBaseEntity::PrecacheModel( g_SuperModels[i] );
	}

	ConVar enemy_health( "st_enemy_health", "3.25" );
	ConVar enemy_multiplier( "st_enemy_multiplier", "0" );

	void CSDKGameRules::Think()
	{
		if(m_bGameStarted)
		{
			float gametime = gpGlobals->curtime;
			if(m_bNewLevel)
				if(m_iEnemySpawnCount < m_iEnemyMaxCount)
				{
					if(m_flEnemyRespawnCurrent < gametime)
					{
						if( m_flEnemyIntervalCurrent < gametime )
						{
							int hp = (int)(enemy_health.GetFloat() * (float)(m_iLevel*m_iLevel*m_iLevel));
							//float amt = ((float)m_iLevel / 50.0f) * enemy_multiplier.GetFloat();
							

							//if( m_iLevel >= 50 )
							//{
							//	float diff = ((m_iLevel-49) / 5);
							//	if(diff <= 0)
							//		diff = 1.0f;
							//	hp += (int)amt * diff;
							//}
							//else
							//	hp += (int)amt;

							CTowersEnemy *pEnt = CTowersEnemy::CreateEnemy( hp );
							if(pEnt)
							{
								m_flEnemyIntervalCurrent = gametime + m_flEnemyIntervalDelay;
								m_iEnemySpawnCount++;
							}
						}
					}
				}
				else
				{
					m_bNewLevel = false;
				}
		}
		BaseClass::Think();
	}

	ConVar towers_startmoney( "st_startmoney", "400" );
	void CSDKGameRules::BeginGame( void )
	{
		

		CleanMap();
		
		//sdkPlayer->m_iMoney = towers_startmoney.GetInt();
		m_bGameStarted = true;
		m_bNewLevel = true;
		m_iLevel = 1;
		m_iEnemyMaxCount = 8;
		m_iEnemyAmount = m_iEnemyMaxCount;
		m_iEnemySpawnCount = 0;
		m_flEnemyIntervalDelay = 1.3f;
		m_flEnemyRespawnDelay = 4.0f;
		m_flEnemyIntervalCurrent = gpGlobals->curtime + m_flEnemyIntervalDelay;
		m_flEnemyRespawnCurrent = gpGlobals->curtime + m_flEnemyRespawnDelay;
		m_iLives = 10;

		int index;
		for ( index = 1; index <= gpGlobals->maxClients; ++index )
		{
			CSDKPlayer *pPlayer =	ToSDKPlayer( UTIL_PlayerByIndex( index ) );

			if ( !pPlayer )
				continue;

			pPlayer->SetMoney( towers_startmoney.GetInt() );
		}
	}
	void CSDKGameRules::EndGame( void )
	{
		m_bGameStarted = false;
		
	}

	ConVar enemy_money( "st_enemy_money", "10" );
	ConVar money_interest_rate( "st_interest_rate", "0.1" );
	void CSDKGameRules::EnemyKilled( CBaseEntity *player )
	{
		m_iEnemyAmount--;
		if(m_iEnemyAmount < 1)
		{
			m_bNewLevel = true;
			//m_iLevel = 250;
			m_iLevel++;
			m_iEnemyAmount = m_iEnemyMaxCount; //Just make sure we're not called more than once.
			m_iEnemySpawnCount = 0;
			m_flEnemyRespawnCurrent = gpGlobals->curtime + 2.0f;//m_flEnemyRespawnDelay;
		}

		if( player == NULL )
		{
			m_iLives--;
			NetworkStateChanged();
		}

		//MONEY!!!
		int index;
		CSDKPlayer *sdkPlayer = NULL;
		for ( index = 1; index <= gpGlobals->maxClients; ++index )
		{
			sdkPlayer =	ToSDKPlayer( UTIL_PlayerByIndex( index ) );
			if ( !sdkPlayer )
				continue;

			sdkPlayer->SetLevel( m_iLevel );
			//if(m_iLevel > 50)
			//	sdkPlayer->m_iMoney += 50 + ((m_iLevel-50) / 2);
			//else
			//{
				//if(m_iLevel < 6)
				//	sdkPlayer->m_iMoney += 6;
				//else
			sdkPlayer->AddMoney( (m_iLevel * enemy_money.GetInt()) );
			//}
		}
		
	}

	void CSDKGameRules::PlayerSpawn( CBasePlayer *pPlayer )
	{
		//CSDKPlayer *sdkPlayer = ToSDKPlayer( pPlayer );
		//if(sdkPlayer->m_iMoney == -1 && sdkPlayer->m_pTowers.Count() == 0)
		//	sdkPlayer->m_iMoney = towers_startmoney.GetInt();
	
		
		if(m_pTowerInfo.Count() != MAX_TOWERS)
			return;

		CRecipientFilter filter;
		filter.AddRecipient( pPlayer );
		filter.MakeReliable();

		UserMessageBegin( filter, "TowerPrices" );

		WRITE_BYTE( 4 );

		WRITE_SHORT( m_pTowerInfo[EARTH_TOWER].upgradecost );
		WRITE_SHORT(  m_pTowerInfo[FIRE_TOWER].upgradecost );
		WRITE_SHORT(  m_pTowerInfo[ICE_TOWER].upgradecost );
		WRITE_SHORT(  m_pTowerInfo[SUPER_TOWER].upgradecost );
		
		MessageEnd();
		
	}

	void CSDKGameRules::ClientDisconnected( edict_t *pClient )
	{
		if ( pClient )
		{
			CSDKPlayer *sdkPlayer = (CSDKPlayer *)CBaseEntity::Instance( pClient );

			if ( sdkPlayer )
			{
				int i;
				for(i=0;i<sdkPlayer->m_pTowers.Count();i++)
					if(sdkPlayer->m_pTowers[i])
						UTIL_Remove( sdkPlayer->m_pTowers[i] );
			}
		}
		BaseClass::ClientDisconnected( pClient );
	}

	//Create all towers, capture their first level info, then destroy immediately.
	//Break out if we failed at making one.
	bool CSDKGameRules::QueryTowerInfo()
	{
		int i;
		CBaseTower *pTemp = NULL;
		LevelInfo_t info;
		for(i=0;i<MAX_TOWERS;i++)
		{
			pTemp = CBaseTower::CreateTower( NULL, i );
			if(!pTemp)
				return false;

			info.upgradecost = pTemp->m_iUpgradeCost;
			m_pTowerInfo.AddToTail( info ); //First level info.
			pTemp->Remove(); 
		}
		return true;
	}

	bool CSDKGameRules::GetTowerInfo( int tower, int &cost, int &dmg, int &radius )
	{
		if(tower < 0 || tower >= MAX_TOWERS)
			return false;
		
		cost = m_pTowerInfo[tower].upgradecost;
		dmg = m_pTowerInfo[tower].damage;
		radius = m_pTowerInfo[tower].radius;
		return true;
	}

	void CSDKGameRules::CleanMap()
	{
		// Recreate all the map entities from the map data (preserving their indices),
		// then remove everything else except the players.

		// Get rid of all entities except players.
		CBaseEntity *pCur = gEntList.FirstEnt();
		while ( pCur )
		{
			if ( !FindInList( s_PreserveEnts, pCur->GetClassname() ) )
				UTIL_Remove( pCur );
			pCur = gEntList.NextEnt( pCur );
		}

		// Really remove the entities so we can have access to their slots below.
		gEntList.CleanupDeleteList();


		// Now reload the map entities.
	class CMapEntityFilter : public IMapEntityFilter
	{
		public:
			virtual bool ShouldCreateEntity( const char *pClassname )
			{
				// Don't recreate the preserved entities.
				if ( !FindInList( s_PreserveEnts, pClassname ) )
					return true;
				else
				{
					// Increment our iterator since it's not going to call CreateNextEntity for this ent.
					if ( m_iIterator != g_MapEntityRefs.InvalidIndex() )
						m_iIterator = g_MapEntityRefs.Next( m_iIterator );
					return false;
				}
			}
			virtual CBaseEntity* CreateNextEntity( const char *pClassname )
			{
			if ( m_iIterator == g_MapEntityRefs.InvalidIndex() )
			{
				// We should never reach this point - g_MapEntityRefs should have been filled
				// when we loaded the map due to the use of CHDNMapLoadFilter, but we cover ourselves by checking here.
				Assert( m_iIterator != g_MapEntityRefs.InvalidIndex() );
				return NULL;
			}
			else
			{
				CMapEntityRef &ref = g_MapEntityRefs[m_iIterator];
				m_iIterator = g_MapEntityRefs.Next( m_iIterator ); // Seek to the next entity.
				if ( ref.m_iEdict == -1 || engine->PEntityOfEntIndex( ref.m_iEdict ) )
					// the entities previous edict has been used for whatever reason,
					// so just create it and use any spare edict slot
					return CreateEntityByName( pClassname );
				else
					// The entity's edict slot was free, so we put it back where it came from.
					return CreateEntityByName( pClassname, ref.m_iEdict );
			}
			}
		public:
			int m_iIterator; // Iterator into g_MapEntityRefs.
	};

		CMapEntityFilter filter;
		filter.m_iIterator = g_MapEntityRefs.Head();

		// final task, trigger the recreation of any entities that need it.
		MapEntity_ParseAllEntities( engine->GetMapEntitiesString(), &filter, true );
	}

#endif


bool CSDKGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		swap(collisionGroup0,collisionGroup1);
	}
	
	//Don't stand on COLLISION_GROUP_WEAPON
	if( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT &&
		collisionGroup1 == COLLISION_GROUP_WEAPON )
	{
		return false;
	}

	//TOWERS CUSTOM COLLISION
	if(collisionGroup0 == COLLISION_GROUP_TOWER_PROJECTILE && collisionGroup1 == COLLISION_GROUP_TOWER_PROJECTILE)
		return false;

	bool canProjHit = (collisionGroup0 == COLLISION_GROUP_TOWER 
					|| collisionGroup0 == COLLISION_GROUP_PLAYER
					|| collisionGroup0 == COLLISION_GROUP_TOWERS_NOBUILD );

	if(canProjHit && collisionGroup1 == COLLISION_GROUP_TOWER_PROJECTILE )
		return false;

	bool canPlayerHit = (collisionGroup1 == COLLISION_GROUP_TOWERS_NOBUILD ||
						collisionGroup1 == COLLISION_GROUP_TOWER ||
						collisionGroup1 == COLLISION_GROUP_TOWERS_ENEMY || 
						collisionGroup1 == COLLISION_GROUP_TOWER_PROJECTILE );

	if( (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 ==COLLISION_GROUP_PLAYER_MOVEMENT ) 
		&& canPlayerHit )
		return false;

	if( collisionGroup0 == COLLISION_GROUP_TOWERS_NOBUILD && collisionGroup1 == COLLISION_GROUP_TOWERS_ENEMY )
		return false;
	if( collisionGroup0 == COLLISION_GROUP_TOWER && collisionGroup1 == COLLISION_GROUP_TOWERS_ENEMY )
		return false;
	if( collisionGroup0 == COLLISION_GROUP_TOWERS_ENEMY && collisionGroup1 == COLLISION_GROUP_TOWERS_ENEMY)
		return false;
	
	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 ); 
}


//-----------------------------------------------------------------------------
// Purpose: Init CS ammo definitions
//-----------------------------------------------------------------------------

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			1	

// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)


CAmmoDef* GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;

	if ( !bInitted )
	{
		bInitted = true;
		
		// def.AddAmmoType( BULLET_PLAYER_50AE,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_50AE_max",		2400, 0, 10, 14 );
		def.AddAmmoType( AMMO_GRENADE, DMG_BLAST, TRACER_LINE, 0, 0,	1/*max carry*/, 1, 0 );
		def.AddAmmoType( AMMO_BULLETS, DMG_BULLET, TRACER_LINE, 0, 0,	1/*max carry*/, 1, 0 );
	}

	return &def;
}


#ifndef CLIENT_DLL

const char *CSDKGameRules::GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer )
{
	return "";
}

#endif

//-----------------------------------------------------------------------------
// Purpose: Find the relationship between players (teamplay vs. deathmatch)
//-----------------------------------------------------------------------------
int CSDKGameRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
#ifndef CLIENT_DLL
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() || IsTeamplay() == false )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
		return GR_TEAMMATE;

#endif

	return GR_NOTTEAMMATE;
}
