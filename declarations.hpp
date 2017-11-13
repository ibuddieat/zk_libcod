#ifndef _DECLARATIONS_HPP_
#define _DECLARATIONS_HPP_

#define qboolean int
#define qtrue 1
#define qfalse 0

typedef unsigned char byte;
typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;

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

typedef enum
{
	CS_FREE,
	CS_ZOMBIE,
	CS_CONNECTED,
	CS_PRIMED,
	CS_ACTIVE
} clientState_t;

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
	byte ipx[10];
	unsigned short port;
} netadr_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	qboolean overflowed;
	byte* data;
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

typedef union
{
	int i;
	byte rgba[4];
} ucolor_t;

typedef struct cvar_s
{
	char *name;
	char *description;
	short int flags;
	byte type;
	byte modified;
	union
	{
		float floatval;
		float value;
		int integer;
		char* string;
		byte boolean;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		ucolor_t color;
	};
	union
	{
		float latchedFloatval;
		int latchedInteger;
		char* latchedString;
		byte latchedBoolean;
		vec2_t latchedVec2;
		vec3_t latchedVec3;
		vec4_t latchedVec4;
		ucolor_t latchedColor;
	};
	union
	{
		float resetFloatval;
		int resetInteger;
		char* resetString;
		byte resetBoolean;
		vec2_t resetVec2;
		vec3_t resetVec3;
		vec4_t resetVec4;
		ucolor_t resetColor;
	};
	union
	{
		int imin;
		float fmin;
	};
	union
	{
		int imax;
		float fmax;
		const char** enumStr;
	};
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;


#define	CVAR_ARCHIVE		1
#define	CVAR_USERINFO		2
#define	CVAR_SERVERINFO		4
#define	CVAR_SYSTEMINFO		8
#define	CVAR_INIT			16
#define	CVAR_LATCH			32
#define	CVAR_ROM			64
#define CVAR_CHEAT			128
#define	CVAR_TEMP			256
#define CVAR_NORESTART		1024
#define	CVAR_USER_CREATED	16384

struct VariableStackBuffer
{
	const char *pos;
	u_int16_t size;
	u_int16_t bufLen;
	u_int16_t localId;
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

typedef struct
{
	union VariableUnion u;
	int type;
} VariableValue;

typedef struct
{
	const char *fieldBuffer;
	u_int16_t canonicalStrCount;
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
	u_int16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	struct HunkUser *programHunkUser;
	const char *programBuffer;
	const char *endScriptBuffer;
	u_int16_t saveIdMap[24574];
	u_int16_t saveIdMapRev[24574];
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

typedef int	fileHandle_t;

typedef void (*xfunction_t)();

typedef struct scr_function_s
{
	const char      *name;
	xfunction_t     call;
	int             developer;
} scr_function_t;

typedef void (*xmethod_t)(int);

typedef struct scr_method_s
{
	const char     *name;
	xmethod_t      call;
	int            developer;
} scr_method_t;

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
	EV_FOOTSTEP_RUN_GLASS,
	EV_FOOTSTEP_RUN_GRASS,
	EV_FOOTSTEP_RUN_GRAVEL,
	EV_FOOTSTEP_RUN_ICE,
	EV_FOOTSTEP_RUN_METAL,
	EV_FOOTSTEP_RUN_MUD,
	EV_FOOTSTEP_RUN_PAPER,
	EV_FOOTSTEP_RUN_PLASTER,
	EV_FOOTSTEP_RUN_ROCK,
	EV_FOOTSTEP_RUN_SAND,
	EV_FOOTSTEP_RUN_SNOW,
	EV_FOOTSTEP_RUN_WATER,
	EV_FOOTSTEP_RUN_WOOD,
	EV_FOOTSTEP_RUN_ASPHALT,
	EV_FOOTSTEP_WALK_DEFAULT,
	EV_FOOTSTEP_WALK_BARK,
	EV_FOOTSTEP_WALK_BRICK,
	EV_FOOTSTEP_WALK_CARPET,
	EV_FOOTSTEP_WALK_CLOTH,
	EV_FOOTSTEP_WALK_CONCRETE,
	EV_FOOTSTEP_WALK_DIRT,
	EV_FOOTSTEP_WALK_FLESH,
	EV_FOOTSTEP_WALK_FOLIAGE,
	EV_FOOTSTEP_WALK_GLASS,
	EV_FOOTSTEP_WALK_GRASS,
	EV_FOOTSTEP_WALK_GRAVEL,
	EV_FOOTSTEP_WALK_ICE,
	EV_FOOTSTEP_WALK_METAL,
	EV_FOOTSTEP_WALK_MUD,
	EV_FOOTSTEP_WALK_PAPER,
	EV_FOOTSTEP_WALK_PLASTER,
	EV_FOOTSTEP_WALK_ROCK,
	EV_FOOTSTEP_WALK_SAND,
	EV_FOOTSTEP_WALK_SNOW,
	EV_FOOTSTEP_WALK_WATER,
	EV_FOOTSTEP_WALK_WOOD,
	EV_FOOTSTEP_WALK_ASPHALT,
	EV_FOOTSTEP_PRONE_DEFAULT,
	EV_FOOTSTEP_PRONE_BARK,
	EV_FOOTSTEP_PRONE_BRICK,
	EV_FOOTSTEP_PRONE_CARPET,
	EV_FOOTSTEP_PRONE_CLOTH,
	EV_FOOTSTEP_PRONE_CONCRETE,
	EV_FOOTSTEP_PRONE_DIRT,
	EV_FOOTSTEP_PRONE_FLESH,
	EV_FOOTSTEP_PRONE_FOLIAGE,
	EV_FOOTSTEP_PRONE_GLASS,
	EV_FOOTSTEP_PRONE_GRASS,
	EV_FOOTSTEP_PRONE_GRAVEL,
	EV_FOOTSTEP_PRONE_ICE,
	EV_FOOTSTEP_PRONE_METAL,
	EV_FOOTSTEP_PRONE_MUD,
	EV_FOOTSTEP_PRONE_PAPER,
	EV_FOOTSTEP_PRONE_PLASTER,
	EV_FOOTSTEP_PRONE_ROCK,
	EV_FOOTSTEP_PRONE_SAND,
	EV_FOOTSTEP_PRONE_SNOW,
	EV_FOOTSTEP_PRONE_WATER,
	EV_FOOTSTEP_PRONE_WOOD,
	EV_FOOTSTEP_PRONE_ASPHALT,
	EV_JUMP_DEFAULT,
	EV_JUMP_BARK,
	EV_JUMP_BRICK,
	EV_JUMP_CARPET,
	EV_JUMP_CLOTH,
	EV_JUMP_CONCRETE,
	EV_JUMP_DIRT,
	EV_JUMP_FLESH,
	EV_JUMP_FOLIAGE,
	EV_JUMP_GLASS,
	EV_JUMP_GRASS,
	EV_JUMP_GRAVEL,
	EV_JUMP_ICE,
	EV_JUMP_METAL,
	EV_JUMP_MUD,
	EV_JUMP_PAPER,
	EV_JUMP_PLASTER,
	EV_JUMP_ROCK,
	EV_JUMP_SAND,
	EV_JUMP_SNOW,
	EV_JUMP_WATER,
	EV_JUMP_WOOD,
	EV_JUMP_ASPHALT,
	EV_LANDING_DEFAULT,
	EV_LANDING_BARK,
	EV_LANDING_BRICK,
	EV_LANDING_CARPET,
	EV_LANDING_CLOTH,
	EV_LANDING_CONCRETE,
	EV_LANDING_DIRT,
	EV_LANDING_FLESH,
	EV_LANDING_FOLIAGE,
	EV_LANDING_GLASS,
	EV_LANDING_GRASS,
	EV_LANDING_GRAVEL,
	EV_LANDING_ICE,
	EV_LANDING_METAL,
	EV_LANDING_MUD,
	EV_LANDING_PAPER,
	EV_LANDING_PLASTER,
	EV_LANDING_ROCK,
	EV_LANDING_SAND,
	EV_LANDING_SNOW,
	EV_LANDING_WATER,
	EV_LANDING_WOOD,
	EV_LANDING_ASPHALT,
	EV_LANDING_PAIN_DEFAULT,
	EV_LANDING_PAIN_BARK,
	EV_LANDING_PAIN_BRICK,
	EV_LANDING_PAIN_CARPET,
	EV_LANDING_PAIN_CLOTH,
	EV_LANDING_PAIN_CONCRETE,
	EV_LANDING_PAIN_DIRT,
	EV_LANDING_PAIN_FLESH,
	EV_LANDING_PAIN_FOLIAGE,
	EV_LANDING_PAIN_GLASS,
	EV_LANDING_PAIN_GRASS,
	EV_LANDING_PAIN_GRAVEL,
	EV_LANDING_PAIN_ICE,
	EV_LANDING_PAIN_METAL,
	EV_LANDING_PAIN_MUD,
	EV_LANDING_PAIN_PAPER,
	EV_LANDING_PAIN_PLASTER,
	EV_LANDING_PAIN_ROCK,
	EV_LANDING_PAIN_SAND,
	EV_LANDING_PAIN_SNOW,
	EV_LANDING_PAIN_WATER,
	EV_LANDING_PAIN_WOOD,
	EV_LANDING_PAIN_ASPHALT,
	EV_FOLIAGE_SOUND,
	EV_STANCE_FORCE_STAND,
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_ITEM_PICKUP,
	EV_AMMO_PICKUP,
	EV_NOAMMO,
	EV_EMPTYCLIP,
	EV_EMPTY_OFFHAND,
	EV_RESET_ADS,
	EV_RELOAD,
	EV_RELOAD_FROM_EMPTY,
	EV_RELOAD_START,
	EV_RELOAD_END,
	EV_RAISE_WEAPON,
	EV_PUTAWAY_WEAPON,
	EV_WEAPON_ALT,
	EV_PULLBACK_WEAPON,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,
	EV_RECHAMBER_WEAPON,
	EV_EJECT_BRASS,
	EV_MELEE_SWIPE,
	EV_FIRE_MELEE,
	EV_PREP_OFFHAND,
	EV_USE_OFFHAND,
	EV_SWITCH_OFFHAND,
	EV_BINOCULAR_ENTER,
	EV_BINOCULAR_EXIT,
	EV_BINOCULAR_FIRE,
	EV_BINOCULAR_RELEASE,
	EV_BINOCULAR_DROP,
	EV_MELEE_HIT,
	EV_MELEE_MISS,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_QUADBARREL_1,
	EV_FIRE_QUADBARREL_2,
	EV_BULLET_TRACER,
	EV_SOUND_ALIAS,
	EV_SOUND_ALIAS_AS_MASTER,
	EV_BULLET_HIT_SMALL,
	EV_BULLET_HIT_LARGE,
	EV_SHOTGUN_HIT,
	EV_BULLET_HIT_AP,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	EV_GRENADE_BOUNCE,
	EV_GRENADE_EXPLODE,
	EV_ROCKET_EXPLODE,
	EV_ROCKET_EXPLODE_NOMARKS,
	EV_CUSTOM_EXPLODE,
	EV_CUSTOM_EXPLODE_NOMARKS,
	EV_BULLET,
	EV_PLAY_FX,
	EV_PLAY_FX_ON_TAG,
	EV_EARTHQUAKE,
	EV_GRENADE_SUICIDE,
	EV_OBITUARY
} entity_event_t;

typedef enum
{
	TRACE_HITTYPE_NONE = 0x0,
	TRACE_HITTYPE_ENTITY = 0x1,
	TRACE_HITTYPE_DYNENT_MODEL = 0x2,
	TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
	TRACE_HITTYPE_GLASS = 0x4
} TraceHitType;

typedef struct trace_s
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	TraceHitType hitType;
	u_int16_t hitId;
	u_int16_t modelIndex;
	u_int16_t partName;
	u_int16_t partGroup;
	byte allsolid;
	byte startsolid;
	byte walkable;
	byte padding;
} trace_t;

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s
{
	netadrtype_t type;
	unsigned char adr[4];
	int	lastTime;
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

#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2
#define MAX_MSGLEN 0x4000
#elif COD_VERSION == COD2_1_3
#define MAX_MSGLEN 0x20000
#endif

typedef void netProfileInfo_t;

typedef struct
{
	int			outgoingSequence;
	netsrc_t	sock;
	int			dropped;
	int			incomingSequence;
	netadr_t	remoteAddress;
	int 		qport;
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
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
	TR_STATIONARY,
	TR_INTERPOLATE,
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,
	TR_GRAVITY
} trType_t;

typedef struct
{
	trType_t	trType;
	int			trTime;
	int			trDuration;
	vec3_t		trBase;
	vec3_t		trDelta;
} trajectory_t;

typedef struct entityState_s
{
	int number;
	int eType;
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
	int surfaceFlags;
	int modelindex;
	int clientNum;
	int	iHeadIcon;
	int	iHeadIconTeam;
	int	solid;
	int	eventParm;
	int	eventSequence;
	vec4_t events;
	vec4_t eventParms;
	int weapon;
	int legsAnim;
	int torsoAnim;
	int stage;
	int loopfxid;
	int hintstring;
	int animMovetype;
	vec3_t unkAngles;
} entityState_t; // verified

typedef struct
{
	byte		linked;
	byte		bmodel;
	byte		svFlags;
	byte		pad1;
	int			clientMask[2];
	byte		inuse;
	byte		pad2[3];
	int			broadcastTime;
	vec3_t		mins, maxs;
	int			contents;
	vec3_t		absmin, absmax;
	vec3_t		currentOrigin;
	vec3_t		currentAngles;
	u_int16_t	ownerNum;
	u_int16_t	pad3;
	int			eventTime;
} entityShared_t; // verified

typedef enum
{
	STAT_HEALTH,
	STAT_DEAD_YAW,
	STAT_MAX_HEALTH,
	STAT_UNKNOWN1,
	STAT_UNKNOWN2,
	STAT_UNKNOWN3
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
	hudelem_color_t glowColor;
} hudelem_t; // verified

typedef struct hudElemState_s
{
	hudelem_t current[31];
	hudelem_t archival[31];
} hudElemState_t;

typedef struct playerState_s
{
	int	commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	vec2_t oldVelocity;
	int weaponDelay;
	// not confirmed below
	int	grenadeTimeLeft;
	int	throwBackGrenadeOwner;
	int	throwBackGrenadeTimeLeft;
	int	weaponRestrictKickTime;
	// end of not confirmed
	int	gravity;
	float leanf;
	int speed;
	vec3_t delta_angles;
	int groundEntityNum;
	vec3_t vLadderVec;
	int jumpTime;
	float jumpOriginZ;
	int legsTime;
	int legsAnim;
	int torsoTime;
	int torsoAnim;
	int	legsAnimDuration;
	int	torsoAnimDuration;
	int	damageTimer;
	int	damageDuration;
	int	flinchYawAnim;
	int	movementDir;
	int	eFlags;
	int	eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int	oldEventSequence;
	int	clientNum;
	int	offHandIndex;
	unsigned int weapon;
	int	weaponstate;
	float fWeaponPosFrac;
	int adsDelayTime;
	int	viewmodelIndex;
	vec3_t viewangles;
	char unk40[40];
	int	damageEvent;
	int	damageYaw;
	int	damagePitch;
	int	damageCount;
	int	stats[6];
	int	ammo[128];
	int	ammoclip[128];
	// not confirmed below
	unsigned int weapons[4];
	unsigned int weaponsold[4];
	unsigned int weaponrechamber[2];
	// end of not confirmed
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	int unknown3[3];
	ViewLockTypes_t	viewlocked;
	int unknown;
	int viewlocked_entNum;
	int unknown10[10];
	int entityEventSequence;
	int	weapAnim;
	float aimSpreadScale;
	int	shellshockIndex;
	int	shellshockTime;
	int	shellshockDuration;
	objective_t objective[16];
	int archiveTime;
	hudElemState_t hud;
} playerState_t;

typedef enum
{
	TEAM_NONE,
	TEAM_AXIS,
	TEAM_ALLIES,
	TEAM_SPECTATOR
} sessionTeam_t;

typedef struct
{
	sessionState_t state;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int	score;
	int deaths;
	u_int16_t scriptPersId;
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
	float moveSpeedScaleMultiplier; // ?
	int viewmodelIndex;
	qboolean noSpectate;
	int teamInfo;
	int clientId;
	sessionTeam_t team;
	int model;
	int attachedModels[6];
	int attachedModelsTags[6];
	char manualModeName[32];
	int psOffsetTime;
} clientSession_t; // verified

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
	int accurateCount; // N/A
	int accuracy_shots; // N/A
	int accuracy_hits; // N/A
	int inactivityTime;
	qboolean inactivityWarning;
	int playerTalkTime;
	int rewardTime; // N/A
	float antilagShootTime;
	int unused_space[6];
	gentity_t *lookAtEntity; // needs a NULL check, otherwise crash.
	int activateEntNumber;
	int activateTime;
	int nonPVSFriendlyEntNum;
	int pingPlayerTime;
	int damageFeedBackTime;
	vec2_t damageFeedBackDir;
	float weaponSwayDir; // 10316
	int unused_space2[15];
	float weaponRecoil; // 10380
	int unused_space3[4];
	int lastServerTime;
}; // verified

typedef int turretInfo_s;

struct gentity_s
{
	entityState_t s;
	entityShared_t r;
	struct gclient_s *client;
	turretInfo_s *pTurretInfo;
	byte physicsObject; // 352
	byte takedamage;
	byte active;
	byte nopickup;
	byte handler;
	byte team;
	u_int16_t classname;
	u_int16_t target;
	u_int16_t targetname;
	u_int16_t pad;
	int spawnflags;
	int flags;
	int eventTime;
	int tempEntity;
	int dobj; // ? maybe not 384
	int clipmask;
	int unknown; //  392
	int pointContents;
	int nextthink; // 400
	int damage;
	int unknown2;
	int damage2;
	int unknownItemRelated;
	int unknown3;
	int unknownItemRelated2;
	int item; // 428
	int hurtTouchTime;
	int useSharedNum;
	int offsetTime;
	int unknown4[9];
	int think; // ?
	float unknown5;
	int unknown6;
	int playerStatePrediction[3]; // 500
	vec3_t lerpPosition; // 512
	vec3_t moverOrigin; // 524
	byte attachedModels[6]; // 528
	byte free;
	byte free2;
	byte numAttachedModels; // 540
	byte free3;
	byte free4;
	int animTree; // ?
	vec4_t color;
}; // verified

#define MAX_DOWNLOAD_BLKSIZE 1024
#define MAX_DOWNLOAD_WINDOW 8

typedef struct
{
	playerState_t ps;
	int	num_entities;
	int	num_clients;
	int	first_entity;
	int	first_client;
	unsigned int messageSent;
	unsigned int messageAcked;
	int	messageSize;
} clientSnapshot_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	char num;
	char data[256];
	int dataLen;
} voices_t;
#pragma pack(pop)

