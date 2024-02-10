#ifndef _DECLARATIONS_HPP_
#define _DECLARATIONS_HPP_

#include <setjmp.h>

#define QDECL __attribute__((cdecl))

#define qboolean int
#define qtrue 1
#define qfalse 0

// 2D vectors
#define DotProduct2( a, b )         ( ( a )[0] * ( b )[0] + ( a )[1] * ( b )[1] )
#define VectorScale2( v, s, o )     ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ) )
#define VectorSubtract2( a, b, c )  ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1] )

// 3D vectors
#define DotProduct( a, b )          ( ( a )[0] * ( b )[0] + ( a )[1] * ( b )[1] + ( a )[2] * ( b )[2] )
#define VectorAdd( a, b, c )        ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2] )
#define VectorClear( a )            ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )
#define VectorCopy( a, b )          ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2] )
#define VectorCross( a, b, c )      ( ( c )[0] = ( a )[1] * ( b )[2] - ( a )[2] * ( b )[1],( c )[1] = ( a )[2] * ( b )[0] - ( a )[0] * ( b )[2],( c )[2] = ( a )[0] * ( b )[1] - ( a )[1] * ( b )[0] )
#define VectorMA( v, s, b, o )      ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2]=( v )[2] + ( b )[2] * ( s ) )
#define VectorNegate( a, b )        ( ( b )[0] = -( a )[0],( b )[1] = -( a )[1],( b )[2] = -( a )[2] )
#define VectorScale( v, s, o )      ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ) )
#define VectorSet( v, x, y, z )     ( ( v )[0] = ( x ),( v )[1] = ( y ),( v )[2] = ( z ) )
#define VectorSnap( v )             { v[0] = (int)v[0]; v[1] = (int)v[1]; v[2] = (int)v[2]; }
#define VectorSubtract( a, b, c )   ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2] )

// 4D vectors
#define DotProduct4( x, y )         ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] + ( x )[3] * ( y )[3] )
#define VectorAdd4( a, b, c )       ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2],( c )[3] = ( a )[3] + ( b )[3] )
#define VectorCopy4( a, b )         ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )
#define VectorMA4( v, s, b, o )     ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ),( o )[3] = ( v )[3] + ( b )[3] * ( s ) )
#define VectorScale4( v, s, o )     ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ),( o )[3] = ( v )[3] * ( s ) )
#define VectorSubtract4( a, b, c )  ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2],( c )[3] = ( a )[3] - ( b )[3] )

#define ARCHIVEDSSBUF_SIZE          0x2000000
#define BIG_INFO_STRING             0x2000
#define ENTFIELD_MASK               0xC000
#define FLOAT_INT_BITS              13
#define FLOAT_INT_BIAS              ( 1 << ( FLOAT_INT_BITS - 1 ) ) // 0x1000
#define FRAMETIME                   50
#define GENTITYNUM_BITS             10
#define PACKET_BACKUP               32
#define PACKET_MASK                 ( PACKET_BACKUP - 1 )
#define SV_OUTPUTBUF_LENGTH         ( 2048 * MAX_CLIENTS - 16 )

#define MAX_ANIMSCRIPT_ANIMCOMMANDS 8
#define MAX_ANIMSCRIPT_ITEMS_PER_MODEL  2048
#define MAX_ANIMSCRIPT_ITEMS        128
#define MAX_BPS_WINDOW              20
#define MAX_CHALLENGES              1024
#define MAX_CLIENTS                 64
#define MAX_CONFIGSTRINGS           2048
#define MAX_DOWNLOAD_BLKSIZE        1024
#define MAX_DOWNLOAD_WINDOW         8
#define MAX_ENT_CLUSTERS            16
#define MAX_EVENTS                  4
#define MAX_GENTITIES               ( 1 << GENTITYNUM_BITS )
#define MAX_INFO_STRING             0x400
#define MAX_ITEM_MODELS             2
#define MAX_MODEL_ANIMATIONS        512 // animations per model
#define MAX_MODELS                  256
#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2
#define MAX_MSGLEN                  0x4000
#elif COD_VERSION == COD2_1_3
#define MAX_MSGLEN                  0x20000
#endif
#define MAX_NETNAME                 16
#define MAX_OSPATH                  256
#define MAX_QPATH                   64
#define MAX_RELIABLE_COMMANDS		128
#define MAX_SNAPSHOT_ENTITIES       1024
#define MAX_STRINGLENGTH            1024
#define MAX_TOTAL_ENT_LEAFS         128
#define MAX_VASTRINGS               2
#define MAX_VOICEFRAMESIZE          160
#define MAX_VOICEPACKETDATALEN      256
#define MAX_VOICEPACKETS            40
#define MAX_VOICEPACKETSPERFRAME    2.56
#define MAX_ZPATH                   256

#define DVAR_NOFLAG             0               // 0x0000
#define DVAR_ARCHIVE            (1 << 0)        // 0x0001
#define DVAR_USERINFO           (1 << 1)        // 0x0002
#define DVAR_SERVERINFO         (1 << 2)        // 0x0004
#define DVAR_SYSTEMINFO         (1 << 3)        // 0x0008
#define DVAR_INIT               (1 << 4)        // 0x0010
#define DVAR_LATCH              (1 << 5)        // 0x0020
#define DVAR_ROM                (1 << 6)        // 0x0040
#define DVAR_CHEAT              (1 << 7)        // 0x0080
#define DVAR_CODINFO            (1 << 8)        // 0x0100
#define DVAR_SAVED              (1 << 9)        // 0x0200
#define DVAR_NORESTART          (1 << 10)       // 0x0400
#define DVAR_CHANGEABLE_RESET   (1 << 12)       // 0x1000
#define DVAR_EXTERNAL           (1 << 14)       // 0x4000
#define DVAR_AUTOEXEC           (1 << 15)       // 0x8000

#define HASH_STAT_HEAD    0x8000
#define HASH_NEXT_MASK    0x3FFF
#define HASH_STAT_MASK    0xC000

#define ENTITY_WORLD    0x3FE
#define ENTITY_NONE     0x3FF

// gentity_s->flags
#define FL_GODMODE              0x1
#define FL_DEMI_GODMODE         0x2
#define FL_NOTARGET             0x4
#define FL_NO_KNOCKBACK         0x8
#define FL_DROPPED_ITEM         0x10
#define FL_TURRET_UNKNOWN1      0x100
#define FL_TURRET_UNKNOWN2      0x200
#define FL_TURRET_UNKNOWN3      0x400
#define FL_INVISIBLE            0x800
#define FL_LINKTO_ENABLED       0x1000
#define FL_GRENADE_TOUCH_DAMAGE 0x4000
#define FL_MISSILE_UNKNOWN      0x10000
#define FL_STABLE_MISSILE       0x20000

#define SVF_NOCLIENT    0x1
#define SVF_BROADCAST   0x8

#define KEY_MASK_NONE       0
#define KEY_MASK_FORWARD    127
#define KEY_MASK_BACK       -127
#define KEY_MASK_MOVERIGHT  127
#define KEY_MASK_MOVELEFT   -127

#define KEY_MASK_FIRE           0x1
#define KEY_MASK_MELEE          0x4
#define KEY_MASK_USE            0x8
#define KEY_MASK_RELOAD         0x10
#define KEY_MASK_LEANLEFT       0x40
#define KEY_MASK_LEANRIGHT      0x80
#define KEY_MASK_PRONE          0x100
#define KEY_MASK_CROUCH         0x200
#define KEY_MASK_JUMP           0x400
#define KEY_MASK_ADS_MODE       0x1000
#define KEY_MASK_HOLDBREATH     0x8000
#define KEY_MASK_MELEE_BREATH   0x8004
#define KEY_MASK_FRAG           0x10000
#define KEY_MASK_SMOKE          0x20000

// playerState_t->eFlags
// entityState_t->eFlags
#define EF_CROUCHING    0x4
#define EF_PRONE        0x8
#define EF_FIRING       0x20
#define EF_USETURRET    0x300
#define EF_MANTLE       0x4000
#define EF_TAGCONNECT   0x8000
#define EF_DEAD         0x20000
#define EF_AIMDOWNSIGHT 0x40000
#define EF_VOTED        0x100000
#define EF_TALK         0x200000
#define EF_TAUNT        0x400000
#define EF_BOUNCE       0x1000000

#define PMF_PRONE           0x1
#define PMF_CROUCH          0x2 // PMF_DUCKED
#define PMF_MANTLE          0x4
#define PMF_FRAG            0x10
#define PMF_LADDER          0x20
#define PMF_BACKWARDS_RUN   0x80
#define PMF_SLIDING         0x200
#define PMF_MELEE           0x2000
#define PMF_JUMPING         0x80000
#define PMF_VIEWLOCKED      0x800000 // name guessed
#define PMF_SPECTATING      0x1000000
#define PMF_DISABLEWEAPON   0x4000000

#define CONTENTS_SOLID              0x1
#define CONTENTS_FOLIAGE            0x2
#define CONTENTS_NONCOLLIDING       0x4
#define CONTENTS_LAVA               0x8
#define CONTENTS_GLASS              0x10
#define CONTENTS_WATER              0x20
#define CONTENTS_CANSHOTCLIP        0x40
#define CONTENTS_MISSILECLIP        0x80
#define CONTENTS_ITEM				0x100
#define CONTENTS_VEHICLECLIP        0x200
#define CONTENTS_ITEMCLIP           0x400
#define CONTENTS_SKY                0x800
#define CONTENTS_AI_NOSIGHT         0x1000
#define CONTENTS_CLIPSHOT           0x2000
#define CONTENTS_MOVER              0x4000
#define CONTENTS_AREAPORTAL         0x8000
#define CONTENTS_PLAYERCLIP         0x10000
#define CONTENTS_MONSTERCLIP        0x20000
#define CONTENTS_TELEPORTER         0x40000
#define CONTENTS_JUMPPAD            0x80000
#define CONTENTS_CLUSTERPORTAL      0x100000
#define CONTENTS_DONOTENTER         0x200000
#define CONTENTS_DONOTENTER_LARGE   0x400000
#define CONTENTS_CURRENT_DOWN       0x800000 // might be named differently
#define CONTENTS_MANTLE             0x1000000
#define CONTENTS_BODY               0x2000000
#define CONTENTS_CORPSE             0x4000000
#define CONTENTS_DETAIL             0x8000000
#define CONTENTS_STRUCTURAL         0x10000000
#define CONTENTS_TRANSPARENT        0x20000000
#define CONTENTS_TRIGGER            0x40000000
#define CONTENTS_NODROP             0x80000000

#define MASK_ALL        ( -1 )
#define MASK_SHOT       ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_WATER | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_CURRENT_DOWN | CONTENTS_BODY ) // 0x2802821
#define MASK_OPAQUE_AI  ( CONTENTS_SOLID | CONTENTS_FOLIAGE | CONTENTS_SKY | CONTENTS_AI_NOSIGHT | CONTENTS_CURRENT_DOWN | CONTENTS_BODY ) // 0x2801803
#define MASK_SOLID      ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MISSILECLIP | CONTENTS_ITEMCLIP ) // 0x491

#define SURF_NOLIGHTMAP         0x0
#define SURF_NODAMAGE           0x1
#define SURF_SLICK              0x2
#define SURF_SKY                0x4
#define SURF_LADDER             0x8
#define SURF_NOIMPACT           0x10
#define SURF_NOMARKS            0x20
#define SURF_NODRAW             0x80
#define SURF_NOSTEPS            0x2000
#define SURF_NONSOLID           0x4000
#define SURF_NODLIGHT           0x20000
#define SURF_NOCASTSHADOW       0x40000
#define SURF_MANTLEON           0x2000000
#define SURF_MANTLEOVER         0x4000000
#define SURF_PORTAL             0x80000000

#define SURF_BARK               0x100000
#define SURF_BRICK              0x200000
#define SURF_CARPET             0x300000
#define SURF_CLOTH              0x400000
#define SURF_CONCRETE           0x500000
#define SURF_DIRT               0x600000
#define SURF_FLESH              0x700000
#define SURF_FOLIAGE            0x800000
#define SURF_GLASS              0x900000
#define SURF_GRASS              0xa00000
#define SURF_GRAVEL             0xb00000
#define SURF_ICE                0xc00000
#define SURF_METAL              0xd00000
#define SURF_MUD                0xe00000
#define SURF_PAPER              0xf00000
#define SURF_PLASTER            0x1000000
#define SURF_ROCK               0x1100000
#define SURF_SAND               0x1200000
#define SURF_SNOW               0x1300000
#define SURF_WATER              0x1400000
#define SURF_WOOD               0x1500000
#define SURF_ASPHALT            0x1600000

typedef unsigned char byte;
typedef signed char sbyte;
typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;
typedef int LargeLocal;
typedef gentity_t (*useList_t)[2050];
typedef int clipHandle_t;

typedef struct scr_entref_s
{
	uint16_t entnum;
	uint16_t classnum;
} scr_entref_t;

typedef struct game_client_field_s
{
	const char *name;
	int ofs;
	int type;
	void (*setter)(gclient_s *, const game_client_field_s *);
	void (*getter)(gclient_s *, const game_client_field_s *);
} game_client_field_t;

typedef enum
{
	CRITSECT_CONSOLE = 0,
	CRITSECT_UNKNOWN1 = 1,
	CRITSECT_COM_ERROR = 2,
	CRITSECT_UNKNOWN3 = 3,
	CRITSECT_DVAR = 4,
	CRITSECT_RD_BUFFER = 5,
	CRITSECT_COUNT = 6
} criticalSection_t;

typedef enum
{
	CON_CHANNEL_DONT_FILTER = 0x0,
	CON_CHANNEL_ERROR = 0x1,
	CON_CHANNEL_GAMENOTIFY = 0x2,
	CON_CHANNEL_BOLDGAME = 0x3,
	CON_CHANNEL_LOGFILEONLY = 0x4,
} conChannel_t;

typedef enum
{
	ERR_FATAL = 0x0,
	ERR_DROP = 0x1,
	ERR_SERVERDISCONNECT = 0x2,
	ERR_DISCONNECT = 0x3,
	ERR_SCRIPT = 0x4,
	ERR_SCRIPT_DROP = 0x5,
	ERR_LOCALIZATION = 0x6,
	ERR_MAPLOADERRORSUMMARY = 0x7,
} errorParm_t;

typedef enum
{
	HITLOC_NONE,
	HITLOC_HELMET,
	HITLOC_HEAD,
	HITLOC_NECK,
	HITLOC_TORSO_UPR,
	HITLOC_TORSO_LWR,
	HITLOC_R_ARM_UPR,
	HITLOC_L_ARM_UPR,
	HITLOC_R_ARM_LWR,
	HITLOC_L_ARM_LWR,
	HITLOC_R_HAND,
	HITLOC_L_HAND,
	HITLOC_R_LEG_UPR,
	HITLOC_L_LEG_UPR,
	HITLOC_R_LEG_LWR,
	HITLOC_L_LEG_LWR,
	HITLOC_R_FOOT,
	HITLOC_L_FOOT,
	HITLOC_GUN,
	HITLOC_NUM
} hitLocation_t;

typedef enum
{
	MOD_UNKNOWN,
	MOD_PISTOL_BULLET,
	MOD_RIFLE_BULLET,
	MOD_GRENADE,
	MOD_GRENADE_SPLASH,
	MOD_PROJECTILE,
	MOD_PROJECTILE_SPLASH,
	MOD_MELEE,
	MOD_HEAD_SHOT,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TRIGGER_HURT,
	MOD_EXPLOSIVE
} meansOfDeath_t;

