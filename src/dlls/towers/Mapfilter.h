#ifndef CMAPENTITYFILTER_H
#define CMAPENTITYFILTER_H

// general definitions and includes
#include "cbase.h"
#include "mapentities.h"
#include "utllinkedlist.h"

// These entities are preserved each round restart. The rest are removed and recreated.
static const char *s_PreserveEnts[] =
{
  "ai_network",
  "ai_hint",
  "ambient_generic",
  "sdk_gamerules",
  "sdk_team_manager",
  "player_manager",
  "env_soundscape",
  "env_soundscape_proxy",
  "env_soundscape_triggerable",
  "env_sun",
  "env_wind",
  "env_fog_controller",
  "func_brush",
  "func_wall",
  "func_illusionary",
  "func_rotating",
  "infodecal",
  "info_projecteddecal",
  "info_node",
  "info_target",
  "info_node_hint",
  "info_spectator",
  "info_map_parameters",
  "keyframe_rope",
  "move_rope",
  "info_ladder",
  "player",
  "point_viewcontrol",
  "scene_manager",
  "shadow_control",
  "sky_camera",
  "soundent",
  "trigger_soundscape",
  "viewmodel",
  "predicted_viewmodel",
  "worldspawn",
  "point_devshot_camera",
  "towers_spawn",
  "towers_move",
  "",
  NULL,// END Marker
};

class CMapEntityRef
{
public:
  int m_iEdict;
  int m_iSerialNumber;
};

extern CUtlLinkedList<CMapEntityRef, unsigned short> g_MapEntityRefs;

class CMapLoadEntityFilter : public IMapEntityFilter
{
public:

virtual bool ShouldCreateEntity( const char *pClassname )
{
  // During map load, create all the entities.
  return true;
}

virtual CBaseEntity* CreateNextEntity( const char *pClassname )
{
     // create each entity in turn and an instance of CMapEntityRef
     CBaseEntity *pRet = CreateEntityByName( pClassname );

     CMapEntityRef ref;
     ref.m_iEdict = -1;
     ref.m_iSerialNumber = -1;

     // if the new entity is valid store the entity information in ref
     if ( pRet )
     {
        ref.m_iEdict = pRet->entindex();

        if ( pRet->edict() )
           ref.m_iSerialNumber = pRet->edict()->m_NetworkSerialNumber;
     }

     // add the new ref to the linked list and return the entity
     g_MapEntityRefs.AddToTail( ref );
     return pRet;
  }
};

#endif