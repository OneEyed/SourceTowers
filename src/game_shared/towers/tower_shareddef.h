#ifndef TOWER_SHAREDDEF
#define TOWER_SHAREDDEF


#define MAX_PROJECTILES 3
#define MAX_TOWERS 4

//Names to Display
static const char *g_szTowerNames[ MAX_TOWERS ] =
{
	"Earth Tower",
	"Fire Tower",
	"Ice Tower",
	"Super Tower"
};

//Tower CLASSNAMES
static const char *g_TowerTypes[ MAX_TOWERS ] = 
{
	"earth_tower",
	"fire_tower",
	"ice_tower",
	"super_tower"

};

//Projectile CLASSNAMES
static const char* g_szProjectile[MAX_PROJECTILES] =
{
	"proj_earth",
	"proj_fire",
	"proj_ice"
};

#define MAX_EARTH_MODELS 5
static const char *g_EarthModels[MAX_EARTH_MODELS] = 
{
	"models/props_canal/rock_riverbed02c.mdl",
	"models/props_debris/barricade_tall04a.mdl",
	"models/props_wasteland/rockcliff01e.mdl",
	"models/lostcoast/props_wasteland/rock_coast02g.mdl",
	"models/props_wasteland/antlionhill.mdl"
};

#define MAX_FIRE_MODELS 5
static const char *g_FireModels[MAX_FIRE_MODELS] = 
{
	"models/props_wasteland/buoy01.mdl",
	"models/lostcoast/props_junk/float01a.mdl",
	"models/props_wasteland/coolingtank01.mdl",
	"models/props_trainstation/train002.mdl",
	"models/cranes/crane_frame.mdl"
};

#define MAX_ICE_MODELS 5
static const char *g_IceModels[MAX_ICE_MODELS] = 
{
	"models/props_c17/pillarcluster_001a.mdl",
	"models/props_wasteland/horizontalcoolingtank04.mdl",
	"models/props_wasteland/coolingtank02.mdl",
	"models/props_buildings/watertower_001c.mdl",
	"models/props_buildings/watertower_001a.mdl"
};

#define MAX_SUPER_MODELS 3
static const char *g_SuperModels[MAX_SUPER_MODELS] = 
{
	"models/props_combine/breentp_rings.mdl",
	"models/props_combine/stasisvortex.mdl",
	"models/props_combine/stasisshield.mdl"
};
enum TowerType_t
{
	SELL_TOWER = -2,
	UPGRADE_TOWER = -1,
	EARTH_TOWER = 0,
	FIRE_TOWER,
	ICE_TOWER,
	SUPER_TOWER
};

struct LevelInfo_t
{
	const char *szModel;
	int upgradecost;
	int radius;
	int damage;
	int speed;
	float attackdelay;
	QAngle angle;
};

#endif