typedef enum __attribute__((__packed__)) 
{
	ENT_HANDLER_NULL = 0x0,
	ENT_HANDLER_TRIGGER_MULTIPLE = 0x1,
	ENT_HANDLER_TRIGGER_HURT = 0x2,
	ENT_HANDLER_TRIGGER_HURT_TOUCH = 0x3,
	ENT_HANDLER_TRIGGER_DAMAGE = 0x4,
	ENT_HANDLER_SCRIPT_MOVER = 0x5,
	ENT_HANDLER_SCRIPT_MODEL = 0x6,
	ENT_HANDLER_GRENADE = 0x7,
	ENT_HANDLER_ROCKET = 0x8,
	ENT_HANDLER_CLIENT = 0x9,
	ENT_HANDLER_CLIENT_SPECTATOR = 0xA,
	ENT_HANDLER_CLIENT_DEAD = 0xB,
	ENT_HANDLER_PLAYER_CLONE = 0xC,
	ENT_HANDLER_TURRET_INIT = 0xD,
	ENT_HANDLER_TURRET = 0xE,
	ENT_HANDLER_DROPPED_ITEM = 0xF,
	ENT_HANDLER_ITEM_INIT = 0x10,
	ENT_HANDLER_ITEM = 0x11,
	ENT_HANDLER_TRIGGER_USE = 0x12,
	ENT_HANDLER_PLAYER_BLOCK = 0x13,
	ENT_HANDLER_COUNT = 0x14
} entHandlers_t;

typedef struct entityHandler_s
{
	void (*think)(gentity_t *);
	void (*reached)(gentity_t *);
	void (*blocked)(gentity_t *, gentity_t *);
	void (*touch)(gentity_t *, gentity_t *, int);
	void (*use)(gentity_t *, gentity_t *);
	void (*pain)(gentity_t *, gentity_t *, int, const float *, meansOfDeath_t, const float *, hitLocation_t);
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, meansOfDeath_t, const int, const float *, hitLocation_t, int);
	void (*controller)(gentity_t *, gentity_t *);
	int methodOfDeath;
	int splashMethodOfDeath;
} entityHandler_t;

enum svc_ops_e
{
	svc_nop,
	svc_gamestate,
	svc_configstring,
	svc_baseline,
	svc_serverCommand,
	svc_download,
	svc_snapshot,
	svc_EOF
};

enum clc_ops_e
{
	clc_move,
	clc_moveNoDelta,
	clc_clientCommand,
	clc_EOF
};

typedef enum
{
	ET_GENERAL = 0x0,
	ET_PLAYER = 0x1,
	ET_PLAYER_CORPSE = 0x2,
	ET_ITEM = 0x3,
	ET_MISSILE = 0x4,
	ET_INVISIBLE = 0x5,
	ET_SCRIPTMOVER = 0x6,
	ET_FX = 0x7,
	ET_LOOP_FX = 0x8,
	ET_TURRET = 0x9,
	ET_EVENTS = 0xA
} entityType_t;

typedef enum
{
	TEAM_FREE = 0x0,
	TEAM_NONE = 0x0,
	TEAM_BAD = 0x0,
	TEAM_AXIS = 0x1,
	TEAM_ALLIES = 0x2,
	TEAM_SPECTATOR = 0x3,
	TEAM_NUM_TEAMS = 0x4,
} team_t;

typedef enum
{
	CS_FREE,
	CS_ZOMBIE,
	CS_CONNECTED,
	CS_PRIMED,
	CS_ACTIVE
} clientConnectState_t;

typedef enum
{
	STATE_PLAYING,
	STATE_DEAD,
	STATE_SPECTATOR,
	STATE_INTERMISSION
} sessionState_t;

typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum
{
	NA_BOT = 0,
	NA_BAD = 0,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IPX = 5,
	NA_BROADCAST_IPX = 6
} netadrtype_t;

typedef struct
{
	netadrtype_t type;
	byte ip[4];
	unsigned short port;
	byte ipx[10];
} netadr_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	qboolean overflowed;
	byte *data;
	int maxsize;
	int cursize;
	int readcount;
	int bit;
} msg_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

enum DvarType : char
{
	DVAR_TYPE_BOOL = 0x00,
	DVAR_TYPE_FLOAT = 0x01,
	DVAR_TYPE_VEC2 = 0x02,
	DVAR_TYPE_VEC3 = 0x03,
	DVAR_TYPE_VEC4 = 0x04,
	DVAR_TYPE_INT = 0x05,
	DVAR_TYPE_ENUM = 0x06,
	DVAR_TYPE_STRING = 0x07,
	DVAR_TYPE_COLOR = 0x08
};

union DvarLimits
{
	struct
	{
		int stringCount;
		const char **strings;
	} enumeration;
	struct
	{
		int min;
		int max;
	} integer;
	struct
	{
		float min;
		float max;
	} decimal;
};

typedef struct
{
	union
	{
		bool boolean;
		int integer;
		float decimal;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		const char *string;
		unsigned char color[4];
	};
} DvarValue;

typedef struct dvar_s
{
	const char *name;
	unsigned short flags;
	DvarType type;
	bool modified;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarLimits domain;
	dvar_s *next;
	dvar_s *hashNext;
} dvar_t;

#pragma pack(push, 4)
struct __attribute__((aligned(4))) RefString
{
	union
	{
		struct
		{
			unsigned int refCount : 16;
			unsigned int user : 8;
			unsigned int byteLen : 8;
		};
		volatile int data;
	};
	char str[1];
};
#pragma pack(pop)

struct HashEntry
{
	unsigned short status_next;
	union
	{
		unsigned short prev;
		unsigned short str;
	};
};

struct scrStringGlob_t
{
	HashEntry hashTable[0x4000];
	unsigned char inited;
	byte pad[3];
	HashEntry *nextFreeEntry;
};

struct VariableStackBuffer
{
	const char *pos;
	uint16_t size;
	uint16_t bufLen;
	uint16_t localId;
	char time;
	char buf[1];
};

union VariableUnion
{
	int intValue;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	struct VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

union ObjectInfo_u
{
	uint16_t size;
	uint16_t entnum;
	uint16_t nextEntId;
	uint16_t self;
};

struct ObjectInfo
{
	uint16_t refCount;
	union ObjectInfo_u u;
};

union VariableValueInternal_u
{
	uint16_t next;
	union VariableUnion u;
	struct ObjectInfo o;
};

union VariableValueInternal_w
{
	unsigned int status;
	unsigned int type;
	unsigned int name;
	unsigned int classnum;
	unsigned int notifyName;
	unsigned int waitTime;
	unsigned int parentLocalId;
};

union VariableValueInternal_v
{
	uint16_t next;
	uint16_t index;
};

typedef struct
{
	union VariableUnion u;
	int type;
} VariableValue;

union Variable_u
{
	uint16_t prev;
	uint16_t prevSibling;
};

struct Variable
{
	uint16_t id;
	union Variable_u u;
};

typedef struct
{
	struct Variable hash;
	union VariableValueInternal_u u;
	union VariableValueInternal_w w;
	union VariableValueInternal_v v;
	uint16_t nextSibling;
} VariableValueInternal;

typedef enum
{
	VAR_UNDEFINED,
	VAR_OBJECT,
	VAR_STRING,
	VAR_ISTRING,
	VAR_VECTOR,
	VAR_FLOAT,
	VAR_INTEGER,
	VAR_CODEPOS,
	VAR_PRECODEPOS,
	VAR_FUNCTION,
	VAR_STACK,
	VAR_ANIMATION,
	VAR_DEVELOPER_CODEPOS,
	VAR_INCLUDE_CODEPOS,
	VAR_THREAD_LIST,
	VAR_THREAD,
	VAR_NOTIFY_THREAD,
	VAR_TIME_THREAD,
	VAR_CHILD_THREAD,
	VAR_STRUCT,
	VAR_REMOVED_ENTITY,
	VAR_ENTITY,
	VAR_ARRAY,
	VAR_REMOVED_THREAD,
	VAR_COUNT
} var_type_t;

typedef struct
{
	const char *fieldBuffer;
	struct HunkUser *programHunkUser;
	uint16_t canonicalStrCount;
	byte developer;
	byte developer_script;
	byte evaluate;
	byte pad[3];
	const char *error_message;
	int error_index;
	unsigned int time;
	unsigned int timeArrayId;
	unsigned int pauseArrayId;
	unsigned int levelId;
	unsigned int gameId;
	unsigned int animId;
	unsigned int freeEntList;
	unsigned int tempVariable;
	byte bInited;
	byte pad2;
	uint16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	const char *programBuffer;
	const char *endScriptBuffer;
} scrVarPub_t;

struct function_stack_t
{
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};

struct function_frame_t
{
	struct function_stack_t fs;
	int topType;
};

typedef struct
{
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	struct function_frame_t *function_frame;
	VariableValue *top;
	byte debugCode;
	byte abort_on_error;
	byte terminal_error;
	byte pad;
	unsigned int inparamcount;
	unsigned int outparamcount;
	struct function_frame_t function_frame_start[32];
	VariableValue stack[2048];
} scrVmPub_t;

typedef struct 
{
	int value_count;
	int far_function_count;
	unsigned int loadedscripts;
	unsigned int scriptsPos;
	unsigned int builtinFunc;
	unsigned int builtinMeth;
	uint16_t *canonicalStrings;
	const char *in_ptr;
	const char *parseBuf;
	byte script_loading;
	byte allowedBreakpoint;
	int16_t pad;
	int developer_statement;
	char *opcodePos;
	unsigned int programLen;
	int func_table_size;
	int *func_table[1024];
} scrCompilePub_t;

typedef struct 
{
	const char *codePos;
	char *buf;
	const char *sourceBuf;
	int len;
	int sortedIndex;
	bool archive;
} SourceBufferInfo;

typedef struct
{
	SourceBufferInfo *sourceBufferLookup;
	unsigned int sourceBufferLookupLen;
	const char *scriptfilename;
	const char *sourceBuf;
} scrParserPub_t;

typedef struct 
{
	VariableValue eval_stack[2];
	const char *dialog_error_message;
	int loading;
	int starttime;
	unsigned int localVarsStack[2048];
} scrVmGlob_t;

typedef int	fileHandle_t;
typedef void *unzFile;
typedef void (*xfunction_t)();
typedef void (*xmethod_t)(scr_entref_t);
typedef void (*xcommand_t) (void);

typedef struct scr_function_s
{
	const char      *name;
	xfunction_t     call;
	qboolean        developer;
} scr_function_t;

typedef struct scr_method_s
{
	const char     *name;
	xmethod_t      call;
	qboolean       developer;
} scr_method_t;

struct directory_t
{
	char path[MAX_OSPATH];
	char gamedir[MAX_OSPATH];
};

union qfile_gus
{
	FILE *o;
	unzFile z;
};

struct qfile_us
{
	qfile_gus file;
	qboolean unique;
};

struct fileInIwd_t
{
	unsigned long pos;
	char *name;
	fileInIwd_t *next;
};

struct iwd_t
{
	char iwdFilename[MAX_OSPATH];
	char iwdBasename[MAX_OSPATH];
	char iwdGamename[MAX_OSPATH];
	unzFile handle;
	int checksum;
	int pure_checksum;
	int numFiles;
	int referenced;
	int hashSize;
	fileInIwd_t **hashTable;
	fileInIwd_t *buildBuffer;
};

struct fileHandleData_t
{
	qfile_us handleFiles;
	qboolean handleSync;
	int fileSize;
	int zipFilePos;
	qboolean zipFile;
	qboolean streamed;
	char name[MAX_ZPATH];
};

struct searchpath_t
{
	searchpath_t *next;
	iwd_t *iwd;
	directory_t *dir;
	qboolean localized;
	int language;
};

typedef struct tm_unz_s
{
    unsigned int tm_sec;
    unsigned int tm_min;
    unsigned int tm_hour;
    unsigned int tm_mday;
    unsigned int tm_mon;
    unsigned int tm_year;
} tm_unz;

typedef struct unz_global_info_s
{
    unsigned long number_entry;
    unsigned long size_comment;
} unz_global_info;

typedef struct unz_file_info_s
{
    unsigned long version;
    unsigned long version_needed;
    unsigned long flag;
    unsigned long compression_method;
    unsigned long dosDate;
    unsigned long crc;
    unsigned long compressed_size;
    unsigned long uncompressed_size;
    unsigned long size_filename;
    unsigned long size_file_extra;
    unsigned long size_file_comment;
    unsigned long disk_num_start;
    unsigned long internal_fa;
    unsigned long external_fa;
    tm_unz tmu_date;
} unz_file_info;

typedef enum
{
	EV_NONE = 0,
	EV_FOOTSTEP_RUN_DEFAULT,
	EV_FOOTSTEP_RUN_BARK,
	EV_FOOTSTEP_RUN_BRICK,
	EV_FOOTSTEP_RUN_CARPET,
	EV_FOOTSTEP_RUN_CLOTH,
	EV_FOOTSTEP_RUN_CONCRETE,
	EV_FOOTSTEP_RUN_DIRT,
	EV_FOOTSTEP_RUN_FLESH,
	EV_FOOTSTEP_RUN_FOLIAGE,
	EV_FOOTSTEP_RUN_GLASS,      // 10
	EV_FOOTSTEP_RUN_GRASS,
	EV_FOOTSTEP_RUN_GRAVEL,
	EV_FOOTSTEP_RUN_ICE,
	EV_FOOTSTEP_RUN_METAL,
	EV_FOOTSTEP_RUN_MUD,
	EV_FOOTSTEP_RUN_PAPER,
	EV_FOOTSTEP_RUN_PLASTER,
	EV_FOOTSTEP_RUN_ROCK,
	EV_FOOTSTEP_RUN_SAND,
	EV_FOOTSTEP_RUN_SNOW,       // 20
	EV_FOOTSTEP_RUN_WATER,
	EV_FOOTSTEP_RUN_WOOD,
	EV_FOOTSTEP_RUN_ASPHALT,
	EV_FOOTSTEP_WALK_DEFAULT,
	EV_FOOTSTEP_WALK_BARK,
	EV_FOOTSTEP_WALK_BRICK,
	EV_FOOTSTEP_WALK_CARPET,
	EV_FOOTSTEP_WALK_CLOTH,
	EV_FOOTSTEP_WALK_CONCRETE,
	EV_FOOTSTEP_WALK_DIRT,      // 30
	EV_FOOTSTEP_WALK_FLESH,
	EV_FOOTSTEP_WALK_FOLIAGE,
	EV_FOOTSTEP_WALK_GLASS,
	EV_FOOTSTEP_WALK_GRASS,
	EV_FOOTSTEP_WALK_GRAVEL,
	EV_FOOTSTEP_WALK_ICE,
	EV_FOOTSTEP_WALK_METAL,
	EV_FOOTSTEP_WALK_MUD,
	EV_FOOTSTEP_WALK_PAPER,
	EV_FOOTSTEP_WALK_PLASTER,   // 40
	EV_FOOTSTEP_WALK_ROCK,
	EV_FOOTSTEP_WALK_SAND,
	EV_FOOTSTEP_WALK_SNOW,
	EV_FOOTSTEP_WALK_WATER,
	EV_FOOTSTEP_WALK_WOOD,
	EV_FOOTSTEP_WALK_ASPHALT,
	EV_FOOTSTEP_PRONE_DEFAULT,
	EV_FOOTSTEP_PRONE_BARK,
	EV_FOOTSTEP_PRONE_BRICK,
	EV_FOOTSTEP_PRONE_CARPET,   // 50
	EV_FOOTSTEP_PRONE_CLOTH,
	EV_FOOTSTEP_PRONE_CONCRETE,
	EV_FOOTSTEP_PRONE_DIRT,
	EV_FOOTSTEP_PRONE_FLESH,
	EV_FOOTSTEP_PRONE_FOLIAGE,
	EV_FOOTSTEP_PRONE_GLASS,
	EV_FOOTSTEP_PRONE_GRASS,
	EV_FOOTSTEP_PRONE_GRAVEL,
	EV_FOOTSTEP_PRONE_ICE,
	EV_FOOTSTEP_PRONE_METAL,    // 60
	EV_FOOTSTEP_PRONE_MUD,
	EV_FOOTSTEP_PRONE_PAPER,
	EV_FOOTSTEP_PRONE_PLASTER,
	EV_FOOTSTEP_PRONE_ROCK,
	EV_FOOTSTEP_PRONE_SAND,
	EV_FOOTSTEP_PRONE_SNOW,
	EV_FOOTSTEP_PRONE_WATER,
	EV_FOOTSTEP_PRONE_WOOD,
	EV_FOOTSTEP_PRONE_ASPHALT,
	EV_JUMP_DEFAULT,            // 70
	EV_JUMP_BARK,
	EV_JUMP_BRICK,
	EV_JUMP_CARPET,
	EV_JUMP_CLOTH,
	EV_JUMP_CONCRETE,
	EV_JUMP_DIRT,
	EV_JUMP_FLESH,
	EV_JUMP_FOLIAGE,
	EV_JUMP_GLASS,
	EV_JUMP_GRASS,              // 80
	EV_JUMP_GRAVEL,
	EV_JUMP_ICE,
	EV_JUMP_METAL,
	EV_JUMP_MUD,
	EV_JUMP_PAPER,
	EV_JUMP_PLASTER,
	EV_JUMP_ROCK,
	EV_JUMP_SAND,
	EV_JUMP_SNOW,
	EV_JUMP_WATER,              // 90
	EV_JUMP_WOOD,
	EV_JUMP_ASPHALT,
	EV_LANDING_DEFAULT,
	EV_LANDING_BARK,
	EV_LANDING_BRICK,
	EV_LANDING_CARPET,
	EV_LANDING_CLOTH,
	EV_LANDING_CONCRETE,
	EV_LANDING_DIRT,
	EV_LANDING_FLESH,           // 100
	EV_LANDING_FOLIAGE,
	EV_LANDING_GLASS,
	EV_LANDING_GRASS,
	EV_LANDING_GRAVEL,
	EV_LANDING_ICE,
	EV_LANDING_METAL,
	EV_LANDING_MUD,
	EV_LANDING_PAPER,
	EV_LANDING_PLASTER,
	EV_LANDING_ROCK,            // 110
	EV_LANDING_SAND,
	EV_LANDING_SNOW,
	EV_LANDING_WATER,
	EV_LANDING_WOOD,
	EV_LANDING_ASPHALT,
	EV_LANDING_PAIN_DEFAULT,
	EV_LANDING_PAIN_BARK,
	EV_LANDING_PAIN_BRICK,
	EV_LANDING_PAIN_CARPET,
	EV_LANDING_PAIN_CLOTH,      // 120
	EV_LANDING_PAIN_CONCRETE,
	EV_LANDING_PAIN_DIRT,
	EV_LANDING_PAIN_FLESH,
	EV_LANDING_PAIN_FOLIAGE,
	EV_LANDING_PAIN_GLASS,
	EV_LANDING_PAIN_GRASS,
	EV_LANDING_PAIN_GRAVEL,
	EV_LANDING_PAIN_ICE,
	EV_LANDING_PAIN_METAL,
	EV_LANDING_PAIN_MUD,        // 130
	EV_LANDING_PAIN_PAPER,
	EV_LANDING_PAIN_PLASTER,
	EV_LANDING_PAIN_ROCK,
	EV_LANDING_PAIN_SAND,
	EV_LANDING_PAIN_SNOW,
	EV_LANDING_PAIN_WATER,
	EV_LANDING_PAIN_WOOD,
	EV_LANDING_PAIN_ASPHALT,
	EV_FOLIAGE_SOUND,
	EV_STANCE_FORCE_STAND,      // 140
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_ITEM_PICKUP,
	EV_AMMO_PICKUP,
	EV_NOAMMO,
	EV_EMPTYCLIP,
	EV_EMPTY_OFFHAND,
	EV_RESET_ADS,
	EV_RELOAD,                  // 150
	EV_RELOAD_FROM_EMPTY,
	EV_RELOAD_START,
	EV_RELOAD_END,
	EV_RAISE_WEAPON,
	EV_PUTAWAY_WEAPON,
	EV_WEAPON_ALT,
	EV_PULLBACK_WEAPON,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,    // 160
	EV_RECHAMBER_WEAPON,
	EV_EJECT_BRASS,
	EV_MELEE_SWIPE,
	EV_FIRE_MELEE,
	EV_PREP_OFFHAND,
	EV_USE_OFFHAND,
	EV_SWITCH_OFFHAND,
	EV_BINOCULAR_ENTER,
	EV_BINOCULAR_EXIT,
	EV_BINOCULAR_FIRE,          // 170
	EV_BINOCULAR_RELEASE,
	EV_BINOCULAR_DROP,
	EV_MELEE_HIT,
	EV_MELEE_MISS,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_QUADBARREL_1,
	EV_FIRE_QUADBARREL_2,
	EV_BULLET_TRACER,
	EV_SOUND_ALIAS,
	EV_SOUND_ALIAS_AS_MASTER,   // 180
	EV_BULLET_HIT_SMALL,
	EV_BULLET_HIT_LARGE,
	EV_SHOTGUN_HIT,
	EV_BULLET_HIT_AP,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	EV_GRENADE_BOUNCE,
	EV_GRENADE_EXPLODE,
	EV_ROCKET_EXPLODE,
	EV_ROCKET_EXPLODE_NOMARKS,  // 190
	EV_CUSTOM_EXPLODE,
	EV_CUSTOM_EXPLODE_NOMARKS,
	EV_BULLET,
	EV_PLAY_FX,
	EV_PLAY_FX_ON_TAG,
	EV_EARTHQUAKE,
	EV_GRENADE_SUICIDE,
	EV_OBITUARY                 // 198
} entity_event_t;

#define HMAX 256 /* Maximum symbol */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype
{
	struct  nodetype *left, *right, *parent; /* tree structure */
	struct  nodetype *next, *prev; /* doubly-linked list */
	struct  nodetype **head; /* highest ranked node in block */
	int weight;
	int symbol;
} node_t;

typedef struct
{
	int blocNode;
	int blocPtrs;

	node_t*     tree;
	node_t*     lhead;
	node_t*     ltail;
	node_t*     loc[HMAX + 1];
	node_t**    freelist;

	node_t nodeList[768];
	node_t*     nodePtrs[768];
} huff_t;

typedef struct
{
	huff_t compressor;
	huff_t decompressor;
} huffman_t;

typedef struct netField_s
{
	char *name;
	int offset;
	int bits;
} netField_t;

enum StanceState
{
	CL_STANCE_STAND = 0x0,
	CL_STANCE_CROUCH = 0x1,
	CL_STANCE_PRONE = 0x2,
	CL_STANCE_DIVE_TO_PRONE = 0x3,
};

typedef struct DObjTrace_s
{
	float fraction;
	int surfaceflags;
	vec3_t normal;
	unsigned short partName;
	unsigned short partGroup;
} DObjTrace_t;

typedef struct trace_s
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	uint16_t entityNum;
	uint16_t partName;
	uint16_t partGroup;
	byte allsolid;
	byte startsolid;
} trace_t;