typedef struct client_s
{
	clientState_t	state;
	int				gamestateSent;
	int				unkClientStateVar;
	char			userinfo[1024];
	reliableCommands_t	reliableCommands[128];
	int				reliableSequence;
	int				reliableAcknowledge;
	int				reliableSent;
	int				messageAcknowledge;
	int				gamestateMessageNum;
	int				challenge;
	usercmd_t  	 	lastUsercmd;
	int				lastClientCommand;
	char			lastClientCommandString[1024];
	gentity_t 		*gentity;
	char 			name[32];
	char			downloadName[64];
	fileHandle_t	download;
	int				downloadSize;
	int				downloadCount;
	int				downloadClientBlock;
	int				downloadCurrentBlock;
	int				downloadXmitBlock;
	unsigned char	*downloadBlocks[MAX_DOWNLOAD_WINDOW];
	int				downloadBlockSize[MAX_DOWNLOAD_WINDOW];
	qboolean		downloadEOF;
	int				downloadSendTime;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char			wwwDownloadURL[256];
	qboolean		wwwDownload;
	qboolean		wwwDownloadStarted;
	qboolean		wwwDlAck;
	qboolean		wwwDl_failed;
#endif
	int				deltaMessage;
	int				nextReliableTime;
	int				lastPacketTime;
	int				lastConnectTime;
	int				nextSnapshotTime;
	qboolean		rateDelayed;
	int				timeoutCount;
	clientSnapshot_t frames[32];
	int				ping;
	int				rate;
	int				snapshotMsec;
	int				pureAuthentic;
	netchan_t		netchan;
	int 			guid;
	short			clscriptid;
	int				bot;
	int				serverId;
	voices_t		voicedata[40];
	int				unsentVoiceData;
	byte			mutedClients[64];
	byte			hasVoip;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char 			pbguid[64];
#endif
} client_t;

