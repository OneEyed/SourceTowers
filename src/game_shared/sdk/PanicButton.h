#ifndef PANIC_BUTTON_H
#define PANIC_BUTTON_H

#include "BaseClickable.h"

#ifdef CLIENT_DLL
#define CPanicButton C_PanicButton
#endif

class CPanicButton : public CBaseClickable
{
	DECLARE_CLASS( CPanicButton, CBaseClickable );
	DECLARE_NETWORKCLASS();

	int GetClickType() { return CLICK_UNIT; };

public:
	virtual void Selected( CSDKPlayer *sdkPlayer );

#ifdef CLIENT_DLL

	virtual void OnHover( CSDKPlayer *sdkPlayer );
	
	//void Spawn();
	void ClientThink();
	virtual void OnDataChanged( DataUpdateType_t updateType );

	void DoAnimation( int seq, float spd );

	void DoAnimation( const char *seqName, float spd );

	//void ConvergeNumber( float &cur, float goal, float speed );
	bool m_bRanSelected;
	bool m_bReverse;

#else

	DECLARE_DATADESC();

	CPanicButton() {};
	~CPanicButton() {};

	void Spawn( void );
	void Precache( void );

	void Think( void );

	//virtual void Selected( CSDKPlayer *sdkPlayer );

#endif
};

#endif //PANIC_BUTTON_H