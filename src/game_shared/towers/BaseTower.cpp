
#include "cbase.h"
#include "towers/BaseTower.h"
//#include "towers/BaseProjectile.h"
#include "engine/ivdebugoverlay.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#include "c_sdk_player.h"
#else
#include "sdk_player.h"
#endif

ConVar tower_sellpercent( "st_tower_sellpercent", "0.4", FCVAR_REPLICATED );

IMPLEMENT_NETWORKCLASS_ALIASED( BaseTower, DT_BaseTower )

BEGIN_NETWORK_TABLE( CBaseTower, DT_BaseTower )
#ifdef CLIENT_DLL 
	RecvPropBool	(RECVINFO(m_bBuilt)),
	RecvPropBool	(RECVINFO( m_bSelected )),
	RecvPropInt		(RECVINFO(m_iMaxLevel)),
	RecvPropInt		(RECVINFO(m_iLevel)),
	RecvPropInt		(RECVINFO(m_iRadius)),
	RecvPropInt		(RECVINFO(m_iUpgradeCost)),
	RecvPropInt		(RECVINFO(m_iTowerType)),
	RecvPropInt( RECVINFO(m_iDamage)),
	RecvPropInt( RECVINFO(m_iSpeed)),
	RecvPropFloat( RECVINFO(m_flAttackDelay)),
#else
	SendPropBool( SENDINFO( m_bBuilt ) ),
	SendPropBool( SENDINFO( m_bSelected ) ),
	SendPropInt( SENDINFO( m_iRadius ) ),
	SendPropInt( SENDINFO( m_iMaxLevel ) ),
	SendPropInt( SENDINFO( m_iLevel ) ),
	SendPropInt( SENDINFO( m_iUpgradeCost ) ),
	SendPropInt( SENDINFO( m_iTowerType ) ),
	SendPropInt( SENDINFO( m_iDamage )),
	SendPropInt( SENDINFO( m_iSpeed )),
	SendPropFloat( SENDINFO( m_flAttackDelay ), 32,	SPROP_NOSCALE),
#endif
END_NETWORK_TABLE()

CBaseTower::CBaseTower() : CBaseClickable()
{
	m_bCanBuild = false;
	m_bSelected = false;
	m_bBuilt = false;
	m_iRadius = 300;
	m_iLevel = 1;
	m_iTowerType = -1;
	m_iDamage = 0;
	m_iSpeed = 0;
	m_flNextAttackTime = 0.0f;
	m_flAttackDelay = 0.0f;
	m_iMaxLevel = 0;
	m_iTowerID = 0;

#ifdef CLIENT_DLL
	m_bDrawPalette = false;
	m_vecRadiusCenter.Init();
#endif

	LevelInfo_t dummy;
	m_sLevelInfo.AddToTail( dummy );
	m_szModels.AddToTail( "NULL" );
}


#ifdef CLIENT_DLL

void CBaseTower::Spawn( void )
{
	SetNextClientThink( CLIENT_THINK_ALWAYS );
	BaseClass::Spawn();
}

void CBaseTower::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if( m_iTowerType >= 0 && m_iTowerType < MAX_TOWERS )
	{
		m_szName = (char*)g_szTowerNames[m_iTowerType];
	}
}

const Vector& CBaseTower::GetRenderOrigin( void )
{
	if(m_bSelected && !m_bBuilt && ShouldCheck() )
		return GetFakeOrigin();
	return GetAbsOrigin();
}

void CBaseTower::ClientThink( void )
{
}

void CBaseTower::OnHover( CSDKPlayer *sdkPlayer )
{
}

void CBaseTower::Selected( CSDKPlayer *pPlayer )
{
	if(IsBuilt())
		return;

	m_vecRadiusCenter = GetFakeOrigin() + Vector(0,0, WorldAlignMins().z+1);
}