typedef struct traceExtents_s
{
	vec3_t start;
	vec3_t end;
	vec3_t invDelta;
} traceExtents_t;

typedef struct pointtrace_s
{
	traceExtents_t extents;
	int passEntityNum;
	int ignoreEntNum;
	int contentmask;
	int bLocational;
	unsigned char *priorityMap;
} pointtrace_t;

typedef struct moveclip_s
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t outerSize;
	traceExtents_t extents;
	int passEntityNum;
	int passOwnerNum;
	int contentmask;
} moveclip_t;

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s
{
	netadrtype_t type;
	unsigned char adr[4];
	int lastTime;
	signed char	burst;
	long hash;
	leakyBucket_t *prev, *next;
};

typedef struct usercmd_s
{
	int serverTime;
	int buttons;
	byte weapon;
	byte offHandIndex;
	int angles[3];
	char forwardmove;
	char rightmove;
} usercmd_t;

typedef void netProfileInfo_t;

typedef struct
{
	int outgoingSequence;
	netsrc_t sock;
	int dropped;
	int incomingSequence;
	netadr_t remoteAddress;
	int qport;
	int fragmentSequence;
	int fragmentLength;
	byte fragmentBuffer[MAX_MSGLEN];
	qboolean unsentFragments;
	int unsentFragmentStart;
	int unsentLength;
	byte unsentBuffer[MAX_MSGLEN];
	netProfileInfo_t *netProfile;
} netchan_t;

typedef struct
{
	char command[1024];
	int cmdTime;
	int cmdType;
} reliableCommands_t;

typedef struct
{
	netadr_t adr;
	int challenge;
	int time;
	int pingTime;
	int firstTime;
	int firstPing;
	qboolean connected;
	int guid;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char pbguid[64];
	int ipAuthorize;
#endif
} challenge_t; // verified for 1.0, guessed for 1.2 and 1.3

typedef enum
{
	TR_STATIONARY = 0,
	TR_INTERPOLATE = 1,
	TR_LINEAR = 2,
	TR_LINEAR_STOP = 3,
	TR_SINE = 4,
	TR_GRAVITY = 5,
	TR_GRAVITY_PAUSED = 6,
	TR_ACCELERATE = 7,
	TR_DECCELERATE = 8
} trType_t;

typedef struct
{
	trType_t trType;
	int trTime;
	int trDuration;
	vec3_t trBase;
	vec3_t trDelta;
} trajectory_t;

typedef struct entityState_s
{
	int number;
	entityType_t eType;
	int eFlags;
	trajectory_t pos;
	trajectory_t apos;
	int time;
	int time2;
	vec3_t origin2;
	vec3_t angles2;
	int otherEntityNum;
	int attackerEntityNum;
	int groundEntityNum;
	int constantLight;
	int loopSound;
	int surfType;
	int index; // modelIndex
	int clientNum;
	int iHeadIcon;
	int iHeadIconTeam;
	int solid;
	int eventParm;
	int eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int weapon;
	int legsAnim;
	int torsoAnim;
	int leanf;
	int scale; // used as loopfxid, hintstring, ... and doesn't actually scale a player's model size
	int dmgFlags;
	int animMovetype;
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
} entityState_t; // verified

typedef struct
{
	byte linked;
	byte bmodel;
	byte svFlags;
	byte pad1;
	int clientMask[2];
	byte inuse;
	byte pad2[3];
	int broadcastTime;
	vec3_t mins;
	vec3_t maxs;
	int contents;
	vec3_t absmin;
	vec3_t absmax;
	vec3_t currentOrigin;
	vec3_t currentAngles;
	int ownerNum;
	int eventTime;
} entityShared_t; // verified

typedef enum
{
	STAT_HEALTH,
	STAT_DEAD_YAW,
	STAT_MAX_HEALTH,
	STAT_FRIENDLY_LOOKAT_CLIENTNUM,
	STAT_FRIENDLY_LOOKAT_HEALTH,
	STAT_SPAWN_COUNT
} statIndex_t;

typedef enum
{
	PLAYERVIEWLOCK_NONE = 0x0,
	PLAYERVIEWLOCK_FULL = 0x1,
	PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
	PLAYERVIEWLOCKCOUNT = 0x3,
} ViewLockTypes_t;

typedef enum
{
	OBJST_EMPTY = 0x0,
	OBJST_ACTIVE = 0x1,
	OBJST_INVISIBLE = 0x2,
	OBJST_DONE = 0x3,
	OBJST_CURRENT = 0x4,
	OBJST_FAILED = 0x5,
	OBJST_NUMSTATES = 0x6,
} objectiveState_t;

typedef struct objective_s
{
	objectiveState_t state;
	vec3_t origin;
	int entNum;
	int teamNum;
	int icon;
} objective_t;

typedef enum
{
	HE_TYPE_FREE = 0x0,
	HE_TYPE_TEXT = 0x1,
	HE_TYPE_VALUE = 0x2,
	HE_TYPE_PLAYERNAME = 0x3,
	HE_TYPE_MAPNAME = 0x4,
	HE_TYPE_GAMETYPE = 0x5,
	HE_TYPE_MATERIAL = 0x6,
	HE_TYPE_TIMER_DOWN = 0x7,
	HE_TYPE_TIMER_UP = 0x8,
	HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
	HE_TYPE_TENTHS_TIMER_UP = 0xA,
	HE_TYPE_CLOCK_DOWN = 0xB,
	HE_TYPE_CLOCK_UP = 0xC,
	HE_TYPE_WAYPOINT = 0xD,
	HE_TYPE_COUNT = 0xE,
} he_type_t;

typedef struct
{
	char r;
	char g;
	char b;
	char a;
} hudelem_colorsplit_t;

typedef union
{
	hudelem_colorsplit_t split;
	int rgba;
} hudelem_color_t;

typedef struct hudelem_s
{
	he_type_t type;
	float x;
	float y;
	float z;
	float fontScale;
	int font;
	int alignOrg;
	int alignScreen;
	hudelem_color_t color;
	hudelem_color_t fromColor;
	int fadeStartTime;
	int fadeTime;
	int label;
	int width;
	int height;
	int materialIndex;
	int fromWidth;
	int fromHeight;
	int scaleStartTime;
	int scaleTime;
	float fromX;
	float fromY;
	int fromAlignOrg;
	int fromAlignScreen;
	int moveStartTime;
	int moveTime;
	int time;
	int duration;
	float value;
	int text;
	float sort;
	hudelem_color_t foreground;
} hudelem_t; // verified

typedef struct game_hudelem_s
{
	hudelem_t elem;
	int clientNum;
	int team;
	int archived;
} game_hudelem_t;

typedef struct hudElemState_s
{
	hudelem_t current[31];
	hudelem_t archival[31];
} hudElemState_t;

typedef struct
{
	float yaw;
	int timer;
	int transIndex;
	int flags;
} mantleState_t;

typedef enum
{
	PM_NORMAL = 0x0,
	PM_NORMAL_LINKED = 0x1,
	PM_NOCLIP = 0x2,
	PM_UFO = 0x3,
	PM_SPECTATOR = 0x4,
	PM_INTERMISSION = 0x5,
	PM_DEAD = 0x6,
	PM_DEAD_LINKED = 0x7,
} pmtype_t;

