#ifndef SELECTION_FILTER_H
#define SELECTION_FILTER_H

#include "BaseClickable.h"

#ifdef CLIENT_DLL
	#define CSelectionFilter C_SelectionFilter
#endif

enum SelectType_t
{
	SELECT_EVERYTHING = 0,
	SELECT_BUILDING_ONLY,	
	SELECT_UNIT_ONLY
};

class CBaseClickable;

abstract_class ISelectFilter
{
public:
	virtual bool CanTestEntity( CBaseClickable *pEnt ) = 0;
	virtual bool ShouldSelectEntity( CBaseClickable *pEnt ) = 0;
	virtual SelectType_t GetSelectType() const = 0;
};

//Allow only buildings to be selected.
class CSelectFilter : public ISelectFilter
{
public:
	virtual bool CanTestEntity( CBaseClickable *pEnt ) { return true; };
	virtual bool ShouldSelectEntity( CBaseClickable *pEnt )
	{
		return true;
	};
	virtual SelectType_t GetSelectType() const 
	{
		return SELECT_EVERYTHING;	
	};
};	

//Allow only buildings to be selected.
class CSelectFilterBuilding : public CSelectFilter
{
public:
	virtual bool ShouldSelectEntity( CBaseClickable *pEnt )
	{
		if(pEnt && pEnt->GetClickType() == CLICK_BUILDING)
			return true;
		return false;
	};
	virtual SelectType_t GetSelectType() const 
	{
		return SELECT_BUILDING_ONLY;	
	};
};	

//Allow only characters to be selected.
class CSelectFilterUnit : public CSelectFilter
{
public:
	virtual bool ShouldSelectEntity( CBaseClickable *pEnt )
	{
		if(pEnt && pEnt->GetClickType() == CLICK_UNIT)
			return true;
		return false;
	};
	virtual SelectType_t GetSelectType() const 
	{
		return SELECT_UNIT_ONLY;	
	};
};	

#endif //SELECTION_FILTER_H