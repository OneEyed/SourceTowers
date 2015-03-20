#ifndef BASE_CLICKABLE_H
#define BASE_CLICKABLE_H

#ifdef CLIENT_DLL
	#define CBaseClickable C_BaseClickable
	#define CSDKPlayer C_SDKPlayer
#endif

#define FL_IS_SELECTABLE		(1<<10)
#define MAX_SELECTABLE_UNITS	12
class CSDKPlayer;

enum ClickType_t
{
	CLICK_UNKNOWN = 0,
	CLICK_BUILDING,
	CLICK_UNIT
};

class CBaseClickable : public CBaseAnimating
{
public:

	DECLARE_CLASS( CBaseClickable, CBaseAnimating );
	DECLARE_NETWORKCLASS();

	CBaseClickable();
	virtual ~CBaseClickable();

	virtual void Spawn( void );

#ifdef CLIENT_DLL
	void DrawBoxOverlay( Color color );

	virtual void PreDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered ) {};
	virtual void PostDraw( C_SDKPlayer *localPlayer, bool bSelected, bool bHovered ) {};

	//Predict our origin so we don't get lag!
	virtual const Vector& GetRenderOrigin( void );
	void SetFakeOrigin( const Vector &orig ) { m_vecFakeOrigin = orig; };
	const Vector& GetFakeOrigin() { return m_vecFakeOrigin; };

	Vector m_vecFakeOrigin;
	bool m_bFakeOrigin;
#else
	
#endif
	

	virtual void OnHoverEnter( CSDKPlayer *sdkPlayer );
	virtual void OnHoverExit( CSDKPlayer *sdkPlayer );
	virtual void OnHover( CSDKPlayer *sdkPlayer );

	virtual void OnSelected( CSDKPlayer *sdkPlayer );
	virtual void Selected( CSDKPlayer *sdkPlayer ); //Runs at all times when selected.

	virtual int GetClickType() { return (int)CLICK_UNKNOWN; };
	int m_iClickID;
	inline bool IsIndex( int index );
	bool IsHovered( CSDKPlayer *sdkPlayer );
	bool IsSelected( CSDKPlayer *sdkPlayer );

	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iHealth );
	CNetworkVar( int, m_iMaxHealth );
	//IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iMaxHealth );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_fFlags );

	virtual void SetHealth( int amt ) { m_iHealth = amt; };
	virtual void SetMaxHealth( int amt ) { m_iMaxHealth = amt; };
	virtual int GetHealth() { return m_iHealth; };
	virtual int GetMaxHealth() { return m_iMaxHealth; };

	
	//CNetworkVar( bool, m_bHovered );
	//CNetworkVar( bool, m_bSelected );
	bool m_bMarkedForDeletion;
};

extern CUtlVector< CBaseClickable* > g_pClickables;

static inline bool IsValidClickable( int index )
{
	return (index >= 0 && index < g_pClickables.Count() );
}

inline CBaseClickable *ToClickable( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CBaseClickable*>( pEntity ) != 0 );
#endif
	return static_cast< CBaseClickable* >( pEntity );
}

#endif //BASE_CLICKABLE_H