typedef struct playerState_s
{
	int commandTime;
	pmtype_t pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	vec2_t oldVelocity; // 48
	int weaponTime;
	int weaponDelay;
	int grenadeTimeLeft;
	int weaponRestrictKickTime;
	int foliageSoundTime;
	int gravity;
	float leanf;
	int speed;
	vec3_t delta_angles;
	int groundEntityNum;
	vec3_t vLadderVec;
	int jumpTime;
	float jumpOriginZ;
	int legsTimer;
	int legsAnim;
	int torsoTimer;
	int torsoAnim;
	int legsAnimDuration;
	int torsoAnimDuration;
	int damageTimer;
	int damageDuration;
	int flinchYaw;
	int movementDir;
	int eFlags;
	int eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int oldEventSequence;
	int clientNum;
	int offHandIndex;
	unsigned int weapon;
	weaponstate_t weaponstate;
	float fWeaponPosFrac;
	int adsDelayTime;
	int viewmodelIndex;
	vec3_t viewangles;
	int viewHeightTarget;
	float viewHeightCurrent;
	int viewHeightLerpTime;
	int viewHeightLerpTarget;
	int viewHeightLerpDown;
	float viewHeightLerpPosAdj;
	vec2_t viewAngleClampBase;
	vec2_t viewAngleClampRange;
	int damageEvent;
	int damageYaw;
	int damagePitch;
	int damageCount;
	int stats[6];
	int ammo[128];
	int ammoclip[128];
	unsigned int weapons[4];
	byte weaponslots[8];
	unsigned int weaponrechamber[4];
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	float proneTorsoPitch;
	ViewLockTypes_t viewlocked;
	int viewlocked_entNum;
	int cursorHint;
	int cursorHintString;
	int cursorHintEntIndex;
	int iCompassFriendInfo;
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
	float holdBreathScale;
	int holdBreathTimer;
	mantleState_t mantleState;
	int entityEventSequence;
	int weapAnim;
	float aimSpreadScale;
	int shellshockIndex;
	int shellshockTime;
	int shellshockDuration;
	objective_t objective[16];
	int deltaTime;
	hudElemState_t hud;
} playerState_t;

typedef struct clientState_s
{
	int clientIndex;
	team_t team;
	int modelIndex;
	char name[32];
	int attachModelIndex[6];
	int attachTagIndex[6];
} clientState_t;

typedef struct
{
	sessionState_t sessionState;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int score;
	int deaths;
	uint16_t pers;
	byte pad2;
	byte pad;
	clientConnected_t connected;
	usercmd_t cmd;
	usercmd_t oldcmd;
	qboolean localClient;
	qboolean predictItemPickup;
	char name[32];
	int maxHealth;
	int enterTime;
	int voteCount;
	int teamVoteCount;
	float moveSpeedScaleMultiplier;
	int viewmodelIndex;
	qboolean noSpectate;
	int teamInfo;
	clientState_t state;
	int psOffsetTime;
} clientSession_t;

struct gclient_s
{
	playerState_t ps;
	clientSession_t sess;
	int spectatorClient;
	qboolean noclip;
	qboolean ufo;
	qboolean bFrozen;
	int lastCmdTime;
	int buttons;
	int oldbuttons;
	int latched_buttons;
	int buttonsSinceLastFrame;
	vec3_t oldOrigin;
	float fGunPitch;
	float fGunYaw;
	int damage_blood;
	vec3_t damage_from;
	qboolean damage_fromWorld;
	int accurateCount;
	int accuracy_shots;
	int accuracy_hits;
	int inactivityTime;
	qboolean inactivityWarning;
	int lastVoiceTime;
	int switchTeamTime;
	float currentAimSpreadScale; // 10256
	gentity_t *persistantPowerup;
	int portalID;
	int dropWeaponTime;
	int sniperRifleFiredTime;
	float sniperRifleMuzzleYaw;
	int PCSpecialPickedUpCount;
	gentity_t *pLookatEnt; // needs a NULL check, otherwise crash.
	int useHoldEntity;
	int useHoldTime;
	int iLastCompassFriendlyInfoEnt;
	int compassPingTime;
	int damageTime;
	float v_dmg_roll;
	float v_dmg_pitch;
	vec3_t swayViewAngles; // 10316
	vec3_t swayOffset;
	vec3_t swayAngles;
	vec3_t vLastMoveAng;
	float fLastIdleFactor;
	vec3_t vGunOffset;
	vec3_t vGunSpeed;
	int weapIdleTime;
	int lastServerTime;
	int lastSpawnTime;
}; // verified

struct turretInfo_s
{
	int inuse;
	int flags;
	int fireTime;
	vec2_t arcmin;
	vec2_t arcmax;
	float dropPitch;
	int stance;
	int prevStance;
	int fireSndDelay;
	vec3_t userOrigin;
	float playerSpread;
	int triggerDown;
	char fireSnd;
	char fireSndPlayer;
	char stopSnd;
	char stopSndPlayer;
};

struct item_ent_t
{
	int count;
	uint16_t index;
	uint16_t pad;
};

struct trigger_ent_t
{
	int threshold;
	int accumulate;
	int timestamp;
	int singleUserEntIndex;
	int damage;
	byte requireLookAt;
};

struct mover_ent_t
{
	float decelTime;
	float aDecelTime;
	float speed;
	float aSpeed;
	float midTime;
	float aMidTime;
	vec3_t pos1;
	vec3_t pos2;
	vec3_t pos3;
	vec3_t apos1;
	vec3_t apos2;
	vec3_t apos3;
};

struct corpse_ent_t
{
	int deathAnimStartTime;
};

typedef struct tagInfo_s
{
	struct gentity_s *parent;
	struct gentity_s *next;
	uint16_t name;
	uint16_t pad;
	int index;
	float axis[4][3];
	float parentInvAxis[4][3];
} tagInfo_t;

struct gentity_s
{
	entityState_t s;
	entityShared_t r;
	struct gclient_s *client; // 344
	turretInfo_s *pTurretInfo; // 348
	byte physicsObject; // 352
	byte takedamage; // 353
	byte active; // 354
	byte nopickup; // 355
	byte model; // 356
	byte attachIgnoreCollision; // 357
	entHandlers_t handler; // 358
	byte team; // 359
	uint16_t classname; // 360
	uint16_t target;
	uint16_t targetname;
	uint16_t padding;
	int spawnflags;
	int flags;
	int eventTime;
	qboolean freeAfterEvent; // 380
	qboolean unlinkAfterEvent; // 384
	int clipmask; // 388
	int processedFrame; // 392
	gentity_t *parent; // 396
	int nextthink; // 400
	int health; // 404
	int maxHealth; // 408
	int damage; // 412
	int count; // 416
	gentity_t *chain; // 420
	union { // 424
		struct item_ent_t item[2];
		struct trigger_ent_t trigger;
		struct mover_ent_t mover;
		struct corpse_ent_t corpse;
	};
	tagInfo_t *tagInfo; // 520
	gentity_t *tagChildren; // 524
	uint16_t attachModelNames[6];
	uint16_t attachTagNames[6];
	int useCount; // 552
	gentity_t *nextFree; // 556
}; // verified

typedef struct
{
	int numSnapshotEntities;
	int snapshotEntities[MAX_SNAPSHOT_ENTITIES];
} snapshotEntityNumbers_t;

typedef struct
{
	playerState_t ps;
	int num_entities;
	int num_clients;
	int first_entity;
	int first_client;
	unsigned int messageSent;
	unsigned int messageAcked;
	int messageSize;
} clientSnapshot_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	char talkerNum;
	char data[MAX_VOICEPACKETDATALEN];
	int dataLen;
} VoicePacket_t;
#pragma pack(pop)

typedef struct client_s
{
	clientConnectState_t state;
	qboolean delayed;
	const char *delayDropMsg;
	char userinfo[1024];
	reliableCommands_t reliableCommands[128];
	int reliableSequence;
	int reliableAcknowledge;
	int reliableSent;
	int messageAcknowledge;
	int gamestateMessageNum;
	int challenge;
	usercmd_t lastUsercmd;
	int lastClientCommand;
	char lastClientCommandString[1024];
	gentity_t *gentity;
	char name[32];
	char downloadName[MAX_QPATH];
	fileHandle_t download;
	int downloadSize;
	int downloadCount;
	int downloadClientBlock;
	int downloadCurrentBlock;
	int downloadXmitBlock;
	unsigned char *downloadBlocks[MAX_DOWNLOAD_WINDOW];
	int downloadBlockSize[MAX_DOWNLOAD_WINDOW];
	qboolean downloadEOF;
	int downloadSendTime;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char wwwDownloadURL[MAX_OSPATH];
	qboolean wwwDownload;
	qboolean wwwDownloadStarted;
	qboolean wwwDlAck;
	qboolean wwwDl_failed;
#endif
	int deltaMessage;
	int nextReliableTime;
	int lastPacketTime;
	int lastConnectTime;
	int nextSnapshotTime;
	qboolean rateDelayed;
	int timeoutCount;
	clientSnapshot_t frames[PACKET_BACKUP];
	int ping;
	int rate;
	int snapshotMsec;
	int pureAuthentic;
	netchan_t netchan;
	int guid;
	unsigned short clscriptid;
	int bot;
	int serverId;
	VoicePacket_t voicedata[MAX_VOICEPACKETS];
	int unsentVoiceData;
	byte mutedClients[MAX_CLIENTS];
	byte hasVoip;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char pbguid[64];
#endif
} client_t;

typedef struct
{
	int svFlags;
	int clientMask[2];
	vec3_t absmin;
	vec3_t absmax;
} archivedEntityShared_t;

typedef struct archivedEntity_s
{
	entityState_t s;
	archivedEntityShared_t r;
} archivedEntity_t;

typedef struct archivedSnapshot_s
{
	int start;
	int size;
} archivedSnapshot_t;

typedef struct cachedSnapshot_s
{
	int archivedFrame;
	int time;
	int num_entities;
	int first_entity;
	int num_clients;
	int first_client;
	int usesDelta;
} cachedSnapshot_t;

typedef struct cachedClient_s
{
	int playerStateExists;
	clientState_t cs;
	playerState_t ps;
} cachedClient_t;

typedef struct
{
	qboolean initialized;
	int time;
	int snapFlagServerBit;
	client_t *clients;
	int numSnapshotEntities;
	int numSnapshotClients;
	int nextSnapshotEntities;
	int nextSnapshotClients;
	entityState_t *snapshotEntities;
	clientState_t *snapshotClients;
	int archivedSnapshotEnabled;
	int nextArchivedSnapshotFrames;
	archivedSnapshot_t *archivedSnapshotFrames;
	byte *archivedSnapshotBuffer;
	int nextArchivedSnapshotBuffer;
	int nextCachedSnapshotEntities;
	int nextCachedSnapshotClients;
	int nextCachedSnapshotFrames;
	archivedEntity_t *cachedSnapshotEntities;
	cachedClient_t *cachedSnapshotClients;
	cachedSnapshot_t *cachedSnapshotFrames;
	int nextHeartbeatTime;
	int nextStatusResponseTime;
	challenge_t challenges[1024];
	netadr_t redirectAddress;
	netadr_t authorizeAddress;
	char netProfilingBuf[1504]; // shouldn't that be at most [252]? else we run into sv_offset
} serverStatic_t; // verified

typedef struct
{
	const char *key;
	const char *value;
} keyValueStr_t;

typedef struct
{
	byte spawnVarsValid;
	int numSpawnVars;
	keyValueStr_t spawnVars[64];
	int numSpawnVarChars;
	char spawnVarChars[2048];
} SpawnVar;

typedef struct
{
	uint16_t entnum;
	uint16_t otherEntnum;
	int useCount;
	int otherUseCount;
} trigger_info_t;

struct com_parse_mark_t
{
	int lines;
	const char *text;
	int ungetToken;
	int backup_lines;
	const char *backup_text;
};

typedef struct
{
	struct gclient_s *clients;
	struct gentity_s *gentities;
	int gentitySize;
	int num_entities;
	struct gentity_s *firstFreeEnt;
	struct gentity_s *lastFreeEnt;
	fileHandle_t logFile;
	int initializing;
	int clientIsSpawning;
	objective_t objectives[16];
	int maxclients;
	int framenum;
	int time;
	int previousTime;
	int frameTime;
	int startTime;
	int teamScores[TEAM_NUM_TEAMS];
	int lastTeammateHealthTime;
	qboolean bUpdateScoresForIntermission;
	int manualNameChange;
	int numConnectedClients;
	int sortedClients[MAX_CLIENTS];
	char voteString[1024];
	char voteDisplayString[1024];
	int voteTime; // 711
	int voteExecuteTime;
	int voteYes;
	int voteNo;
	int numVotingClients;
	byte gap[2072];
	SpawnVar spawnVars;
	int savePersist;
	struct gentity_s *droppedWeaponCue[32];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	int remapCount;
	int currentPlayerClone;
	trigger_info_t pendingTriggerList[256];
	trigger_info_t currentTriggerList[256];
	int pendingTriggerListSize;
	int currentTriggerListSize;
	int finished;
	int bPlayerIgnoreRadiusDamage;
	int bPlayerIgnoreRadiusDamageLatched;
	int registerWeapons;
	int bRegisterItems;
	int currentEntityThink;
	void *openScriptIOFileHandles[1];
	char *openScriptIOFileBuffers[1];
	com_parse_mark_t currentScriptIOLineMark[1];
} level_locals_t;

typedef enum
{
	SS_DEAD,
	SS_LOADING,
	SS_GAME
} serverState_t;

typedef struct svEntity_s
{
	uint16_t worldSector;
	uint16_t nextEntityInWorldSector;
	archivedEntity_t baseline;
	int numClusters;
	int clusternums[MAX_ENT_CLUSTERS];
	int	lastCluster;
	int	linkcontents;
	float linkmin[2];
	float linkmax[2];
} svEntity_t;

typedef struct sharedEntity_s
{
	entityState_t s;
	entityShared_t r;
} sharedEntity_t;

typedef struct
{
	serverState_t state;
	qboolean restarting;
	int start_frameTime;
	int	checksumFeed;
	int timeResidual;
	int nextFrameTime;
	struct cmodel_s *models[MAX_MODELS];
	char *configstrings[MAX_CONFIGSTRINGS];
	svEntity_t svEntities[MAX_GENTITIES];
	char *entityParsePoint;
	gentity_t *gentities;
	int gentitySize;
	int	num_entities;
	playerState_t *gameClients;
	int gameClientSize;
	int	skelTimeStamp;
	int	skelMemPos;
	int	bpsWindow[MAX_BPS_WINDOW];
	int	bpsWindowSteps;
	int	bpsTotalBytes;
	int	bpsMaxBytes;
	int	ubpsWindow[MAX_BPS_WINDOW];
	int	ubpsTotalBytes;
	int	ubpsMaxBytes;
	float ucompAve;
	int	ucompNum;
	char gametype[MAX_QPATH];
} server_t; // verified

typedef enum weapType_t
{
	WEAPTYPE_BULLET = 0x0,
	WEAPTYPE_GRENADE = 0x1,
	WEAPTYPE_PROJECTILE = 0x2,
	WEAPTYPE_BINOCULARS = 0x3,
	WEAPTYPE_NUM = 0x4
} weapType_t;