void CBaseTower::PreDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered )
{
	if(m_bMarkedForDeletion)
		return;

	if( bSelected )	
	{
		DrawCircle( m_vecRadiusCenter, m_bCanBuild );

		Vector vColor;
		if(GetOwnerEntity() != localPlayer)		vColor = Vector(1,1,0);
		else if(m_bCanBuild)					vColor = Vector(0,1,0);
		else									vColor = Vector(1,0,0);

		if(!m_bBuilt)
		{
			
			DrawSurroundingTowerPalettes( m_vecRadiusCenter );
		}

		float x = 1.0f;
 		float y = 1.0f;;
		float radius;

		const model_t *mod = modelinfo->GetModel( GetModelIndex() );
		Vector mins, maxs;
		if ( mod )
			modelinfo->GetModelBounds( mod, mins, maxs );

		x = maxs.x;
		y = maxs.y;
		radius = (x+y);

		if(m_bBuilt)
			DrawCircle( "overlay/selected", m_vecRadiusCenter, (int)radius, vColor, 1.0f );
	}
	else
	{
		if(bHovered)
		{
			const model_t *mod = modelinfo->GetModel( GetModelIndex() );
			Vector mins, maxs;
			if ( mod )
			{
				modelinfo->GetModelBounds( mod, mins, maxs );
			}

			float x = maxs.x;// - WorldAlignMins().x;
			float y = maxs.y;// - WorldAlignMins().y;
			float radius = (x+y);// * 0.5f;
			DrawCircle( "overlay/selected", m_vecRadiusCenter, (int)radius, Vector(1,1,0), 1.0f );
		}
	}
}

void CBaseTower::PostDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered )
{
	if(m_bMarkedForDeletion)
		return;

	if( !m_bBuilt && bSelected )
	{
		Vector vColor;
		if(GetOwnerEntity() != localPlayer)		vColor = Vector(1,1,0);
		else if(m_bCanBuild)					vColor = Vector(0,1,0);
		else									vColor = Vector(1,0,0);
		DrawCircle( "overlay/palette", m_vecRadiusCenter, (int)WorldAlignMaxs().x, vColor, 0.4f );;
	}
	if( m_bDrawPalette )
	{
		DrawCircle( "overlay/palette", m_vecRadiusCenter, (int)WorldAlignMaxs().x, Vector(1,1,0), 0.5f );
	}
}


void CBaseTower::DrawSurroundingTowerPalettes( const Vector &origin )
{
	CBaseEntity *pEnt = NULL;
	
	pEnt = FindByGroup( pEnt, COLLISION_GROUP_TOWER );

	static float minDist = 600.0f * 600.0f;
	float dist = 0.0f;
	Vector oOrig;
	CBaseTower *pTemp = NULL;

	for ( ;pEnt; pEnt = FindByGroup( pEnt, COLLISION_GROUP_TOWER ) )
	{
		if(pEnt == this)
			continue;

		pTemp = ToBaseTower( pEnt );
		if(!pTemp || !pTemp->m_bBuilt)
			continue;

		oOrig = pTemp->GetAbsOrigin() - origin;
		dist = oOrig.x*oOrig.x + oOrig.y*oOrig.y;

		if(dist < minDist)
			pTemp->m_bDrawPalette = true;
		else
			pTemp->m_bDrawPalette = false;
			//DrawCircle( "overlay/palette", pTemp->m_vecRadiusCenter, (int)WorldAlignMaxs().x, Vector(1,1,0), 0.5f );
		//	pTemp->DrawBoxOverlay( Color(255,255,0,10) );
			//debugoverlay->AddBoxOverlay(pTemp->GetAbsOrigin(), pTemp->WorldAlignMins(), pTemp->WorldAlignMaxs(), QAngle(0,0,1), 255, 255, 0, 10, -1); 
	}
}
void CBaseTower::DrawCircle( const Vector &origin, bool buildable )
{
	//const char *szMaterial;
	Vector vColor;
	if(buildable)
		vColor = Vector(0,1,0);//szMaterial = "overlay/buildable";
	else
		vColor = Vector(1,0,0);//szMaterial = "overlay/unbuildable";

	DrawCircle( "overlay/radius", origin, m_iRadius, vColor );
}