typedef struct archivedSnapshot_s
{
	int start;
	int size;
} archivedSnapshot_t;

typedef struct cachedClient_s
{
	int playerStateExists;
	clientState_t *cs;
	playerState_t *ps;
} cachedClient_t;

typedef struct
{
	qboolean	initialized;
	int			time;
	int			snapFlagServerBit;
	client_t	*clients;
	int			numSnapshotEntities;
	int			numSnapshotClients;
	int			nextSnapshotEntities;
	int			nextSnapshotClients;
	entityState_t *snapshotEntities;
	clientState_t *snapshotClients;
	int 		archivedSnapshotEnabled;
	int 		nextArchivedSnapshotFrames;
	archivedSnapshot_t *archivedSnapshotFrames;
	int 		*archivedSnapshotBuffer;
	int 		nextArchivedSnapshotBuffer;
	int			nextCachedSnapshotEntities;
	int 		nextCachedSnapshotClients;
	int 		nextCachedSnapshotFrames;
	cachedClient_t cachedSnapshotClients;
	int			nextHeartbeatTime;
	int 		nextStatusResponseTime;
	challenge_t	challenges[1024];
	netadr_t	redirectAddress;
	netadr_t	authorizeAddress;
	char 		netProfilingBuf[1504];
} serverStatic_t; // verified

#define g_entities ((gentity_t*)(gentities))
#define g_clients ((gclient_t*)(playerStates))

#define KEY_MASK_FORWARD        127
#define KEY_MASK_BACK           -127
#define KEY_MASK_MOVERIGHT      127
#define KEY_MASK_MOVELEFT       -127

#define KEY_MASK_FIRE           1
#define KEY_MASK_MELEE			4
#define KEY_MASK_USE            8
#define KEY_MASK_RELOAD         16
#define KEY_MASK_LEANLEFT       64
#define KEY_MASK_LEANRIGHT      128
#define KEY_MASK_PRONE          256
#define KEY_MASK_CROUCH         512
#define KEY_MASK_JUMP           1024
#define KEY_MASK_ADS_MODE       4096
#define KEY_MASK_MELEE_BREATH   32772
#define KEY_MASK_HOLDBREATH     32768
#define KEY_MASK_FRAG           65536
#define KEY_MASK_SMOKE          131072

#endif