typedef enum weapClass_t
{
	WEAPCLASS_RIFLE = 0x0,
	WEAPCLASS_MG = 0x1,
	WEAPCLASS_SMG = 0x2,
	WEAPCLASS_SPREAD = 0x3,
	WEAPCLASS_PISTOL = 0x4,
	WEAPCLASS_GRENADE = 0x5,
	WEAPCLASS_ROCKETLAUNCHER = 0x6,
	WEAPCLASS_TURRET = 0x7,
	WEAPCLASS_NON_PLAYER = 0x8,
	WEAPCLASS_ITEM = 0x9,
	WEAPCLASS_NUM = 0xA
} weapClass_t;

typedef enum weapSlot_t
{
	WEAPSLOT_NONE = 0x0,
	WEAPSLOT_PRIMARY = 0x1,
	WEAPSLOT_PRIMARYB = 0x2,
	WEAPSLOT_NUM = 0x3
} weapSlot_t;

typedef enum OffhandClass_t
{
	OFFHAND_CLASS_NONE = 0x0,
	OFFHAND_CLASS_FRAG_GRENADE = 0x1,
	OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
	OFFHAND_CLASS_COUNT = 0x3
} OffhandClass_t;

typedef enum weapStance_t
{
	WEAPSTANCE_STAND = 0x0,
	WEAPSTANCE_DUCK = 0x1,
	WEAPSTANCE_PRONE = 0x2,
	WEAPSTANCE_NUM = 0x3
} weapStance_t;

typedef enum weapOverlayReticle_t
{
	WEAPOVERLAYRETICLE_NONE = 0x0,
	WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
	WEAPOVERLAYRETICLE_NUM = 0x2
} weapOverlayReticle_t;

typedef enum weapProjExposion_t
{
	WEAPPROJEXP_GRENADE = 0x0,
	WEAPPROJEXP_MOLOTOV = 0x1,
	WEAPPROJEXP_ROCKET = 0x2,
	WEAPPROJEXP_NONE = 0x3,
	WEAPPROJEXP_NUM = 0x4
} weapProjExposion_t;

typedef enum
{
	SASYS_UI = 0,
	SASYS_CGAME = 1,
	SASYS_GAME = 2,
	SASYS_COUNT = 3
} snd_alias_system_t;

typedef enum
{
	SAT_UNKNOWN = 0,
	SAT_LOADED = 1,
	SAT_STREAMED = 2,
	SAT_PRIMED = 3,
	SAT_COUNT = 4
} snd_alias_type_t;

typedef struct
{
	unsigned int format;
	void *data_ptr;
	int data_len;
	int rate;
	unsigned int bits;
	unsigned int channels;
	int samples;
	int block_size;
	void *initial_ptr;
} _AILSOUNDINFO;

typedef struct
{
	_AILSOUNDINFO info;
	byte data;
} MssSound;

typedef struct 
{
	const char *soundName;
	MssSound *fileMem;
	byte isStreamFound;
	byte pad0[3];
	snd_alias_type_t type;
} SoundFile;

typedef struct
{
	int count;
	SoundFile *files;
} SoundFileInfo;

typedef struct
{
	float radius;
	float mindist;
	float maxdist;
} volumeFalloffCurve_t;

struct snd_alias_build_s
{
	char szSourceFile[64];
	char szAliasName[64];
	char szSecondaryAliasName[64];
	int subtitleText;
	int iSequence;
	char szSoundFile[64];
	int permSoundFile;
	float fVolMin;
	float fVolMax;
	float fVolMod;
	float fPitchMin;
	float fPitchMax;
	float fDistMin;
	float fDistMax;
	int iChannel;
	snd_alias_type_t eType;
	volumeFalloffCurve_t *volumeFalloffCurve;
	float fSlavePercentage;
	float fProbability;
	float fLfePercentage;
	int startDelay;
	byte bLooping;
	byte bMaster;
	byte bSlave;
	byte bFullDryLevel;
	byte bNoWetLevel;
	byte error;
	byte keep;
	byte pad0;
	int pSameSoundFile; // SoundFile* ?
	snd_alias_build_s *pNext;
};

typedef struct
{
	const char *pszAliasName;
	const char *pszSubtitle;
	const char *pszSecondaryAliasName;
	SoundFile *soundFile;
	int iSequence;
	float fVolMin;
	float fVolMax;
	float fPitchMin;
	float fPitchMax;
	float fDistMin;
	float fDistMax;
	int flags;
	float fSlavePercentage;
	float fProbability;
	float fLfePercentage;
	int startDelay;
	volumeFalloffCurve_t *volumeFalloffCurve;
} snd_alias_t;

struct snd_alias_list_t
{
	const char *aliasName;
	snd_alias_t *head;
	int count;
	snd_alias_list_t *pHashNext;
};

typedef const char FxEffectDef_t;
typedef const char Material_t;

typedef struct WeaponDef_t
{
	const char *szInternalName;
	const char *szDisplayName;
	const char *szOverlayName;
	const char *szGunXModel;
	const char *szHandXModel;
	const char *szXAnims[23];
	const char *szModeName;
	int playerAnimType;
	weapType_t weapType;
	weapClass_t weapClass;
	weapSlot_t weapSlot;
	OffhandClass_t offhandClass;
	int bSlotStackable;
	weapStance_t stance;
	FxEffectDef_t *szViewFlashEffect;
	FxEffectDef_t *szWorldFlashEffect;
	snd_alias_list_t *szPickupSound;
	snd_alias_list_t *szAmmoPickupSound;
	snd_alias_list_t *szProjectileSound;
	snd_alias_list_t *szPullbackSound;
	snd_alias_list_t *szFireSound;
	snd_alias_list_t *szFireSoundPlayer;
	snd_alias_list_t *szFireLoopSound;
	snd_alias_list_t *szFireLoopSoundPlayer;
	snd_alias_list_t *szFireStopSound;
	snd_alias_list_t *szFireStopSoundPlayer;
	snd_alias_list_t *szFireLastSound;
	snd_alias_list_t *szFireLastSoundPlayer;
	snd_alias_list_t *meleeSwipeSound;
	snd_alias_list_t *szRechamberSound;
	snd_alias_list_t *szRechamberSoundPlayer;
	snd_alias_list_t *szReloadSound;
	snd_alias_list_t *szReloadSoundPlayer;
	snd_alias_list_t *szReloadEmptySound;
	snd_alias_list_t *szReloadEmptySoundPlayer;
	snd_alias_list_t *szReloadStartSound;
	snd_alias_list_t *szReloadStartSoundPlayer;
	snd_alias_list_t *szReloadEndSound;
	snd_alias_list_t *szReloadEndSoundPlayer;
	snd_alias_list_t *szRaiseSound;
	snd_alias_list_t *szAltSwitchSound;
	snd_alias_list_t *szPutawaySound;
	snd_alias_list_t *szNoteTrackSoundA;
	snd_alias_list_t *szNoteTrackSoundB;
	snd_alias_list_t *szNoteTrackSoundC;
	snd_alias_list_t *szNoteTrackSoundD;
	FxEffectDef_t *szShellEjectEffect;
	FxEffectDef_t *szLastShotEjectEffect;
	Material_t *szReticleCenter;
	Material_t *szReticleSide;
	int iReticleCenterSize;
	int iReticleSideSize;
	int iReticleMinOfs;
	float vStandMove[3];
	float vStandRot[3];
	float vDuckedOfs[3];
	float vDuckedMove[3];
	float vDuckedRot[3];
	float vProneOfs[3];
	float vProneMove[3];
	float vProneRot[3];
	float fPosMoveRate;
	float fPosProneMoveRate;
	float fStandMoveMinSpeed;
	float fDuckedMoveMinSpeed;
	float fProneMoveMinSpeed;
	float fPosRotRate;
	float fPosProneRotRate;
	float fStandRotMinSpeed;
	float fDuckedRotMinSpeed;
	float fProneRotMinSpeed;
	const char *szWorldModel;
	Material_t *szHudIcon;
	Material_t *szModeIcon;
	int iStartAmmo;
	const char *szAmmoName;
	int iAmmoIndex;
	const char *szClipName;
	int iClipIndex;
	int iMaxAmmo;
	int iClipSize;
	int shotCount;
	const char *szSharedAmmoCapName;
	int iSharedAmmoCapIndex;
	int iSharedAmmoCap;
	int damage;
	int playerDamage;
	int iMeleeDamage;
	int iDamageType;
	int iFireDelay;
	int iMeleeDelay;
	int iFireTime;
	int iRechamberTime;
	int iRechamberBoltTime;
	int iHoldFireTime;
	int iMeleeTime;
	int iReloadTime;
	int iReloadEmptyTime;
	int iReloadAddTime;
	int iReloadStartTime;
	int iReloadStartAddTime;
	int iReloadEndTime;
	int iDropTime;
	int iRaiseTime;
	int iAltDropTime;
	int iAltRaiseTime;
	int quickDropTime;
	int quickRaiseTime;
	int iFuseTime;
	float autoAimRange;
	float slowdownAimRange;
	float slowdownAimRangeAds;
	float lockonAimRange;
	float lockonAimRangeAds;
	float enemyCrosshairRange;
	float fMoveSpeedScale;
	float fAdsZoomFov;
	float fAdsZoomInFrac;
	float fAdsZoomOutFrac;
	Material_t *szOverlayMaterial;
	weapOverlayReticle_t overlayReticle;
	float overlayWidth;
	float overlayHeight;
	float fAdsBobFactor;
	float fAdsViewBobMult;
	float fHipSpreadStandMin;
	float fHipSpreadDuckedMin;
	float fHipSpreadProneMin;
	float hipSpreadStandMax;
	float hipSpreadDuckedMax;
	float hipSpreadProneMax;
	float fHipSpreadDecayRate;
	float fHipSpreadFireAdd;
	float fHipSpreadTurnAdd;
	float fHipSpreadMoveAdd;
	float fHipSpreadDuckedDecay;
	float fHipSpreadProneDecay;
	float fHipReticleSidePos;
	int iAdsTransInTime;
	int iAdsTransOutTime;
	float fAdsIdleAmount;
	float fHipIdleAmount;
	float adsIdleSpeed;
	float hipIdleSpeed;
	float fIdleCrouchFactor;
	float fIdleProneFactor;
	float fGunMaxPitch;
	float fGunMaxYaw;
	float swayMaxAngle;
	float swayLerpSpeed;
	float swayPitchScale;
	float swayYawScale;
	float swayHorizScale;
	float swayVertScale;
	float swayShellShockScale;
	float adsSwayMaxAngle;
	float adsSwayLerpSpeed;
	float adsSwayPitchScale;
	float adsSwayYawScale;
	float adsSwayHorizScale;
	float adsSwayVertScale;
	int bRifleBullet;
	int armorPiercing;
	int bSemiAuto;
	int bBoltAction;
	int bADSPositionInfo;
	int bRechamberWhileAds;
	float adsViewErrorMin;
	float adsViewErrorMax;
	int bCookOffHold;
	int bClipOnly;
	int bWideListIcon;
	int bADSFire;
	Material_t *killIcon;
	int wideKillIcon;
	int flipKillIcon;
	int bNoPartialReload;
	int bSegmentedReload;
	int iReloadAmmoAdd;
	int iReloadStartAdd;
	const char *szAltWeaponName;
	unsigned int iAltWeaponIndex;
	int iDropAmmoMin;
	int iDropAmmoMax;
	int iExplosionRadius;
	int iExplosionInnerDamage;
	int iExplosionOuterDamage;
	int iProjectileSpeed;
	int iProjectileSpeedUp;
	const char *szProjectileModel;
	weapProjExposion_t projExplosion;
	FxEffectDef_t *szProjExplosionEffect;
	snd_alias_list_t *szProjExplosionSound;
	int bProjImpactExplode;
	float parallelBounce[23];
	float perpendicularBounce[23];
	FxEffectDef_t *szProjTrailEffect;
	int iProjectileDLight;
	float vProjectileColor[3];
	float fAdsAimPitch;
	float fAdsCrosshairInFrac;
	float fAdsCrosshairOutFrac;
	int adsGunKickReducedKickBullets;
	float adsGunKickReducedKickPercent;
	float fAdsGunKickPitchMin;
	float fAdsGunKickPitchMax;
	float fAdsGunKickYawMin;
	float fAdsGunKickYawMax;
	float fAdsGunKickAccel;
	float fAdsGunKickSpeedMax;
	float fAdsGunKickSpeedDecay;
	float fAdsGunKickStaticDecay;
	float fAdsViewKickPitchMin;
	float fAdsViewKickPitchMax;
	float fAdsViewKickYawMin;
	float fAdsViewKickYawMax;
	float fAdsViewKickCenterSpeed;
	float fAdsViewScatterMin;
	float fAdsViewScatterMax;
	float fAdsSpread;
	int hipGunKickReducedKickBullets;
	float hipGunKickReducedKickPercent;
	float fHipGunKickPitchMin;
	float fHipGunKickPitchMax;
	float fHipGunKickYawMin;
	float fHipGunKickYawMax;
	float fHipGunKickAccel;
	float fHipGunKickSpeedMax;
	float fHipGunKickSpeedDecay;
	float fHipGunKickStaticDecay;
	float fHipViewKickPitchMin;
	float fHipViewKickPitchMax;
	float fHipViewKickYawMin;
	float fHipViewKickYawMax;
	float fHipViewKickCenterSpeed;
	float fHipViewScatterMin;
	float fHipViewScatterMax;
	float fightDist;
	float maxDist;
	const char *accuracyGraphName[2];
	int accuracyGraphKnots[2];
	int accuracyGraphKnotCount[2];
	int iPositionReloadTransTime;
	float leftArc;
	float rightArc;
	float topArc;
	float bottomArc;
	float accuracy;
	float aiSpread;
	float playerSpread;
	float minTurnSpeed[2];
	float maxTurnSpeed[2];
	float pitchConvergenceTime;
	float yawConvergenceTime;
	float suppressTime;
	float maxRange;
	float fAnimHorRotateInc;
	float fPlayerPositionDist;
	const char *szUseHintString;
	const char *dropHintString;
	int iUseHintStringIndex;
	int dropHintStringIndex;
	float horizViewJitter;
	float vertViewJitter;
	const char *szScript;
	float fOOPosAnimLength[2];
	int minDamage;
	int minPlayerDamage;
	float fMaxDamageRange;
	float fMinDamageRange;
	float destabilizationBaseTime;
	float destabilizationTimeReductionRatio;
	float destabilizationAngleMax;
	int destabilizeDistance;
	float locationDamageMultipliers[19];
	const char *fireRumble;
	const char *meleeImpactRumble;
} WeaponDef_t;

struct weaponParms
{
	float forward[3];
	float right[3];
	float up[3];
	float muzzleTrace[3];
	float gunForward[3];
	WeaponDef_t* weapDef;
};

struct antilagClientStore
{
	vec3_t realClientPositions[MAX_CLIENTS];
	byte clientMoved[MAX_CLIENTS];
};

typedef enum
{
	AISTATE_RELAXED,
	AISTATE_QUERY,
	AISTATE_ALERT,
	AISTATE_COMBAT,

	MAX_AISTATES
} aistateEnum_t;

typedef enum
{
	ANIM_BP_UNUSED,
	ANIM_BP_LEGS,
	ANIM_BP_TORSO,
	ANIM_BP_BOTH,
	NUM_ANIM_BODYPARTS
} animBodyPart_t;