void CBaseTower::DrawCircle( const char *szMaterial, const Vector &origin, int radius, Vector vColor, float alpha )
{
	IMaterial *m_pMaterial = NULL;;
	m_pMaterial = materials->FindMaterial( szMaterial, TEXTURE_GROUP_CLIENT_EFFECTS );

	IMesh *pMesh = materials->GetDynamicMesh( true, 0, 0, m_pMaterial );

	CMeshBuilder builder;
	builder.Begin( pMesh, MATERIAL_QUADS, 1 );
	
	//Vector vColor( 1, 0, 0 );
	Vector vPt, vBasePt, vRight, vUp;
	vBasePt = origin;
	vRight = Vector(radius,0,0);
	vUp = Vector(0,radius, 0 );

	vPt = vBasePt - vRight + vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 0, 1 );
	builder.AdvanceVertex();
	
	vPt = vBasePt + vRight + vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 1, 1 );
	builder.AdvanceVertex();
	
	vPt = vBasePt + vRight - vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 1, 0 );
	builder.AdvanceVertex();
	
	vPt = vBasePt - vRight - vUp;
	builder.Position3fv( vPt.Base() );
	builder.Color4f( VectorExpand(vColor), alpha );
	builder.TexCoord2f( 0, 0, 0 );
	builder.AdvanceVertex();
	
	builder.End( false, true );
}

CBaseEntity *CBaseTower::FindByGroup( CBaseEntity *pStart, int collisionGroup )
{
	pStart = pStart ? cl_entitylist->NextBaseEntity( pStart ) : cl_entitylist->FirstBaseEntity();
	for ( ;pStart; pStart = cl_entitylist->NextBaseEntity( pStart ) )
		if(pStart->GetCollisionGroup() == collisionGroup )
				return pStart;
	return NULL;
}

void CBaseTower::ClearPalettes()
{
	CBaseEntity *pEnt = NULL;
	
	pEnt = FindByGroup( pEnt, COLLISION_GROUP_TOWER );

	CBaseTower *pTemp = NULL;

	for ( ;pEnt; pEnt = FindByGroup( pEnt, COLLISION_GROUP_TOWER ) )
	{
		if(pEnt == this)
			continue;

		pTemp = ToBaseTower( pEnt );
		if(!pTemp)
			continue;

		pTemp->m_bDrawPalette = false;
	}
}
#else

void CBaseTower::AddLevel( const char *szModel, int cost, int radius, int damage, int speed, float attackrate, const QAngle &angle)
{
	LevelInfo_t newlevel;
	newlevel.szModel = szModel;
	newlevel.upgradecost = cost;
	newlevel.radius = radius;
	newlevel.damage = damage;
	newlevel.speed = speed;
	newlevel.attackdelay = attackrate;
	newlevel.angle = angle;
	m_sLevelInfo.AddToTail( newlevel );
}

void CBaseTower::Spawn( void )
{
	m_iMaxLevel = m_sLevelInfo.Count()-1;

	UpdateTower();

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_NONE );

	SetCollisionGroup( COLLISION_GROUP_TOWER );

	m_bSelected = true;
	m_bBuilt = false;

	m_szName = (char*)g_szTowerNames[m_iTowerType];
	m_flNextAttackTime = gpGlobals->curtime;

	BaseClass::Spawn();
}

bool CBaseTower::Upgrade( void )
{
	if(m_iLevel >= m_iMaxLevel )
		return false;

	CSDKPlayer *sdkPlayer = ToSDKPlayer( GetOwnerEntity() );
	if(sdkPlayer)
	{
		//pPlayer->SetMoney( 5000 ); //HACKS!!!
		int money = sdkPlayer->GetMoney();
		if(money >= m_iUpgradeCost)
		{
			sdkPlayer->SetMoney( money - m_iUpgradeCost );
			m_iLevel++;

			UpdateTower(); //Update Tower info.
			return true;
		}
		return false;
	}
	return false;
}

int CBaseTower::Sell( void )
{
	SetTouch( NULL );
	SetThink( NULL );
	UTIL_Remove( this );

	int amount = (int)( (float)Cost() * tower_sellpercent.GetFloat() );

	CSDKPlayer *sdkPlayer = ToSDKPlayer( GetOwnerEntity() );
	if(sdkPlayer)
	{
		sdkPlayer->SetMoney( sdkPlayer->GetMoney() + amount );
		return amount;
	}

	return 0;
}

int CBaseTower::Cost( void )
{
	int cost = 0;
	for( int i = 1; i <= m_iLevel; i++) cost += m_sLevelInfo[i].upgradecost;

	return cost;
}

void CBaseTower::UpdateTower( void )
{
	if(m_iLevel < 0 || m_iLevel > m_iMaxLevel )
		return;

	const char *szModel = m_sLevelInfo[m_iLevel].szModel;
	if(m_iLevel <= m_iMaxLevel)
	{
		PrecacheModel( szModel );
		SetModel( szModel );
	}

	int index = modelinfo->GetModelIndex( szModel );
	const model_t *mod = modelinfo->GetModel( index );
	Vector mins, maxs;
	if ( mod )
	{
		modelinfo->GetModelBounds( mod, mins, maxs );
	}

	mins.x = -120;
	mins.y = -120;
	maxs.x = 120;
	maxs.y = 120;

	UTIL_SetSize( this, mins, maxs );

	trace_t trace;
	CTraceFilterWORLDSPECIAL filter( this );
	Vector origin = GetAbsOrigin() + Vector( 0, 0, ((maxs.z - mins.z) + 20.0f) );
	Vector end = origin + (Vector(0,0,-1) * MAX_TRACE_LENGTH);
	TraceBBox( origin, end, &filter, trace );
	SetAbsOrigin( trace.endpos );

	m_iDamage = m_sLevelInfo[m_iLevel].damage;
	m_iSpeed = m_sLevelInfo[m_iLevel].speed;
	m_iRadius = m_sLevelInfo[m_iLevel].radius;
	if(m_iLevel >= m_iMaxLevel)
		m_iUpgradeCost = m_sLevelInfo[m_iLevel].upgradecost;
	else if(m_iLevel != 1)
		m_iUpgradeCost = m_sLevelInfo[m_iLevel+1].upgradecost;
	else
		m_iUpgradeCost = m_sLevelInfo[m_iLevel].upgradecost;

	m_flAttackDelay = m_sLevelInfo[m_iLevel].attackdelay;
	SetAbsAngles( m_sLevelInfo[m_iLevel].angle );
}

void CBaseTower::SeekEnemy( void )
{
	if(!m_bBuilt)
		return;

	float gametime = gpGlobals->curtime;

	if( m_flNextAttackTime <= gametime )
	{
		const float minDist = m_iRadius * m_iRadius;
		float dist = 0.0f;
		CBaseEntity *pEnt = NULL;
		Vector myOrig = GetAbsOrigin();
		Vector enemyOrig;
		Vector vDir;
		while ( ( pEnt = gEntList.FindEntityByClassname( pEnt, "towers_enemy" ) ) != NULL )
		{
			enemyOrig = pEnt->GetAbsOrigin();
			vDir = enemyOrig - myOrig;
			dist = (vDir.x*vDir.x + vDir.y*vDir.y);
			if(dist <= minDist)
			{
				FireProjectile( pEnt );
				m_flNextAttackTime = gametime + m_flAttackDelay;
				break;
			}
		}
	}
}

/*
// You MUST override this in your derived tower class and fire a projectile. Here is the basic code.
void CBaseTower::FireProjectile( CBaseEntity *pEnemy )
{
	CBaseProjectile *pProj = CBaseProjectile::CreateProjectile( GetOwnerEntity(), pEnemy, (CBaseProjectile*)CreateEntityByName( "Your Projectile Class" );, (float)m_iDamage, (float)m_iSpeed );
	pProj->SetAbsOrigin( GetAbsOrigin() + Vector(0,0,WorldAlignMaxs().z) );
}
*/