typedef enum
{
	IT_BAD,
	IT_WEAPON,
	IT_AMMO,
	IT_HEALTH,
	IT_HOLDABLE,
} itemType_t;

typedef struct gitem_s
{
	char *classname;
	char *pickup_sound;
	char *world_model[MAX_ITEM_MODELS];
	char *icon;
	char *display_name;
	int quantity;
	itemType_t giType;
	int giTag;
	int giAmmoIndex;
	int giClipIndex;
} gitem_t;

struct XBoneInfo
{
	vec3_t bounds[2];
	vec3_t offset;
	float radiusSquared;
};

struct XAnimToXModel
{
	int partBits[4];
	byte boneIndex[128];
};

struct XModelCollTri_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
};

struct DSkelPartBits_s
{
	int anim[4];
	int control[4];
	int skel[4];
};

struct DObjAnimMat
{
	vec4_t quat;
	vec3_t trans;
	float transWeight;
};

struct DSkel_t
{
	DSkelPartBits_s partBits;
	DObjAnimMat Mat;
};

struct XModelConfigEntry
{
	char filename[1024];
	float dist;
};

struct XModelConfig
{
	XModelConfigEntry entries[4];
	float mins[3];
	float maxs[3];
	int collLod;
	byte flags;
};

struct XBoneHierarchy
{
	unsigned short *names;
	byte parentList[1];
};

typedef struct XModelParts_s
{
	short numBones;
	short numRootBones;
	XBoneHierarchy *hierarchy;
	short *quats;
	float *trans;
	byte *partClassification;
	DSkel_t skel;
} XModelParts;

typedef struct XModelSurfs_s
{
	struct XSurface *surf;
	int partBits[4];
} XModelSurfs;

typedef struct XModelLodInfo_s
{
	float dist;
	const char *filename;
	short numsurfs;
	unsigned short *surfNames;
	XModelSurfs_s *surfs;
} XModelLodInfo;

typedef struct XModelCollSurf_s
{
	XModelCollTri_s *collTris;
	int numCollTris;
	vec3_t mins;
	vec3_t maxs;
	int boneIdx;
	int contents;
	int surfFlags;
} XModelCollSurf;

typedef struct
{
	XModelParts_s *parts;
	XModelLodInfo_s lodInfo[4];
	XModelCollSurf_s *collSurfs;
	int numCollSurfs;
	int contents;
	XBoneInfo *boneInfo;
	vec3_t mins;
	vec3_t maxs;
	short numLods;
	short collLod;
	struct Material *xskins; // !!! Not loaded in server binary
	int memUsage;
	const char *name;
	char flags;
	bool bad;
} XModel;

struct clientControllers_t
{
	vec3_t angles[6];
	vec3_t tag_origin_angles;
	vec3_t tag_origin_offset;
};

typedef enum
{
	ANIM_MT_UNUSED,
	ANIM_MT_IDLE,
	ANIM_MT_IDLECR,
	ANIM_MT_IDLEPRONE,
	ANIM_MT_WALK,
	ANIM_MT_WALKBK,
	ANIM_MT_WALKCR,
	ANIM_MT_WALKCRBK,
	ANIM_MT_WALKPRONE,
	ANIM_MT_WALKPRONEBK,
	ANIM_MT_RUN,
	ANIM_MT_RUNBK,
	ANIM_MT_RUNCR,
	ANIM_MT_RUNCRBK,
	ANIM_MT_TURNRIGHT,
	ANIM_MT_TURNLEFT,
	ANIM_MT_TURNRIGHTCR,
	ANIM_MT_TURNLEFTCR,
	ANIM_MT_CLIMBUP,
	ANIM_MT_CLIMBDOWN,
	ANIM_MT_MANTLE_ROOT,
	ANIM_MT_MANTLE_UP_57,
	ANIM_MT_MANTLE_UP_51,
	ANIM_MT_MANTLE_UP_45,
	ANIM_MT_MANTLE_UP_39,
	ANIM_MT_MANTLE_UP_33,
	ANIM_MT_MANTLE_UP_27,
	ANIM_MT_MANTLE_UP_21,
	ANIM_MT_MANTLE_OVER_HIGH,
	ANIM_MT_MANTLE_OVER_MID,
	ANIM_MT_MANTLE_OVER_LOW,
	ANIM_MT_FLINCH_FORWARD,
	ANIM_MT_FLINCH_BACKWARD,
	ANIM_MT_FLINCH_LEFT,
	ANIM_MT_FLINCH_RIGHT,
	ANIM_MT_STUMBLE_FORWARD,
	ANIM_MT_STUMBLE_BACKWARD,
	ANIM_MT_STUMBLE_WALK_FORWARD,
	ANIM_MT_STUMBLE_WALK_BACKWARD,
	ANIM_MT_STUMBLE_CROUCH_FORWARD,
	ANIM_MT_STUMBLE_CROUCH_BACKWARD,

	NUM_ANIM_MOVETYPES
} scriptAnimMoveTypes_t;

typedef struct animation_s
{
	char name[64];
	int initialLerp;
	float moveSpeed;
	int duration;
	int nameHash;
	int flags;
	scriptAnimMoveTypes_t movetype; // 64 bit enum?
	int pad;
	int noteType;
} animation_t;

typedef struct
{
	int index;
	int value[2];
} animScriptCondition_t;

typedef struct
{
	short bodyPart[2];
	short animIndex[2];
	unsigned short animDuration[2];
	snd_alias_list_t *soundAlias;
} animScriptCommand_t;

typedef enum
{
	ANIM_ET_PAIN,
	ANIM_ET_DEATH,
	ANIM_ET_FIREWEAPON,
	ANIM_ET_JUMP,
	ANIM_ET_JUMPBK,
	ANIM_ET_LAND,
	ANIM_ET_DROPWEAPON,
	ANIM_ET_RAISEWEAPON,
	ANIM_ET_CLIMBMOUNT,
	ANIM_ET_CLIMBDISMOUNT,
	ANIM_ET_RELOAD,
	ANIM_ET_CROUCH_TO_PRONE,
	ANIM_ET_PRONE_TO_CROUCH,
	ANIM_ET_STAND_TO_CROUCH,
	ANIM_ET_CROUCH_TO_STAND,
	ANIM_ET_STAND_TO_PRONE,
	ANIM_ET_PRONE_TO_STAND,
	ANIM_ET_MELEEATTACK,
	ANIM_ET_SHELLSHOCK,

	NUM_ANIM_EVENTTYPES // 19
} scriptAnimEventTypes_t;

typedef enum
{
	ANIM_COND_PLAYERANIMTYPE,
	ANIM_COND_WEAPONCLASS,
	ANIM_COND_MOUNTED,
	ANIM_COND_MOVETYPE,
	ANIM_COND_UNDERHAND,
	ANIM_COND_CROUCHING,
	ANIM_COND_FIRING,
	ANIM_COND_WEAPON_POSITION,
	ANIM_COND_STRAFING,

	NUM_ANIM_CONDITIONS // 9
} scriptAnimConditions_t;

typedef struct
{
	int numConditions;
	animScriptCondition_t conditions[NUM_ANIM_CONDITIONS];
	int numCommands;
	animScriptCommand_t commands[MAX_ANIMSCRIPT_ANIMCOMMANDS];
} animScriptItem_t;

typedef struct
{
	int numItems;
	animScriptItem_t *items[128];
} animScript_t;

struct scr_animtree_t
{
	struct XAnim_s *anims;
};

typedef struct animScriptData_s
{
	animation_s animations[MAX_MODEL_ANIMATIONS];
	int numAnimations;
	animScript_t scriptAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptCannedAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptStateChange[MAX_AISTATES][MAX_AISTATES];
	animScript_t scriptEvents[NUM_ANIM_EVENTTYPES];
	animScriptItem_t scriptItems[MAX_ANIMSCRIPT_ITEMS_PER_MODEL];
	int numScriptItems;
	scr_animtree_t animTree;
	unsigned short torsoAnim;
	unsigned short legsAnim;
	unsigned short turningAnim;
	unsigned short rootAnim;
	snd_alias_list_t *(*soundAlias)(const char *);
	void (*playSoundAlias)(int, snd_alias_list_t *);
} animScriptData_t;

typedef struct lerpFrame_s
{
	float yawAngle;
	int yawing;
	float pitchAngle;
	int pitching;
	int animationNumber;
	animation_t *animation;
	int animationTime;
	vec3_t oldFramePos;
	float animSpeedScale;
	int oldFrameSnapshotTime;
} lerpFrame_t;

typedef struct
{
	uint16_t flags;
	uint16_t children;
} XAnimParent;

typedef struct
{
	uint16_t numAnims;
	uint16_t parent;
	union
	{
		int parts;
		XAnimParent *s;
	};
} XAnimEntry;

typedef struct XAnim_s
{
	const char *debugName;
	unsigned int size;
	const char **debugAnimNames;
	XAnimEntry entries;
} XAnim_t;

typedef struct XAnimTree_s
{
	XAnim_s *anims;
	uint16_t entnum;
	bool bAbs;
	bool bUseGoalWeight;
	uint16_t infoArray[1];
} XAnimTree;

typedef struct clientInfo_s
{
	int infoValid;
	int nextValid;
	int clientNum;
	char name[32];
	int team;
	int oldteam;
	int score;
	int location;
	int health;
	char model[64];
	char attachModelNames[6][64];
	char attachTagNames[6][64];
	lerpFrame_t legs;
	lerpFrame_t torso;
	float lerpMoveDir;
	float lerpLean;
	vec3_t playerAngles;
	int leftHandGun;
	int dobjDirty;
	clientControllers_t control;
	int clientConditions[NUM_ANIM_CONDITIONS][2];
	XAnimTree_s *pXAnimTree;
	int iDObjWeapon;
	int stanceTransitionTime;
	int turnAnimEndTime;
	char turnAnimType;
} clientInfo_t;

struct corpseInfo_t
{
	XAnimTree_s *tree;
	int entnum;
	int time;
	clientInfo_t ci;
	byte falling;
};

struct DObjModel_s
{
	XModel *model;
	const char *boneName;
	qboolean ignoreCollision;
};

typedef struct DObj_s
{
	XAnimTree_s *tree;
	DSkel_t *skel;
	int timeStamp;
	unsigned short *animToModel;
	unsigned short duplicateParts;
	int locked;
	byte numModels;
	byte numBones;
	byte ignoreCollision;
	XModel *models[8];
	byte modelParents[8];
	byte matOffset[8];
	vec3_t mins;
	vec3_t maxs;
} DObj;

typedef struct scr_anim_s
{
	union
	{
		struct
		{
			uint16_t index;
			uint16_t tree;
		};
		const char *linkPointer;
	};
} scr_anim_t;

struct bgsAnim_t
{
	animScriptData_t animScriptData;
	struct
	{
		scr_animtree_t tree;
		scr_anim_t     root;
		scr_anim_t     torso;
		scr_anim_t     legs;
		scr_anim_t     turning;
	} generic_human;
};

typedef struct __attribute__((aligned(8))) bgs_s
{
	bgsAnim_t animData;
	int time;
	int latestSnapshotTime;
	int frametime;
	int anim_user;
	XModel *(*GetXModel)(const char *);
	void (*CreateDObj)(DObjModel_s *, unsigned short, XAnimTree_s *, int);
	void (*SafeDObjFree)(int);
	void *(*AllocXAnim)(int);
	clientInfo_t clientinfo[64];
} bgs_t;

struct pmove_t
{
	playerState_t *ps;
	usercmd_t cmd;
	usercmd_t oldcmd;
	int tracemask;
	int numtouch;
	int touchents[32];
	vec3_t mins;
	vec3_t maxs;
	float xyspeed;
	int proneChange;
	byte mantleStarted; // 229
	vec3_t mantleEndPos;
	int mantleDuration;
};

struct pml_t
{
	vec3_t forward;
	vec3_t right;
	vec3_t up;
	float frametime;
	int msec;
	int walking;
	int groundPlane;
	int almostGroundPlane;
	trace_t groundTrace;
	float impactSpeed;
	vec3_t previous_origin;
	vec3_t previous_velocity;
};

typedef enum
{
	WEAPON_READY = 0x0,
	WEAPON_RAISING = 0x1,
	WEAPON_DROPPING = 0x2,
	WEAPON_FIRING = 0x3,
	WEAPON_RECHAMBERING = 0x4,
	WEAPON_RELOADING = 0x5,
	WEAPON_RELOADING_INTERUPT = 0x6,
	WEAPON_RELOAD_START = 0x7,
	WEAPON_RELOAD_START_INTERUPT = 0x8,
	WEAPON_RELOAD_END = 0x9,
	WEAPON_MELEE_INIT = 0xA,
	WEAPON_MELEE_FIRE = 0xB,
	WEAPON_OFFHAND_INIT = 0xC,
	WEAPON_OFFHAND_PREPARE = 0xD,
	WEAPON_OFFHAND_HOLD = 0xE,
	WEAPON_OFFHAND = 0xF,
	WEAPON_OFFHAND_END = 0x10,
	WEAPON_BINOCULARS_INIT = 0x11,
	WEAPON_BINOCULARS_PREPARE = 0x12,
	WEAPON_BINOCULARS_HOLD = 0x13,
	WEAPON_BINOCULARS_START = 0x14,
	WEAPON_BINOCULARS_DROP = 0x15,
	WEAPON_BINOCULARS_END = 0x16,
	WEAPONSTATES_NUM = 0x17,
} weaponstate_t;

typedef struct
{
	unsigned short emptystring;
	unsigned short allies;
	unsigned short axis;
	unsigned short current;
	unsigned short damage;
	unsigned short death;
	unsigned short dlight;
	unsigned short done;
	unsigned short empty;
	unsigned short entity;
	unsigned short failed;
	unsigned short fraction;
	unsigned short goal;
	unsigned short grenade;
	unsigned short info_notnull;
	unsigned short invisible;
	unsigned short key1;
	unsigned short key2;
	unsigned short killanimscript;
	unsigned short left;
	unsigned short movedone;
	unsigned short noclass;
	unsigned short normal;
	unsigned short pistol;
	unsigned short plane_waypoint;
	unsigned short player;
	unsigned short position;
	unsigned short primary;
	unsigned short primaryb;
	unsigned short prone;
	unsigned short right;
	unsigned short rocket;
	unsigned short rotatedone;
	unsigned short script_brushmodel;
	unsigned short script_model;
	unsigned short script_origin;
	unsigned short spectator;
	unsigned short stand;
	unsigned short surfacetype;
	unsigned short target_script_trigger;
	unsigned short tempEntity;
	unsigned short touch;
	unsigned short trigger;
	unsigned short trigger_use;
	unsigned short trigger_use_touch;
	unsigned short trigger_damage;
	unsigned short trigger_lookat;
	unsigned short truck_cam;
	unsigned short worldspawn;
	unsigned short binocular_enter;
	unsigned short binocular_exit;
	unsigned short binocular_fire;
	unsigned short binocular_release;
	unsigned short binocular_drop;
	unsigned short begin;
	unsigned short intermission;
	unsigned short menuresponse;
	unsigned short playing;
	unsigned short none;
	unsigned short dead;
	unsigned short auto_change;
	unsigned short manual_change;
	unsigned short freelook;
	unsigned short call_vote;
	unsigned short vote;
	unsigned short snd_enveffectsprio_level;
	unsigned short snd_enveffectsprio_shellshock;
	unsigned short snd_channelvolprio_holdbreath;
	unsigned short snd_channelvolprio_pain;
	unsigned short snd_channelvolprio_shellshock;
	unsigned short tag_flash;
	unsigned short tag_flash_11;
	unsigned short tag_flash_2;
	unsigned short tag_flash_22;
	unsigned short tag_brass;
	unsigned short j_head;
	unsigned short tag_weapon;
	unsigned short tag_player;
	unsigned short tag_camera;
	unsigned short tag_aim;
	unsigned short tag_aim_animated;
	unsigned short tag_origin;
	unsigned short tag_butt;
	unsigned short tag_weapon_right;
	unsigned short back_low;
	unsigned short back_mid;
	unsigned short back_up;
	unsigned short neck;
	unsigned short head;
	unsigned short pelvis;
} stringIndex_t;