#endif

void CBaseTower::BuildingPlacement( CSDKPlayer *pPlayer )
{
	Vector origin = pPlayer->GetAbsOrigin();
	Vector forward;
	Vector end;
	trace_t trace;

	forward = pPlayer->GetMouseDirection();//m_vMouseDirection;
	end = origin + (forward * MAX_TRACE_LENGTH);
	
	//Trace a line to where we're pointing.
	CTraceFilterWORLDSPECIAL filter( this );
	UTIL_TraceLine( origin, end, MASK_SOLID_BRUSHONLY, &filter, &trace );

	//If we aim at a wall, use a bbox trace instead (so we don't clip)
	if(trace.surface.flags & SURF_SKY)// || trace.plane.normal.z < 0.7)
		TraceBBox( origin, end, &filter, trace );;

	if(trace.fraction > 0 && trace.fraction != 1 && trace.plane.normal.z > 0.7)
	{
		//Bring straight down to ground and figure out if we can build on this ground.
		CTraceFilterCHECKBUILD filter2( this );
		float zOffset = (WorldAlignMaxs().z - WorldAlignMins().z);
		origin = trace.endpos + Vector(0,0,zOffset+35);
		end = origin + (Vector(0,0,-1) * MAX_TRACE_LENGTH); 

		TraceBBox( origin, end, &filter2, trace );
		
		if((trace.fraction == 0 || trace.fraction == 1.0f) || trace.plane.normal.z < 0.7)
			return;

		//Set origin to floor.
#ifdef CLIENT_DLL
		if(!m_bFakeOrigin)
			m_bFakeOrigin = true;
		SetFakeOrigin( trace.endpos );
#else
		SetAbsOrigin( trace.endpos );
#endif

		bool canBuild = filter2.m_bCanBuild;
		m_bCanBuild = canBuild;


		if((pPlayer->m_afButtonReleased & IN_ATTACK) && canBuild)
		{
			int money = pPlayer->GetMoney();
			if(money < GetUpgradeCost())
			{
#ifdef GAME_DLL
				CRecipientFilter filter;
				filter.AddRecipient( pPlayer );
				filter.MakeReliable();
				char szText[200];
				Q_snprintf( szText, sizeof(szText), "$%i required to build %s.", m_iUpgradeCost, m_szName );
				UTIL_ClientPrintFilter( filter, HUD_PRINTTALK, szText );
#endif
				return;
			}
			pPlayer->SetMoney( money - GetUpgradeCost() );

			OnBuilt( pPlayer );
			pPlayer->m_hSelected = (CBaseClickable*)NULL;
			
#ifdef CLIENT_DLL
			m_bFakeOrigin = false;
			ClearPalettes();
#endif
			return;
		}
		else if((pPlayer->m_afButtonReleased & IN_ATTACK2))
		{	
			pPlayer->m_hSelected = (CBaseClickable*)NULL;

			m_bMarkedForDeletion = true;
#ifdef GAME_DLL
			SetThink( &CBaseEntity::SUB_Remove );
			SetNextThink( gpGlobals->curtime+0.01f );
#else
			ClearPalettes();
#endif
			pPlayer->RemoveCurrentSelection();
		}
	}
	return;
}

void CBaseTower::OnBuilt( CSDKPlayer *pPlayer ) 
{
	pPlayer->m_pTowers.AddToTail( this );

#ifdef GAME_DLL
	if(m_iLevel < m_iMaxLevel)
		m_iUpgradeCost = m_sLevelInfo[m_iLevel+1].upgradecost;
#endif

	m_bBuilt = true;


};

void CBaseTower::TraceBBox( const Vector &start, const Vector &end, ITraceFilter *pFilter, trace_t &pm )
{
	UTIL_TraceEntity( this, start, end, MASK_SOLID_BRUSHONLY, pFilter, &pm );
}