typedef struct
{
	char material[64];
	int surfaceFlags;
	int contentFlags;
} dmaterial_t;

typedef struct cStaticModel_s
{
	uint16_t writable;
	XModel *xmodel;
	vec3_t origin;
	vec3_t invScaledAxis[3];
	vec3_t absmin;
	vec3_t absmax;
} cStaticModel_t;

typedef struct cplane_s
{
	vec3_t normal;
	float dist;
	byte type;
	byte signbits;
	byte pad[2];
} cplane_t;

typedef struct cbrushside_s
{
	cplane_t *plane;
	unsigned int materialNum;
} cbrushside_t;

typedef struct
{
	cplane_t *plane;
	int16_t children[2];
} cNode_t;

typedef struct cLeaf_s
{
	uint16_t firstCollAabbIndex;
	uint16_t collAabbCount;
	int brushContents;
	int terrainContents;
	vec3_t mins;
	vec3_t maxs;
	int leafBrushNode;
	int16_t cluster;
	int16_t pad;
} cLeaf_t;

typedef struct
{
	uint16_t *brushes;
} cLeafBrushNodeLeaf_t;

typedef struct
{
	float dist;
	float range;
	uint16_t childOffset[2];
} cLeafBrushNodeChildren_t;

typedef union
{
	cLeafBrushNodeLeaf_t leaf;
	cLeafBrushNodeChildren_t children;
} cLeafBrushNodeData_t;

#pragma pack(push, 2)
typedef struct cLeafBrushNode_s
{
	byte axis;
	uint16_t leafBrushCount;
	int contents;
	cLeafBrushNodeData_t data;
} cLeafBrushNode_t;
#pragma pack(pop)

typedef struct CollisionBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
} CollisionBorder_t;

typedef struct CollisionPartition
{
	char triCount;
	char borderCount;
	int firstTri;
	CollisionBorder_t *borders;
} CollisionPartition_t;

typedef union
{
	int firstChildIndex;
	int partitionIndex;
} CollisionAabbTreeIndex_t;

typedef struct CollisionAabbTree_s
{
	float origin[3];
	float halfSize[3];
	uint16_t materialIndex;
	uint16_t childCount;
	CollisionAabbTreeIndex_t u;
} CollisionAabbTree_t;

typedef struct cmodel_s
{
	vec3_t mins;
	vec3_t maxs;
	float radius;
	cLeaf_t leaf;
} cmodel_t;

typedef struct __attribute__((aligned(16))) cbrush_s
{
	float mins[3];
	int contents;
	float maxs[3];
	unsigned int numsides;
	cbrushside_t *sides;
	int16_t axialMaterialNum[2][3];
} cbrush_t;

typedef struct
{
	float position[3];
	float normal[3][3];
}
CollisionEdge_t;

typedef struct
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
	unsigned int verts[3];
	unsigned int edges[3];
} CollisionTriangle_t;

typedef void DynEntityDef;
typedef void DynEntityPose;
typedef void DynEntityClient;
typedef void DynEntityColl;

typedef struct clipMap_s
{
	const char *name;
	unsigned int numStaticModels;
	cStaticModel_t *staticModelList;
	unsigned int numMaterials;
	dmaterial_t *materials;
	unsigned int numBrushSides;
	cbrushside_t *brushsides;
	unsigned int numNodes;
	cNode_t *nodes;
	unsigned int numLeafs;
	cLeaf_t *leafs;
	unsigned int leafbrushNodesCount;
	cLeafBrushNode_t *leafbrushNodes;
	unsigned int numLeafBrushes;
	uint16_t *leafbrushes;
	unsigned int numLeafSurfaces;
	unsigned int *leafsurfaces;
	unsigned int vertCount;
	float (*verts)[3];
	unsigned int edgeCount;
	CollisionEdge_t *edges;
	int triCount;
	CollisionTriangle_t *triIndices;
	int borderCount;
	CollisionBorder_t *borders;
	int partitionCount;
	CollisionPartition_t *partitions;
	int aabbTreeCount;
	CollisionAabbTree_t *aabbTrees;
	unsigned int numSubModels;
	cmodel_t *cmodels;
	uint16_t numBrushes;
	cbrush_t *brushes;
	int numClusters;
	int clusterBytes;
	byte *visibility;
	int vised;
	int numEntityChars;
	char *entityString;
	cbrush_t *box_brush;
	cmodel_t box_model;
	uint16_t dynEntCount[2];
	DynEntityDef *dynEntDefList[2];
	DynEntityPose *dynEntPoseList[2];
	DynEntityClient *dynEntClientList[2];
	DynEntityColl *dynEntCollList[2];
	unsigned int checksum;
} clipMap_t; // verified

enum LumpType
{
	LUMP_MATERIALS = 0,
	LUMP_LIGHTBYTES = 1,
	LUMP_LIGHTGRIDENTRIES = 2,
	LUMP_LIGHTGRIDCOLORS = 3,
	LUMP_PLANES = 4,
	LUMP_BRUSHSIDES = 5,
	LUMP_BRUSHES = 6,
	LUMP_TRIANGLES = 7,
	LUMP_DRAWVERTS = 8,
	LUMP_DRAWINDICES = 9,
	LUMP_CULLGROUPS = 10,
	LUMP_CULLGROUPINDICES = 11,
	LUMP_OBSOLETE_1 = 12,
	LUMP_OBSOLETE_2 = 13,
	LUMP_OBSOLETE_3 = 14,
	LUMP_OBSOLETE_4 = 15,
	LUMP_OBSOLETE_5 = 16,
	LUMP_PORTALVERTS = 17,
	LUMP_OCCLUDER = 18,
	LUMP_OCCLUDERPLANES = 19,
	LUMP_OCCLUDEREDGES = 20,
	LUMP_OCCLUDERINDICES = 21,
	LUMP_AABBTREES = 22,
	LUMP_CELLS = 23,
	LUMP_PORTALS = 24,
	LUMP_NODES = 25,
	LUMP_LEAFS = 26,
	LUMP_LEAFBRUSHES = 27,
	LUMP_LEAFSURFACES = 28,
	LUMP_COLLISIONVERTS = 29,
	LUMP_COLLISIONEDGES = 30,
	LUMP_COLLISIONTRIS = 31,
	LUMP_COLLISIONBORDERS = 32,
	LUMP_COLLISIONPARTITIONS = 33,
	LUMP_COLLISIONAABBS = 34,
	LUMP_MODELS = 35,
	LUMP_VISIBILITY = 36,
	LUMP_ENTITIES = 37,
	LUMP_PATHCONNECTIONS = 38,
	LUMP_PRIMARY_LIGHTS = 39,
};

struct BspChunk
{
	enum LumpType type;
	unsigned int length;
};

typedef struct BspHeader
{
	unsigned int ident;
	unsigned int version;
	unsigned int chunkCount;
	struct BspChunk chunks[100];
} BspHeader;

typedef struct comBspGlob_t
{
	char name[64];
	BspHeader *header;
	unsigned int fileSize;
	unsigned int checksum;
	enum LumpType loadedLumpType;
	const void *loadedLumpData;
} comBspGlob_t;

struct va_info_t
{
	char va_string[MAX_VASTRINGS][1024];
	int index;
};

typedef struct gameTypeScript_s
{
	char pscScript[64];
	char pszName[64];
	int bTeamBased;
} gameTypeScript_t;

typedef struct scr_gametype_data_s
{
	int main;
	int startupgametype;
	int playerconnect;
	int playerdisconnect;
	int playerdamage;
	int playerkilled;
	int votecalled;
	int playervote;
	int iNumGameTypes;
	gameTypeScript_t list[32];
} scr_gametype_data_t;

typedef struct scr_data_s
{
	int levelscript;
	int gametypescript;
	scr_gametype_data_t gametype;
	int delete_;
	int initstructs;
	int createstruct;
	corpseInfo_t playerCorpseInfo[8];
} scr_data_t;

typedef struct areaParms_s
{
	const float *start;
	const float *mins;
	const float *maxs;
	int *list;
	int count;
	int maxcount;
	int contentmask;
	int results; // probably not int
} areaParms_t;

typedef struct worldContents_s
{
	int contentsStaticModels;
	int contentsEntities;
	uint16_t entities;
	uint16_t staticModels;
} worldContents_t;

typedef struct __attribute__((packed, aligned(4))) worldTree_s
{
	float dist;
	uint16_t axis;
	union
	{
		uint16_t parent;
		uint16_t nextFree;
	};
	uint16_t child[2];
} worldTree_t;

typedef struct worldSector_s
{
	worldContents_t contents;
	worldTree_t tree;
} worldSector_t;

typedef struct cm_world_s
{
	float mins[3];
	float maxs[3];
	uint16_t freeHead;
	uint16_t gap;
	worldSector_t sectors[1024];
} cm_world_t;

#if COD_VERSION == COD2_1_0
static const int gentities_offset = 0x08665480;
#elif COD_VERSION == COD2_1_2
static const int gentities_offset = 0x08679380;
#elif COD_VERSION == COD2_1_3
static const int gentities_offset = 0x08716400;
#endif

#if COD_VERSION == COD2_1_0
static const int gclients_offset = 0x086F1480;
#elif COD_VERSION == COD2_1_2
static const int gclients_offset = 0x08705480;
#elif COD_VERSION == COD2_1_3
static const int gclients_offset = 0x087A2500;
#endif

#if COD_VERSION == COD2_1_0
static const int varpub_offset = 0x08394000;
#elif COD_VERSION == COD2_1_2
static const int varpub_offset = 0x08396480;
#elif COD_VERSION == COD2_1_3
static const int varpub_offset = 0x08397500;
#endif

#if COD_VERSION == COD2_1_0
static const int varglob_offset = 0x08294000;
#elif COD_VERSION == COD2_1_2
static const int varglob_offset = 0x08296480;
#elif COD_VERSION == COD2_1_3
static const int varglob_offset = 0x08297500;
#endif

#if COD_VERSION == COD2_1_0
static const int stringglob_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int stringglob_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int stringglob_offset = 0x08287400;
#endif

#if COD_VERSION == COD2_1_0
static const int vmpub_offset = 0x083D7600;
#elif COD_VERSION == COD2_1_2
static const int vmpub_offset = 0x083D7A00;
#elif COD_VERSION == COD2_1_3
static const int vmpub_offset = 0x083D8A80;
#endif

#if COD_VERSION == COD2_1_0
static const int compilepub_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int compilepub_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int compilepub_offset = 0x08205C80;
#endif

#if COD_VERSION == COD2_1_0
static const int vmglob_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int vmglob_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int vmglob_offset = 0x083DCE80;
#endif

#if COD_VERSION == COD2_1_0
static const int g_script_error_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int g_script_error_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int g_script_error_offset = 0x083D7620;
#endif

#if COD_VERSION == COD2_1_0
static const int g_script_error_level_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int g_script_error_level_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int g_script_error_level_offset = 0x083D8A3C;
#endif

#if COD_VERSION == COD2_1_0
static const int sv_offset = 0x0842BC80;
#elif COD_VERSION == COD2_1_2
static const int sv_offset = 0x0843F180;
#elif COD_VERSION == COD2_1_3
static const int sv_offset = 0x08440200;
#endif

#if COD_VERSION == COD2_1_0
static const int svs_offset = 0x0841FB00;
#elif COD_VERSION == COD2_1_2
static const int svs_offset = 0x08422000;
#elif COD_VERSION == COD2_1_3
static const int svs_offset = 0x08423080;
#endif

#if COD_VERSION == COD2_1_0
static const int level_offset = 0x0859B400;
#elif COD_VERSION == COD2_1_2
static const int level_offset = 0x085AF300;
#elif COD_VERSION == COD2_1_3
static const int level_offset = 0x0864C380;
#endif

#if COD_VERSION == COD2_1_0
static const int const_offset = 0x087A22A0;
#elif COD_VERSION == COD2_1_2
static const int const_offset = 0x087B61A0;
#elif COD_VERSION == COD2_1_3
static const int const_offset = 0x08853220;
#endif

#if COD_VERSION == COD2_1_0
static const int bgs_offset = 0x0855A4E0;
#elif COD_VERSION == COD2_1_2
static const int bgs_offset = 0x0856E3A0;
#elif COD_VERSION == COD2_1_3
static const int bgs_offset = 0x0860B420;
#endif

#if COD_VERSION == COD2_1_0
static const int level_bgs_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int level_bgs_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int level_bgs_offset = 0x0864F9C0;
#endif

#if COD_VERSION == COD2_1_0
static const int cm_offset = 0x08185BE0;
#elif COD_VERSION == COD2_1_2
static const int cm_offset = 0x08187D40;
#elif COD_VERSION == COD2_1_3
static const int cm_offset = 0x08188DC0;
#endif

#if COD_VERSION == COD2_1_0
static const int bspglob_offset = 0x08185BC8;
#elif COD_VERSION == COD2_1_2
static const int bspglob_offset = 0x08187D28;
#elif COD_VERSION == COD2_1_3
static const int bspglob_offset = 0x08188DA8;
#endif

#if COD_VERSION == COD2_1_0
static const int playerStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int playerStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int playerStateFields_offset = 0x08142520;
#endif

#if COD_VERSION == COD2_1_0
static const int entityStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int entityStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int entityStateFields_offset = 0x081417a0;
#endif

#if COD_VERSION == COD2_1_0
static const int objectiveFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int objectiveFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int objectiveFields_offset = 0x08142a20;
#endif

#if COD_VERSION == COD2_1_0
static const int clientStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int clientStateFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int clientStateFields_offset = 0x08141F60;
#endif

#if COD_VERSION == COD2_1_0
static const int archivedEntityFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int archivedEntityFields_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int archivedEntityFields_offset = 0x08141AE0;
#endif

#if COD_VERSION == COD2_1_0
static const int bg_itemlist_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const int bg_itemlist_offset = 0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const int bg_itemlist_offset = 0x081850C0;
#endif

#if COD_VERSION == COD2_1_0
static const int testclient_connect_string_offset = 0x0;  // Not tested
#elif COD_VERSION == COD2_1_2
static const int testclient_connect_string_offset = 0x0;  // Not tested
#elif COD_VERSION == COD2_1_3
static const int testclient_connect_string_offset = 0x0814ab20;
#endif

#if COD_VERSION == COD2_1_0
static const int g_hudelems_offset = 0x0;  // Not tested
#elif COD_VERSION == COD2_1_2
static const int g_hudelems_offset = 0x0;  // Not tested
#elif COD_VERSION == COD2_1_3
static const int g_hudelems_offset = 0x08628F80;
#endif

#if COD_VERSION == COD2_1_0  // Not tested
static const int spawns_offset = 0x0;
#elif COD_VERSION == COD2_1_2  // Not tested
static const int spawns_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int spawns_offset = 0x08188A00;
#endif

#if COD_VERSION == COD2_1_0  // Not tested
static const int entity_event_names_offset = 0x0;
#elif COD_VERSION == COD2_1_2  // Not tested
static const int entity_event_names_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int entity_event_names_offset = 0x08184DA0;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int logfile_offset = 0x0;
static const int rd_buffer_offset = 0x0;
static const int rd_buffersize_offset = 0x0;
static const int rd_flush_offset = 0x0;
static const int opening_qconsole_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int logfile_offset = 0x0;
static const int rd_buffer_offset = 0x0;
static const int rd_buffersize_offset = 0x0;
static const int rd_flush_offset = 0x0;
static const int opening_qconsole_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int logfile_offset = 0x081A2268;
static const int rd_buffer_offset = 0x081A4F90;
static const int rd_buffersize_offset = 0x081A4F94;
static const int rd_flush_offset = 0x081A4F98;
static const int opening_qconsole_offset = 0x081A4F9C;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int com_errorEntered_offset = 0x0;
static const int com_fixedConsolePosition_offset = 0x0;
static const int com_errorType_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int com_errorEntered_offset = 0x0;
static const int com_fixedConsolePosition_offset = 0x0;
static const int com_errorType_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int com_errorEntered_offset = 0x081A21C0;
static const int com_fixedConsolePosition_offset = 0x081A21C4;
static const int com_errorType_offset = 0x081A2264;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int legacyHacks_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int legacyHacks_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int legacyHacks_offset = 0x0817D838;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int vec3_origin_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int vec3_origin_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int vec3_origin_offset = 0x0814CFC8;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int g_scr_data_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int g_scr_data_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int g_scr_data_offset = 0x0884D700;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int actorLocationalMins_offset = 0x0;
static const int actorLocationalMaxs_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int actorLocationalMins_offset = 0x0;
static const int actorLocationalMaxs_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int actorLocationalMins_offset = 0x0817DB88;
static const int actorLocationalMaxs_offset = 0x0817DB94;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int cm_world_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int cm_world_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int cm_world_offset = 0x08189000;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int bloc_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int bloc_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int bloc_offset = 0x081E9544;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int sv_serverId_value_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int sv_serverId_value_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int sv_serverId_value_offset = 0x08423008;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int msgHuff_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int msgHuff_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int msgHuff_offset = 0x081E9C60;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int cached_models_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int cached_models_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int cached_models_offset = 0x08852E20;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int rcon_lasttime_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int rcon_lasttime_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int rcon_lasttime_offset = 0x0849FBF4;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int com_frameTime_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int com_frameTime_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int com_frameTime_offset = 0x081A21B8;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int bulletPriorityMap_offset = 0x0;
static const int riflePriorityMap_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int bulletPriorityMap_offset = 0x0;
static const int riflePriorityMap_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int bulletPriorityMap_offset = 0x08187C0C;
static const int riflePriorityMap_offset = 0x08187C1F;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int fs_searchpaths_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int fs_searchpaths_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int fs_searchpaths_offset = 0x0849FD68;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int g_EndPos_offset = 0x0;
static const int scrParserPub_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int g_EndPos_offset = 0x0;
static const int scrParserPub_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int g_EndPos_offset = 0x083D7600;
static const int scrParserPub_offset = 0x08287334;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int saLoadObjGlob_offset = 0x0;
static const int saLoadedObjs_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int saLoadObjGlob_offset = 0x0;
static const int saLoadedObjs_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int saLoadObjGlob_offset = 0x085AB160;
static const int saLoadedObjs_offset = 0x085AB164;
#endif

#if COD_VERSION == COD2_1_0 // Not tested
static const int gameInitialized_offset = 0x0;
#elif COD_VERSION == COD2_1_2 // Not tested
static const int gameInitialized_offset = 0x0;
#elif COD_VERSION == COD2_1_3
static const int gameInitialized_offset = 0x083E2F80;
#endif

#define g_entities ((gentity_t*)(gentities_offset))
#define g_clients ((gclient_t*)(gclients_offset))
#define scrVarPub (*((scrVarPub_t*)( varpub_offset )))
#define scrVmPub (*((scrVmPub_t*)( vmpub_offset )))
#define scrVarGlob (((VariableValueInternal*)( varglob_offset )))
#define scrVarGlob_high ((VariableValueInternal*)( varglob_offset + 16 * 32770 ))
#define scrStringGlob (*((scrStringGlob_t*)( stringglob_offset )))
#define scrCompilePub (*((scrCompilePub_t*)( compilepub_offset )))
#define scrVmGlob (*((scrVmGlob_t*)( vmglob_offset )))
#define scrParserPub (*((scrParserPub_t*)( scrParserPub_offset )))
#define g_script_error (*((jmp_buf*)( g_script_error_offset )))
#define g_script_error_level (*((int*)( g_script_error_level_offset )))
#define sv (*((server_t*)( sv_offset )))
#define svs (*((serverStatic_t*)( svs_offset )))
#define level (*((level_locals_t*)( level_offset )))
#define scr_const (*((stringIndex_t*)( const_offset )))
#define bgs (*((bgs_t**)( bgs_offset )))
#define level_bgs (*((bgs_t*)( level_bgs_offset )))
#define cm (*((clipMap_t*)( cm_offset )))
#define comBspGlob (*((comBspGlob_t*)( bspglob_offset )))
#define playerStateFields (*((netField_t*)( playerStateFields_offset )))
#define entityStateFields (*((netField_t*)( entityStateFields_offset )))
#define objectiveFields (*((netField_t*)( objectiveFields_offset )))
#define clientStateFields (*((netField_t*)( clientStateFields_offset )))
#define archivedEntityFields (*((netField_t*)( archivedEntityFields_offset )))
#define bg_itemlist (*((gitem_t*)( bg_itemlist_offset )))
#define g_hudelems (*((game_hudelem_t*)( g_hudelems_offset )))
#define spawns (*((int**)( spawns_offset )))
#define entity_event_names (*((const char**)( entity_event_names_offset )))
#define logfile (*((fileHandle_t*)( logfile_offset )))
#define rd_buffer (*((char**)( rd_buffer_offset )))
#define rd_buffersize (*((int*)( rd_buffersize_offset )))
#define rd_flush (*((void(**)(char*))( rd_flush_offset )))
#define opening_qconsole (*((int*)( opening_qconsole_offset )))
#define com_errorEntered (*((int*)( com_errorEntered_offset )))
#define com_fixedConsolePosition (*((int*)( com_fixedConsolePosition_offset )))
#define com_errorType (*((int*)( com_errorType_offset )))
#define testclient_connect_string (*((char*)( testclient_connect_string_offset )))
#define legacyHacks ((int*)( legacyHacks_offset ))
#define vec3_origin ((float*)( vec3_origin_offset ))
#define g_scr_data (*((scr_data_t*)( g_scr_data_offset )))
#define actorLocationalMins ((float*)( actorLocationalMins_offset ))
#define actorLocationalMaxs ((float*)( actorLocationalMaxs_offset ))
#define cm_world (*((cm_world_t*)(cm_world_offset)))
#define bloc (*((int*)( bloc_offset )))
#define sv_serverId_value (*((int*)( sv_serverId_value_offset )))
#define msgHuff (*((huffman_t*)( msgHuff_offset )))
#define cached_models (((XModel**)( cached_models_offset )))
#define rcon_lasttime (*((int*)( rcon_lasttime_offset )))
#define com_frameTime (*((int*)( com_frameTime_offset )))
#define bulletPriorityMap (*((uint8_t*)( bulletPriorityMap_offset )))
#define riflePriorityMap (*((uint8_t*)( riflePriorityMap_offset )))
#define fs_searchpaths (*((searchpath_t**)( fs_searchpaths_offset )))
#define g_EndPos (*((char*)( g_EndPos_offset )))
#define saLoadObjGlob (*((snd_alias_build_s**)( saLoadObjGlob_offset )))
#define saLoadedObjs (*((int*)( saLoadedObjs_offset ))) // Guessed variable name
#define gameInitialized (*((int*)( gameInitialized_offset )))

// Check for critical structure sizes and fail if not match
#if __GNUC__ >= 6

#if COD_VERSION == COD2_1_0

 static_assert((sizeof(client_t) == 0x78F14), "ERROR: client_t size is invalid!");
#elif COD_VERSION == COD2_1_2
 static_assert((sizeof(client_t) == 0x79064), "ERROR: client_t size is invalid!");
#elif COD_VERSION == COD2_1_3
 static_assert((sizeof(client_t) == 0xB1064), "ERROR: client_t size is invalid!");
 
#endif
 
 static_assert((sizeof(gentity_t) == 560), "ERROR: gentity_t size is invalid!");
 static_assert((sizeof(gclient_t) == 0x28A4), "ERROR: gclient_t size is invalid!");
 static_assert((sizeof(clientState_t) == 0x5c), "ERROR: clientState_t size is invalid!");
 static_assert((sizeof(gitem_t) == 44), "ERROR: gitem_t size is invalid!");
 static_assert((sizeof(XModel) == 144), "ERROR: XModel size is invalid!");
 static_assert((sizeof(scrStringGlob_t) == 65544), "ERROR: scrStringGlob_t size is invalid!");
 static_assert((sizeof(worldContents_s) == 12), "ERROR: worldContents_s size is invalid!");
 static_assert((sizeof(worldTree_t) == 12), "ERROR: worldTree_t size is invalid!");
 static_assert((sizeof(worldSector_t) == 24), "ERROR: worldSector_t size is invalid!");
 static_assert((sizeof(clientInfo_t) == 1208), "ERROR: clientInfo_t size is invalid!");
 static_assert((sizeof(animation_t) == 96), "ERROR: animation_t size is invalid!");
 static_assert((sizeof(scr_data_t) == 14072), "ERROR: scr_data_t size is invalid!");
 static_assert((sizeof(entHandlers_t) == 1), "ERROR: entHandlers_t size is invalid!");

#endif

#endif

// Custom data types

#define MAX_ERROR_BUFFER 64

typedef struct src_error_s
{
	char internal_function[64];
	char message[1024];
} scr_error_t;

typedef struct map_weapon_s
{
	char classname[MAX_STRINGLENGTH]; // Not sure if a classname can actually be that long
	vec3_t origin;
	vec3_t angles;
	int count;
} map_weapon_t;

typedef struct map_turret_s
{
	char classname[MAX_STRINGLENGTH]; // Not sure if a classname can actually be that long
	vec3_t origin;
	vec3_t angles;
	int toparc;
	int bottomarc;
	int leftarc;
	int rightarc;
	char model[MAX_OSPATH];
	char weaponinfo[MAX_OSPATH];
	int script_export;
	char script_gameobjectname[MAX_QPATH];
} map_turret_t;

#define MAX_NOTIFY_DEBUG_BUFFER 4096 // Triggers cause a lot of notify (we might want to filter these to avoid lag)
#define MAX_NOTIFY_DEBUG_PARAMS 16 // The script engine can handle hundreds of parameters, here we set a limit for CodeCallback_NotifyDebug

union SavedVariableUnion
{
	int intValue;
	float floatValue;
	char stringValue[MAX_STRINGLENGTH];
	vec3_t vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
};

typedef struct
{
	union SavedVariableUnion u;
	int type;
} SavedVariableValue;

typedef struct scr_notify_s
{
	unsigned int entId;
	char message[MAX_STRINGLENGTH];
	unsigned int argc;
	SavedVariableValue arguments[MAX_NOTIFY_DEBUG_PARAMS];
} scr_notify_t;

#if COMPILE_CUSTOM_VOICE == 1

#define MAX_CUSTOMSOUNDDURATION 10 // Minutes
#define MAX_STOREDVOICEPACKETS (MAX_CUSTOMSOUNDDURATION * 3072) // MAX_VOICEPACKETSPERFRAME * 20 * 60
#define MAX_CUSTOMSOUNDS 64 // Consider ~8MB of memory usage per 10-minute song
#define MAX_THREAD_RESULTS_BUFFER 64

typedef struct
{
	int callback;
	int soundIndex;
	int result;
} loadSoundFileResult_t;

#endif

typedef enum
{
	GRAVITY_NONE,
	GRAVITY_NO_BOUNCE,
	GRAVITY_BOUNCE,
	GRAVITY_NUM
} customGravityType_t;

typedef enum
{
	CUSTOM_TEAM_NONE,
	CUSTOM_TEAM_AXIS,
	CUSTOM_TEAM_ALLIES,
	CUSTOM_TEAM_SPECTATOR,
	CUSTOM_TEAM_AXIS_ALLIES,
	CUSTOM_TEAM_AXIS_SPECTATOR,
	CUSTOM_TEAM_ALLIES_SPECTATOR,
	CUSTOM_TEAM_ALL
} customTeam_t;

typedef struct customEntityState_s
{
	// Note: Some stock fields are still reused for new stuff where archiving is required
	customGravityType_t gravityType;
	qboolean collideModels;
	vec3_t velocity;
	qboolean angledGravity;
	float parallelBounce;
	float perpendicularBounce;
	qboolean convertedTrigger;
} customEntityState_t;

typedef struct customPlayerState_s
{
	qboolean overridePing;
	int ping;
	qboolean overrideStatusPing;
	int statusPing;
	objective_t objectives[16];
	float meleeHeightScale;
	float meleeRangeScale;
	float meleeWidthScale;
	qboolean overrideBulletMask;
	int bulletMask;
	int fireThroughWalls;
	float fireRangeScale;
	qboolean noPickup;
	qboolean noEarthquakes;
	customTeam_t collisionTeam;
	qboolean silent;
	int speed;
	int gravity;
	int gamestateSize;
	int animation;
	int fps;
	int frames;
	customTeam_t headIconTeam;
	uint64_t frameTime;
	qboolean noBulletImpacts;
	byte talkerIcons[64];
	int previousButtons;
	float turretSpreadScale;
	float weaponSpreadScale;
	#if COMPILE_BOTS == 1
	int botButtons;
	int botWeapon;
	char botForwardMove;
	char botRightMove;
	#endif
	#if COMPILE_CUSTOM_VOICE == 1
	float pendingVoiceDataFrames;
	int currentSoundTalker;
	int currentSoundIndex;
	int sentVoiceDataIndex;
	#endif
	#if COMPILE_JUMP == 1
	qboolean overrideJumpHeight;
	int jumpHeight;
	qboolean overrideJumpSlowdown;
	int jumpSlowdown;
	#endif
} customPlayerState_t;

typedef struct callback_s
{
	int *pos;
	const char *name;
} callback_t;

typedef struct
{
	unsigned short all;
	unsigned short artist;
	unsigned short axis_allies;
	unsigned short bot_trigger;
	unsigned short bounce;
	unsigned short flags;
	unsigned short land;
	#if COMPILE_CUSTOM_VOICE == 1
	unsigned short sound_file_done;
	unsigned short sound_file_stop;
	#endif
	unsigned short title;
	unsigned short trigger_radius;
} customStringIndex_t;