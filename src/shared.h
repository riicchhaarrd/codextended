/*
	This file is part of CoDExtended.

    CoDExtended is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CoDExtended is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CoDExtended.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SHARED_H
#define SHARED_H

//#pragma extern_prefix "xtn_"

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <execinfo.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include "util.h"
#include "surfaceflags.h"
#include "build.txt"

#ifdef uMYSQL
#include "mysql/mysql.h"
#endif
#include <malloc.h>
#include <mcheck.h>

typedef unsigned char byte;
typedef unsigned char boolean;
#define __cdecl __attribute__((__cdecl__))
#define QDECL __cdecl
#define __fastcall __attribute__((fastcall))

#ifdef xDEBUG
#define _STRIP __attribute__((visibility ("hidden")))
#define _VIS __attribute__((visibility ("default")))
#define _DEBUG __attribute__((visibility ("default")))
#define _PROT __attribute__((visibility ("protected")))
#else
#define _STRIP
#define _VIS
#define _STRIP
#define _PROT
#endif

//static int ( QDECL * syscall )( int arg, ... ) = ( int ( QDECL * )( int, ... ) )0x8087DCC;

#define NOP 0x90
#define VERSION_STRING "Call of Duty Extended %.1f\n"
#define EXTENDEDVERSION 1f

static void* (*Z_MallocInternal)(size_t) = (void*(*)(size_t))0x80681E8;
static void (*Z_FreeInternal)(void*) = (void(*)(void*))0x8068224;

static void* (*Hunk_AllocateTempMemoryHighInternal)(size_t) = (void*(*)(size_t))0x806823C;
static void* (*Hunk_ReallocateTempMemory)(size_t) = (void*(*)(size_t))0x806859C;
static void (*Hunk_FreeTempMemory)(void*) = (void(*)(void*))0x8068624;

#define NUMVERTEXNORMALS    162

#define DotProduct( x,y )         ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] )
#define VectorSubtract( a,b,c )   ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2] )
#define VectorAdd( a,b,c )        ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2] )
#define VectorCopy( a,b )         ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2] )
#define VectorScale( v, s, o )    ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ) )
#define VectorMA( v, s, b, o )    ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ) )

#define VectorClear( a )              ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )
#define VectorNegate( a,b )           ( ( b )[0] = -( a )[0],( b )[1] = -( a )[1],( b )[2] = -( a )[2] )
#define VectorSet( v, x, y, z )       ( ( v )[0] = ( x ), ( v )[1] = ( y ), ( v )[2] = ( z ) )

#define Vector2Set( v, x, y )         ( ( v )[0] = ( x ),( v )[1] = ( y ) )
#define Vector2Copy( a,b )            ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1] )
#define Vector2Subtract( a,b,c )      ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1] )

#define Vector4Set( v, x, y, z, n )   ( ( v )[0] = ( x ),( v )[1] = ( y ),( v )[2] = ( z ),( v )[3] = ( n ) )
#define Vector4Copy( a,b )            ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )
#define Vector4MA( v, s, b, o )       ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ),( o )[3] = ( v )[3] + ( b )[3] * ( s ) )
#define Vector4Average( v, b, s, o )  ( ( o )[0] = ( ( v )[0] * ( 1 - ( s ) ) ) + ( ( b )[0] * ( s ) ),( o )[1] = ( ( v )[1] * ( 1 - ( s ) ) ) + ( ( b )[1] * ( s ) ),( o )[2] = ( ( v )[2] * ( 1 - ( s ) ) ) + ( ( b )[2] * ( s ) ),( o )[3] = ( ( v )[3] * ( 1 - ( s ) ) ) + ( ( b )[3] * ( s ) ) )

#define SnapVector( v ) {v[0] = ( (int)( v[0] ) ); v[1] = ( (int)( v[1] ) ); v[2] = ( (int)( v[2] ) );}

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

#define PITCH               0       // up / down
#define YAW                 1       // left / right
#define ROLL                2       // fall over
#define M_PI 3.14

#define MAKERGB( v, r, g, b ) v[0] = r; v[1] = g; v[2] = b
#define MAKERGBA( v, r, g, b, a ) v[0] = r; v[1] = g; v[2] = b; v[3] = a

#define DEG2RAD( a ) ( ( ( a ) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( ( a ) * 180.0f ) / M_PI )

#define nanmask ( 255 << 23 )

#define IS_NAN( x ) ( ( ( *(int *)&x ) & nanmask ) == nanmask )

#define random()    ( ( rand() & 0x7fff ) / ( (float)0x7fff ) )
#define crandom()   ( 2.0 * ( random() - 0.5 ) )

//typedef enum { qfalse, qtrue }	qboolean;
#define qboolean int //g++ complaining again
#define qtrue 1
#define qfalse 0

#define MAX_MSGLEN 0x4000

typedef int clipHandle_t;

int     _Q_rand( int *seed );
float   _Q_random( int *seed );
float   _Q_crandom( int *seed );
signed char _ClampChar( int i );
signed short _ClampShort( int i );
int _DirToByte( vec3_t dir );
void _ByteToDir( int b, vec3_t dir );
unsigned _ColorBytes3( float r, float g, float b );
unsigned _ColorBytes4( float r, float g, float b, float a );
float _NormalizeColor( const vec3_t in, vec3_t out );
void _RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point,
							  float degrees );
void _RotateAroundDirection( vec3_t axis[3], float yaw );
void _vectoangles( const vec3_t value1, vec3_t angles );
void _AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
void _AxisClear( vec3_t axis[3] );
void _AxisCopy( vec3_t in[3], vec3_t out[3] );
void _ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal );
void _MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
void _VectorRotate( vec3_t in, vec3_t matrix[3], vec3_t out );
float _Q_rsqrt( float number );
float _Q_fabs( float f );
float _LerpAngle( float from, float to, float frac );
void _LerpPosition( vec3_t start, vec3_t end, float frac, vec3_t out );
float   _AngleSubtract( float a1, float a2 );
void _AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );
float   _AngleMod( float a );
float _AngleNormalize360( float angle );
float _AngleNormalize180( float angle );
float _AngleDelta( float angle1, float angle2 );
float _RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
void _ClearBounds( vec3_t mins, vec3_t maxs );
void _AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );
int _VectorCompare( const vec3_t v1, const vec3_t v2 );
vec_t _VectorNormalize( vec3_t v );
void _VectorNormalizeFast( vec3_t v );
vec_t _VectorNormalize2( const vec3_t v, vec3_t out );
vec_t _VectorLength( const vec3_t v );
vec_t _VectorLengthSquared( const vec3_t v );
vec_t _Distance( const vec3_t p1, const vec3_t p2 );
vec_t _DistanceSquared( const vec3_t p1, const vec3_t p2 );
void _VectorInverse( vec3_t v );
void _Vector4Scale( const vec4_t in, vec_t scale, vec4_t out );
void _AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void _MatrixMultiply( float in1[3][3], float in2[3][3], float out[3][3] );
void _PerpendicularVector( vec3_t dst, const vec3_t src );
void _GetPerpendicularViewVector( const vec3_t point, const vec3_t p1, const vec3_t p2, vec3_t up );
void _ProjectPointOntoVector( vec3_t point, vec3_t vStart, vec3_t vEnd, vec3_t vProj );
float _vectoyaw( const vec3_t vec );
float _VectorDistance( vec3_t v1, vec3_t v2 );

typedef enum {
	ERR_FATAL,                  // exit the entire game with a popup window
	ERR_VID_FATAL,              // exit the entire game with a popup window and doesn't delete profile.pid
	ERR_DROP,                   // print to console and disconnect from game
	ERR_SERVERDISCONNECT,       // don't kill server
	ERR_DISCONNECT,             // client disconnected from the server
	ERR_NEED_CD,                // pop up the need-cd dialog
	ERR_AUTOUPDATE
} errorParm_t;

/*
    CVARS
*/

#define CVAR_ARCHIVE        1   // set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define CVAR_USERINFO       2   // sent to server on connect or change
#define CVAR_SERVERINFO     4   // sent in response to front end requests
#define CVAR_SYSTEMINFO     8   // these cvars will be duplicated on all clients
#define CVAR_INIT           16  // don't allow change from console at all,
								// but can be set from the command line
#define CVAR_LATCH          32  // will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define CVAR_ROM            64  // display only, cannot be set by user at all
#define CVAR_USER_CREATED   128 // created by a set command
#define CVAR_TEMP           256 // can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT          512 // can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued
#define CVAR_WOLFINFO       2048    // DHM - NERVE :: Like userinfo, but for wolf multiplayer info

#define CVAR_UNSAFE         4096    // ydnar: unsafe system cvars (renderer, sound settings, anything that might cause a crash)
#define CVAR_SERVERINFO_NOUPDATE        8192    // gordon: WONT automatically send this to clients, but server browsers will see it

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
	char        *name;
	char        *string;
	char        *resetString;       // cvar_restart will reset to this value
	char        *latchedString;     // for CVAR_LATCH vars
	int flags;
	qboolean modified;              // set each time the cvar is changed
	int modificationCount;          // incremented each time the cvar is changed
	float value;                    // atof( string )
	int integer;                    // atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

#define MAX_CVAR_VALUE_STRING   256

typedef int cvarHandle_t;

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
typedef struct {
	cvarHandle_t handle;
	int modificationCount;
	float value;
	int integer;
	char string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;


#define PASSFLOAT( x ) (*(INT*)&x)

#define    MASK_ALL                (-1)
#define    MASK_SOLID                (CONTENTS_SOLID)
#define    MASK_PLAYERSOLID        (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define    MASK_DEADSOLID            (CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define    MASK_WATER                (CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define    MASK_OPAQUE                (CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define    MASK_SHOT                (CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)

//#define MAX_STRING_CHARS    1024    // max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_CHARS    1024    // max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_TOKENS   256     // max tokens resulting from Cmd_TokenizeString
#define MAX_TOKEN_CHARS     1024    // max length of an individual token

#define MAX_INFO_STRING     1024
#define MAX_INFO_KEY        1024
#define MAX_INFO_VALUE      1024

#define BIG_INFO_STRING     8192    // used for system info key only
#define BIG_INFO_KEY        8192
#define BIG_INFO_VALUE      8192

#define MAX_QPATH           64      // max length of a quake game pathname
#define MAX_OSPATH          256     // max length of a filesystem pathname

// rain - increased to 36 to match MAX_NETNAME, fixes #13 - UI stuff breaks
// with very long names
#define MAX_NAME_LENGTH     36      // max length of a client name

#define MAX_SAY_TEXT        150

#define MAX_BINARY_MESSAGE  32768   // max length of binary message

typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[2];
} cplane_t;

typedef struct {
	float		fraction;
	vec3_t		endpos;

	vec3_t normal; //normal?
	
	//not sure can be dist? but most likely not due to too far away distance on low range and non-float
	byte	type;
	byte	signbits;
	byte	pad[2];
	
	int contents;
	char *textureName;
	int entityNum;
	int surfaceFlags; //assuming it's surfaceFlags, but where is allsolid, startsolid? ( 2.3509887e-38 or 0x01000000 )
} trace_t;

/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define ANGLE2SHORT(x)  ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)  ((x)*(360.0/65536))

#define SNAPFLAG_RATE_DELAYED 1
#define SNAPFLAG_NOT_ACTIVE   2 // snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT  4 // toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define MAX_CLIENTS     64    // absolute limit
#define MAX_LOCATIONS   64

#define GENTITYNUM_BITS   10    // don't need to send any more
#define MAX_GENTITIES   (1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values that are going to be communcated over the net need to
// also be in this range
#define ENTITYNUM_NONE    (MAX_GENTITIES-1)
#define ENTITYNUM_WORLD   (MAX_GENTITIES-2)
#define ENTITYNUM_MAX_NORMAL  (MAX_GENTITIES-2)

#pragma pack(1)
typedef struct {
	int overflowed; //0
	byte* data; //4
	int maxsize; //8 (most likely maxsize value 16384)
	int cursize; //12 //value 129
	int readcount; //16 //value 16
	int bit; //20 //value 0
} msg_t; //size is 0x18 (24)
#pragma pack(push, 1)

extern short   BigShort( short l );
extern short   LittleShort( short l );
extern int     BigLong( int l );
extern int     LittleLong( int l );
extern float   BigFloat( float l );
extern float   LittleFloat( float l );

typedef void (*MSG_BeginReading_t)( msg_t *msg );
typedef int     (*MSG_ReadLong_t)( msg_t *sb );
typedef char* (*MSG_ReadStringLine_t)( msg_t *sb );
typedef char * (*MSG_ReadString_t)( msg_t *);

extern MSG_ReadStringLine_t MSG_ReadStringLine;
extern MSG_ReadLong_t MSG_ReadLong;
extern MSG_BeginReading_t MSG_BeginReading;
extern MSG_ReadString_t MSG_ReadString;

typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum {
    ET_GENERAL, //0
    ET_PLAYER, //1
    ET_CORPSE, //2
    ET_ITEM, //3
    ET_MISSILE, //4
    ET_MOVER, //5
    ET_PORTAL, //6
    ET_INVISIBLE, //7
    ET_SCRIPTMOVER, //8
	ET_UNKNOWN, //9
	ET_FX, //10
	ET_TURRET, //11
	ET_EVENTS //12
} entityType_t;

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,             // non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_LINEAR_STOP_BACK,        //----(SA)	added.  so reverse movement can be different than forward
	TR_SINE,                    // value = base + sin( time / duration ) * delta
	TR_GRAVITY,
	// Ridah
	TR_GRAVITY_LOW,
	TR_GRAVITY_FLOAT,           // super low grav with no gravity acceleration (floating feathers/fabric/leaves/...)
	TR_GRAVITY_PAUSED,          //----(SA)	has stopped, but will still do a short trace to see if it should be switched back to TR_GRAVITY
	TR_ACCELERATE,
	TR_DECCELERATE,
	// Gordon
	TR_SPLINE,
	TR_LINEAR_PATH
} trType_t;

typedef struct {
	trType_t trType;
	int trTime;
	int trDuration;             // if non 0, trTime + trDuration = stop time
//----(SA)	removed
	vec3_t trBase;
	vec3_t trDelta;             // velocity, etc
//----(SA)	removed
} trajectory_t;

#pragma pack(1)
typedef struct {
    byte unk[480];
    int maxclients;
    int frameNum;
    int time;
    int previousTime;
    int startTime; //?idk
} level_locals_t;
#pragma pack(push, 1)
extern level_locals_t *level;

//gentity->eFlags
#define EF_DEAD 0x1
#define EF_CROUCH 0x20
#define EF_PRONE 0x40
#define EF_NODRAW 0x100
#define EF_AIM 0x200 //if a player is typing in chat or if their console is open or menu is open
#define EF_FIRING 0x400
#define EF_TALK 0x40000
#define ET_MIX = 0x3000000 //ET_BOUNCE | ET_BOUNCE_HALF
//#define ET_TURRET = 0xC000 //when player is on mg42 e.g
/* Already using ET_TURRET in entity types */
//gentity->flags
#define FL_NODRAW 0x1000

#define WBUTTON_RELOAD 8

typedef enum {
	WEAPON_READY, //0
	WEAPON_RAISING, //1
	WEAPON_DROPPING, //2
	WEAPON_FIRING, //3
	WEAPON_RECHAMBERING, //4
	WEAPON_RELOADING, //5
	WEAPON_RELOADING_INTERUPT, //6
	WEAPON_RELOAD_START, //7
	WEAPON_RELOAD_START_INTERUPT, //8
	WEAPON_RELOAD_END, //9
	WEAPON_MELEE_WINDUP, //10
	WEAPON_MELEE_RELAX, //11
	WEAPON_UNKNOWN //12
} weaponstates;

typedef enum {
	WEAP_IDLE, //0
	WEAP_UNK, //1
	WEAP_ATTACK, //2
	WEAP_ATTACK_LASTSHOT, //3
	WEAP_RECHAMBER, //4
	WEAP_ADS_ATTACK, //5
	WEAP_ADS_ATTACK_LASTSHOT, //6
	WEAP_ADS_RECHAMBER, //7
	WEAP_MELEE_ATTACK, //8
	WEAP_DROP, //9
	WEAP_RAISE, //10
	WEAP_RELOAD, //11
	WEAP_RELOAD_EMPTY, //12
	WEAP_RELOAD_START, //13
	WEAP_RELOAD_END, //14
	WEAP_ALTSWITCHFROM, //15
	WEAP_ALTSWITCHTO, //16
	WEAP_UNKNOWN //17
} weaponanimations;

#define MAX_ENTITIES 1024
#define MAX_ENTITY_SIZE 1024
#define PLAYERSTATE_SIZE 0x22cc
#if CODPATCH == 1
#define GENTITY_SIZE 788
#else
#define GENTITY_SIZE 0x31c
#endif

typedef enum {
	SS_PLAYING,
	SS_DEAD,
	SS_SPECTATOR,
	SS_INTERMISSION
} sessionstate_types;

#if CODPATCH == 1
typedef enum {
	EOFF_S_SVFLAGS = 244,
	EOFF_S_GROUNDENTITYNUM = 124,
	EOFF_ETYPE = 4,
    EOFF_EFLAGS = 8,
	EOFF_PHYSICSOBJECT = 353,
    EOFF_PLAYERSTATE = 344,
    EOFF_CONTENTS = 280,
	EOFF_TAKEDAMAGE = 369,
    EOFF_NEXTTHINK = 508,
	EOFF_FLAGS = 380,
    EOFF_THINK = 512,
    EOFF_USE = 528,
    EOFF_PAIN = 532,
    EOFF_DIE = 536,
    EOFF_HEALTH = 560,
    EOFF_CLASSNAME = 374, //it's a string index of scr_
    EOFF_ORIGIN = 308,
    EOFF_ANGLES = 320,
	EOFF_CONTROLLER = 544
} ENTITY_OFFSET;

#else

typedef enum {
	EOFF_S_ETYPE = 4,
	EOFF_EFLAGS = 8,
	EOFF_PLAYERSTATE = 348,
	EOFF_TAKEDAMAGE = 373,
	EOFF_NEXTTHINK = 516,
	EOFF_THINK = 520,
	EOFF_USE = 536,
	EOFF_PAIN = 540,
	EOFF_DIE = 544,
	EOFF_HEALTH = 568,
	EOFF_FLAGS = 388,
	EOFF_R_CONTENTS = 284,
	EOFF_R_SVFLAGS = 244,
	EOFF_TEAMMASTER = 620,
	EOFF_TEAMCHAIN = 616,
	EOFF_TEAM = 480,
	EOFF_INUSE = 356,
	EOFF_CLASSNAME = 380, //it's a string index of scr_
	EOFF_R_OWNERNUM = 336,
	EOFF_EVENTTIME = 392,
	EOFF_FREEAFTEREVENT = 396,
	EOFF_WAIT = 624,
	EOFF_RANDOM = 628,
	EOFF_CLIENTNUM = 144,
	EOFF_ORIGIN = 312,
	EOFF_ANGLES = 324,
	EOFF_PHYSICSOBJECT = 357,
	EOFF_UNLINKAFTEREVENT = 400,
	EOFF_GROUNDENTITYNUM = 124,
	EOFF_LOOPSOUND = 132,
	EOFF_SOUND_OPENING = 359,
	EOFF_SOUND_CLOSING = 360,
	EOFF_SOUND_OPEN_END = 361,
	EOFF_SOUND_OPEN_LOOP = 363,
	EOFF_SOUND_CLOSE_LOOP = 364,
	EOFF_SOUND_LOCKED = 365,
	EOFF_SOUND_OPENING_QUIET = 366,
	EOFF_SOUND_OPEN_QUIET_END = 367,
	EOFF_SOUND_CLOSING_QUIET = 368,
	EOFF_SOUND_CLOSE_QUIET_END = 369,
	EOFF_MOVERSTATE = 376,
	EOFF_SPAWNFLAGS = 384,
	EOFF_DAMAGE = 576,
	EOFF_SPEED = 488,
	EOFF_SPLASHDAMAGE = 580,
	EOFF_R_CURRENTORIGIN = 312,
	EOFF_PARENT = 416,
	EOFF_SPLASHRADIUS = 584,
	EOFF_SPLASHMETHODOFDEATH = 588,
	EOFF_R_EVENTTIME = 340
} ENTITY_OFFSET;

#endif

typedef enum {
	POFF_CLIENTNUM = 172,
	POFF_EFLAGS = 128,
	POFF_PM_TYPE = 4,
    POFF_VELOCITY = 0x20,
    POFF_ANGLES = 0xC0,
	POFF_SESSIONSTATE = 8400
} PLAYER_OFFSET;

#pragma pack(1)

typedef struct {
	char data[64]; //holds some values like origin of player before on turret, is being used flag, degrees of freedom (arcs)
} turret_entity_info; //size 0x40

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

typedef struct entityState_s {
	int number;
	entityType_t eType; //4
	int eFlags; //8
	trajectory_t pos; //12
	trajectory_t apos; //48
	int unk; //84 //time??
	int unk2; //88 //time2??
	vec3_t origin2; //92
	vec3_t angles2; //104 (guessed name)
	int otherEntityNum; //116
	int otherEntityNum2; //120
	int groundEntityNum; //124
	int constantLight; //128
	int loopSound; //132
	int surfaceFlags; //136
	int modelindex; //140
	int clientNum; //144
	char ___cba[0x34];
	/*
	gentity_t *teammaster; //152
	int eventParm; //160
	int eventSequence; //164
	int events[4]; //168
	int eventParms[4]; //184
	*/
	
	int weapon; //200
	int legsAnim; //204
	int torsoAnim; //208
	float leanf; //212
	int loopfxid; //216
	int hintstring; //220
	int animMovetype; //224
} entityState_t;

#pragma pack(push, 1)
typedef struct playerState_s
{
  int commandTime;
  int pm_type;
  int bobCycle;
  int pm_flags;
  int pm_time;
  vec3_t origin;
  vec3_t velocity;
  char gap_2C[20];
  float leanf;
  int speed;
  char gap_48[12];
  int groundEntityNum;
  char gap_58[12];
  int jumpTime;
  int field_68;
  int legsTime;
  int legsAnim;
  int torsoTime;
  int torsoAnim;
  int movementDir;
  int eFlags;
  char gap_84[24];
  int field_9C;
  int field_A0;
  int field_A4;
  int field_A8;
  int clientNum;
  int weapon;
  int field_B4;
  char gap_B8;
  char gap_B9;
  char gap_BA[2];
  int field_BC;
  vec3_t viewangles;
  char gap_CC[40];
  int health;
  char gap_F8[556];
  vec3_t mins;
  vec3_t maxs;
  float viewheight_prone;
  int viewheight_crouched;
  float viewheight_standing;
  int field_348;
  float runSpeedScale;
  float sprintSpeedScale;
  char gap_354[40];
  float friction;
  char gap_380[68];
  float fTorsoHeight;
  float fTorsoPitch;
  float fWaistPitch;
  char rest[7416];
  int end;
} playerState_t;
#pragma pack(pop)

typedef struct usercmd_s {
	int serverTime;
	byte buttons; //console,chat talking, aim down the sight, attackbutton, usebutton
	byte wbuttons; //lean right,left,reload
	byte weapon;
	byte flags;
	int angles[3];

	signed char forwardmove, rightmove, upmove;
	byte unknown; //could be doubleTap or client
} usercmd_t;

#pragma pack(push, 1)
struct gclient_s
{
  playerState_t ps;
  int ab;
  int cb;
  int spectatorClient;
  int idklell;
  int archivetime;
  char gap_20E0[16];
  usercmd_t cmd;
  char gap_2109[204];
  int forceSpectatorClient;
  char gap_21D8[4];
  int noclip;
  char bs;
  char gap_21E1[43];
  int field_220C;
  int field_2210;
  char gap_2214[8140];
  int end;
};
#pragma pack(pop)


typedef enum {
	PERK_QUICK_RELOAD
} perk_names;

#define MAX_PERKS 16

typedef struct gitem_s {
	char        *classname;
	//rest idc
} gitem_t;

struct gentity_s {
  entityState_t s;
  float PM_GetViewHeightLerpTime_unknown;
  float some_view_angle_when_prone;
  float some_view_angle_when_prone2;
  int linked;
  int svFlags;
  int singleClient;
  int unk2;
  float mins[3];
  float maxs[3];
  int contents;
  float absmin[3];
  float absmax[3];
  float currentOrigin[3];
  float currentAngles[3];
  int ownerNum;
  int r_eventTime;
  int unk3;
  gclient_t *client;
  int *turret_info;
  char inuse;
  char physicsObject;
  char sound1;
  char sound2;
  char sound2to1;
  char sound4;
  char soundLoop;
  char sound6;
  char sound3to2;
  char sound8;
  char sound9;
  char sound10;
  char soundSoftclose;
  char sound12;
  char noise_index;
  char idk;
  char idk2;
  char takedamage;
  char active;
  char idk3;
  char moverState;
  unsigned char modelindex2;
  unsigned short classname;
  int spawnflags;
  int flags;
  int eventTime;
  int freeAfterEvent;
  int unlinkAfterEvent;
  float physicsBounce;
  int clipmask;
  int framenum;
  gentity_t *parent;
  gentity_t *nextTrain;
  char rest_fill_for_ida;
  char gap_1A1[3];
  vec3_t pos1;
  vec3_t pos2;
  char gap_1BC[28];
  int team;
  char gap_1DC[4];
  float speed;
  float closespeed;
  vec3_t movedir;
  int gDuration;
  int gDurationBack;
  int nextthink;
  int think;
  int reached;
  int blocked;
  int touch;
  int use;
  int pain;
  int die;
  int field_21C;
  int controller;
  char gap_224;
  char gap_225[11];
  int health;
  int maxhealth;
  int damage;
  char gap_23C;
  char gap_23D[19];
  int accumulate;
  char gap_254[12];
  gentity_t *teamchain;
  gentity_t *teammaster;
  float wait;
  float random;
  char gap_270[4];
  float delay;
  char gap_278[32];
  gitem_t *item;
  int gap_29C;
  char gap_2A0[8];
  int threshold;
  int harc;
  char gap_2B0[4];
  int missionlevel;
  char gap_2B8[16];
  int spawnitem;
  char gap_2CC[24];
  int generlink_field_idk;
  char rest[44];
};
#pragma pack(push, 1)

extern gentity_t *g_entities;
/*
#define ENT_SET(e, off, src, size) do { \
memcpy((void*)((int)e + off), (void*)src, size); \
} while(0)
#define ENT_GET(e, off, dest, size) do { \
memcpy((void*)dest, (void*)((int)e + off), size); \
} while(0)
*/

static inline void ENT_SET(gentity_t *e, int off, void *src, size_t size) {
	//printf("ENT_SET(%x, %d, %x, %d) ent num = %d\n", e, off, src, size, e->s.number);
	memcpy((void*)(((int)e) + off), src, size);
}
static inline void ENT_GET(gentity_t *e, int off, void *dest, size_t size) {
	//printf("ENT_GET(%x, %d, %x, %d) ent num = %d\n", e, off, dest, size, e->s.number);
	memcpy(dest, (void*)(((int)e) + off), size);
}

typedef struct {
	bool sprinting;
} xentity_t;

extern xentity_t xentities[1024];

extern char* modNames[];

typedef struct {
	int weaponNumber; //not sure
	char *weaponName;
	
} weaponInfo;

typedef enum {
    TRAP_PRINT,
    TRAP_ERROR,
    TRAP_ERROR_LOCALIZED,
    TRAP_MILLISECONDS,
    TRAP_CVAR_REGISTER,
    TRAP_CVAR_UPDATE,
    TRAP_CVAR_SET,
    TRAP_CVAR_VARIABLE_INTEGER_VALUE,
    TRAP_CVAR_VARIABLE_VALUE,
    TRAP_CVAR_VARIABLE_STRINGBUFFER,
    TRAP_ARGC,
    TRAP_ARGV,
    TRAP_HUNK_ALLOC_INTERNAL,
    TRAP_HUNK_ALLOC_LOW_INTERNAL,
    TRAP_HUNK_ALLOC_ALIGN_INTERNAL,
    TRAP_HUNK_ALLOC_LOW_ALIGN_INTERNAL,
    TRAP_HUNK_ALLOCATE_TEMP_MEMORY_INTERNAL,
    TRAP_HUNK_FREE_TEMP_MEMORY_INTERNAL,
    TRAP_FS_FOPEN_FILE,
    TRAP_FS_READ,
    TRAP_FS_WRITE,
    TRAP_FS_RENAME,
    TRAP_FS_FCLOSE_FILE,
    TRAP_SEND_CONSOLE_COMMAND,
    TRAP_LOCATE_GAME_DATA,
    TRAP_GETGUID,
    TRAP_DROP_CLIENT,
    TRAP_SEND_SERVER_COMMAND,
    TRAP_SET_CONFIGSTRING,
    TRAP_GET_CONFIGSTRING,
    TRAP_GET_CONFIGSTRINTRAP_CONST,
    TRAP_IS_LOCAL_CLIENT,
    TRAP_GET_CLIENT_PING,
    TRAP_GET_USERINFO,
    TRAP_SET_USERINFO,
    TRAP_GET_SERVERINFO,
    TRAP_SET_BRUSHMODEL,
    COD_TRAP_TRACE, //g++ conflicts ok lol
    TRAP_TRACE_CAPSULE,
    TRAP_SIGHT_TRACE,
    TRAP_SIGHT_TRACE_CAPSULE,
    TRAP_SIGHT_TRACE_TO_ENTITY,
    TRAP_CM_BOX_TRACE,
    TRAP_CM_CAPSULE_TRACE,
    TRAP_CM_BOX_SIGHT_TRACE,
    TRAP_CM_CAPSULE_SIGHT_TRACE,
    TRAP_LOCATIONAL_TRACE,
    TRAP_POINT_CONTENTS,
    TRAP_IN_PVS,
    TRAP_IN_PVS_IGNORE_PORTALS,
    TRAP_IN_SNAPSHOT,
    TRAP_ADJUST_AREA_PORTAL_STATE,
    TRAP_AREAS_CONNECTED,
    TRAP_LINK_ENTITY,
    TRAP_UNLINK_ENTITY,
    TRAP_ENTITIES_IN_BOX,
    TRAP_ENTITY_CONTACT,
    TRAP_GET_USERCMD,
    TRAP_GET_ENTITY_TOKEN,
    TRAP_FS_GET_FILELIST,
    TRAP_MAP_EXISTS,
    TRAP_REAL_TIME,
    TRAP_SNAP_VECTOR,
    TRAP_ENTITY_CONTACT_CAPSULE,
    TRAP_COM_SOUNDALIAS_STRING,
    TRAP_COM_PICK_SOUNDALIAS,
    TRAP_COM_SOUNDALIAS_INDEX,
    TRAP_SURFACETYPE_FROM_NAME,
    TRAP_SURFACETYPE_TO_NAME,
    TRAP_ADDTESTCLIENT,
    TRAP_GET_ARCHIVED_CLIENTINFO,
    TRAP_ADDDEBUGSTRING,
    TRAP_ADDDEBUGLINE,
    TRAP_SETARCHIVE,
    TRAP_Z_MALLOC_INTERNAL,
    TRAP_Z_FREE_INTERNAL,
    TRAP_XANIM_CREATE_TREE,
    TRAP_XANIM_CREATE_SMALL_TREE,
    TRAP_XANIM_FREE_SMALL_TREE,
    TRAP_XMODEL_EXISTS,
    TRAP_XMODEL_GET,
    TRAP_DOBJ_CREATE,
    TRAP_DOBJ_EXISTS,
    TRAP_SAFE_DOBJ_FREE,
    TRAP_XANIM_GET_ANIMS,
    //not sure about 85 | 0x55
    //could be TRAP_XANIM_GET_ROOT
    TRAP_XANIM_CLEAR_TREE_GOAL_WEIGHTS = 86,
    TRAP_XANIM_CLEAR_GOAL_WEIGHT,
    TRAP_XANIM_CLEAR_TREE_GOAL_WEIGHTS_STRICT,
    TRAP_XANIM_SET_COMPLETE_GOAL_WEIGHT_KNOB,
    TRAP_XANIM_SET_COMPLETE_GOAL_WEIGHT_KNOB_ALL,
    TRAP_XANIM_SET_ANIM_RATE,
    TRAP_XANIM_SET_TIME,
    TRAP_XANIM_SET_GOAL_WEIGHT_KNOB,
    TRAP_XANIM_CLEAR_TREE,
    TRAP_XANIM_HAS_TIME,
    TRAP_XANIM_IS_PRIMITIVE,
    TRAP_XANIM_GET_LENGTH,
    TRAP_XANIM_GET_LENGTH_SECONDS,
    TRAP_XANIM_SET_COMPLETE_GOAL_WEIGHT,
    TRAP_XANIM_SET_GOAL_WEIGHT,
    TRAP_XANIM_CALC_ABS_DELTA,
    TRAP_XANIM_CALC_DELTA,
    TRAP_XANIM_GET_REL_DELTA,
    TRAP_XANIM_GET_ABS_DELTA,
    TRAP_XANIM_IS_LOOPED,
    TRAP_XANIM_NOTETRACK_EXISTS,
    TRAP_XANIM_GET_TIME,
    TRAP_XANIM_GET_WEIGHT,
    TRAP_DOBJ_DUMP_INFO,
    TRAP_DOBJ_CREATE_SKEL_FOR_BONE,
    TRAP_DOBJ_CREATE_SKEL_FOR_BONES,
    TRAP_DOBJ_UPDATE_SERVERTIME,
    TRAP_DOBJ_INIT_SERVERTIME,
    TRAP_DOBJ_GET_HIERARCHY_BITS,
    TRAP_DOBJ_CALC_ANIM,
    TRAP_DOBJ_CALC_SKEL,
    TRAP_XANIM_LOAD_ANIMTREE,
    TRAP_XANIM_SAVE_ANIMTREE,
    TRAP_XANIM_CLONE_ANIMTREE,
    TRAP_DOBJ_NUM_BONES,
    TRAP_DOBJ_GET_BONE_INDEX,
    TRAP_DOBJ_GET_MATRIX_ARRAY,
    TRAP_DOBJ_DISPLAY_ANIM,
    TRAP_XANIM_HAS_FINISHED,
    TRAP_XANIM_GET_NUM_CHILDREN,
    TRAP_XANIM_GET_CHILD_AT,
    TRAP_XMODEL_NUM_BONES,
    TRAP_XMODEL_GET_BONE_NAMES,
    TRAP_DOBJ_GET_ROT_TRANS_ARRAY,
    TRAP_DOBJ_SET_ROT_TRANS_INDEX,
    TRAP_DOBJ_SET_CONTROL_ROT_TRANS_INDEX,
    TRAP_XANIM_GET_ANIM_NAME,
    TRAP_DOBJ_GET_TREE,
    TRAP_XANIM_GET_ANIMTREE_SIZE,
    TRAP_XMODEL_DEBUTRAP_BOXES,
    TRAP_GET_WEAPONINFO_MEMORY,
    TRAP_FREE_WEAPONINFO_MEMORY,
    TRAP_FREE_CLIENT_SCRIPT_PERS,
    TRAP_RESET_ENTITY_PARSE_POINT,


    TRAP_MEMSET = 200,
    TRAP_MEMCPY,
    TRAP_STRNCPY,
    TRAP_SIN,
    TRAP_COS,
    TRAP_ATAN2,
    TRAP_SQRT,
    TRAP_MATRIXMULTIPLY,
    TRAP_ANGLEVECTORS,
    TRAP_PERPENDICULARVECTOR,
    TRAP_FLOOR,
    TRAP_CEIL
    /*
    from
    typedef enum {
	TRAP_MEMSET = 100,
	TRAP_MEMCPY,
	TRAP_STRNCPY,
	TRAP_SIN,
	TRAP_COS,
	TRAP_ATAN2,
	TRAP_SQRT,
	TRAP_MATRIXMULTIPLY,
	TRAP_ANGLEVECTORS,
	TRAP_PERPENDICULARVECTOR,
	TRAP_FLOOR,
	TRAP_CEIL,

	TRAP_TESTPRINTINT,
	TRAP_TESTPRINTFLOAT
    } sharedTraps_t;
    */
} gameImport_t;

#define LEVELTIME (*(int*)&level[488])
//#define g_entities(x) ((gentity_t*)( (int)g_entities + GENTITY_SIZE * x )) //no linking issues since it's a macro <3 - Richard
//#define level.num_entities (*(int*)&level[12])

void set_trap_func_ptr( void );

typedef void (*T_SetBrushModel_t)(gentity_t*);
extern T_SetBrushModel_t T_SetBrushModel;

typedef void (*T_LinkEntity_t)(gentity_t*);
typedef void (*T_UnlinkEntity_t)(gentity_t*);

extern T_UnlinkEntity_t T_UnlinkEntity;
extern T_LinkEntity_t T_LinkEntity;

typedef void (*BG_EvaluateTrajectory_t)(const trajectory_t*, int, vec3_t);
typedef void (*BG_EvaluateTrajectoryDelta_t)(const trajectory_t*, int, vec3_t);

extern BG_EvaluateTrajectory_t BG_EvaluateTrajectory;
extern BG_EvaluateTrajectoryDelta_t BG_EvaluateTrajectoryDelta;

typedef enum {
    GAME_INIT,
	GAME_SHUTDOWN,
	GAME_CLIENT_CONNECT,
	GAME_CLIENT_BEGIN,
	GAME_CLIENT_USERINFO_CHANGED,
	GAME_CLIENT_DISCONNECT,
	GAME_CLIENT_COMMAND,
	GAME_CLIENT_THINK,
	GAME_GET_FOLLOW_PLAYERSTATE,
	GAME_UPDATE_CVARS,
	GAME_RUN_FRAME,
	GAME_CONSOLE_COMMAND
	//dont rlly need rest atm
} gameExport_t;

/*
int __stdcall trap_XAnimGetRoot(int a1, int a2)
{
  int v3; // ST0C_4@1
  int v4; // eax@1

  LOWORD(v3) = 0;
  v4 = trap_XAnimGetAnims(a2);
  HIWORD(v3) = Scr_GetAnimsIndex(v4);
  *(_DWORD *)a1 = v3;
  return a1;
}
*/

typedef void ( *xcommand_t )( void );

void Cmd_AddCommand(const char*,xcommand_t);
typedef void (*_Cmd_AddCommand_t)(const char*, xcommand_t);
typedef void (*Com_Error_t)(int code, const char *fmt, ...);

extern _Cmd_AddCommand_t _Cmd_AddCommand;
extern Com_Error_t Com_Error;

static void __attribute__((visibility ("hidden"))) *xalloc(size_t size) {
	void *p = malloc(size);
	if(!p) {
		Com_Error(0, "EXE_ERR_OUT_OF_MEMORY");
		return NULL;
	}
	return p;
}

void myClientCommand();
void myClientEndFrame(int*);
void myClientUserinfoChanged( int clientNum );
char* myClientConnect(int, int);
void myClientDisconnect(int);
/*
================
CMD
================
*/

DEFINE_F(void, Com_Printf, const char*, ...);

//typedef void (QDECL *Com_Printf_t)(const char*, ...);
typedef void (QDECL *Com_DPrintf_t)(const char*, ...);
#define Com_DPrintf _PREFX
extern Com_DPrintf_t Com_DPrintf;

typedef int (*Cmd_Argc_t)();
typedef char* (*Cmd_Argv_t)(unsigned int);
typedef void (*Cmd_ArgvBuffer_t)(int arg, char *buffer, int bufferLength);
typedef void (*Cmd_TokenizeString_t)(const char*);

extern Cmd_Argc_t Cmd_Argc;
extern Cmd_Argv_t Cmd_Argv;
extern Cmd_ArgvBuffer_t Cmd_ArgvBuffer;
extern Cmd_TokenizeString_t Cmd_TokenizeString;

typedef int (*FS_FileIsInPAK_t)( const char *filename, int *pChecksum );
extern FS_FileIsInPAK_t FS_FileIsInPAK;

#define MAX_STACK   256
#define STACK_MASK  ( MAX_STACK - 1 )

typedef void vm_t;

typedef int (QDECL *VM_Call_t)( int, int callnum, ... );
extern VM_Call_t VM_Call;

extern vm_t *gvm;

/*
================
CVAR
================
*/

typedef cvar_t* (*Cvar_FindVar_t)(const char*);
typedef void (*Cvar_Set_t)(const char*, const char*);
typedef cvar_t* (*Cvar_Get_t)( const char *var_name, const char *var_value, int flags );

extern Cvar_Set_t Cvar_Set;
extern Cvar_Get_t Cvar_Get;

char* Cvar_VariableString(const char*);
char* Cvar_InfoString(int bit);

qboolean COM_BitCheck( const int array[], int bitNum );
void COM_BitSet( int array[], int bitNum );
void COM_BitClear( int array[], int bitNum );

/*
==============
COMMON
==============
*/

int Q_isprint( int c );
int Q_islower( int c );
int Q_isupper( int c );
int Q_isalpha( int c );
int Q_isnumeric( int c );
int Q_isalphanumeric( int c );
int Q_isforfilename( int c );

const char *Q_stristr( const char *s, const char *find);

// portable case insensitive compare
int     Q_stricmp( const char *s1, const char *s2 );
int     Q_strncmp( const char *s1, const char *s2, int n );
int     Q_stricmpn( const char *s1, const char *s2, int n );
char    *Q_strlwr( char *s1 );
char    *Q_strupr( char *s1 );
char    *Q_strrchr( const char* string, int c );

// buffer size safe library replacements
void    Q_strncpyz( char *dest, const char *src, int destsize );
void    Q_strcat( char *dest, int size, const char *src );

void Info_Print( const char *s );
char *Q_CleanStr( char *string );

void QDECL Com_sprintf( char *dest, int size, const char *fmt, ... );

char    * QDECL va( char *format, ... );
char *Info_ValueForKey( const char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );

extern void* gamelib;
extern int base;
extern char (*pml)[140];


bool is_good_string(char* str);

#define g_client_size 8900

extern unsigned char* g_clients;

#ifdef uMYSQL
extern MYSQL *db;
#endif

bool Scr_Continue();

void SV_AddOperatorCommands(void);
void COD_Destructor();

/*
static int GAME(const char* n) {
	//printf("GAME(%s);\n", n);
	if(gamelib!=NULL)
	return (int)dlsym(gamelib, n);
	return 0;
}*/
#define GAME(n) ((gamelib!=NULL)?(int)dlsym(gamelib,n):0)

static char *utrim(char *str) //malloc unsafe trim
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

#define PRINT_RESET (1<<1)
#define PRINT_BOLD (1<<2)
#define PRINT_UNDERLINE (1<<3)
#define PRINT_BLINK (1<<4)
#define PRINT_DEF (1<<5)
#define PRINT_BLINK (1<<6)
#define PRINT_DEF (1<<7)
#define PRINT_ERR (1<<8)
#define PRINT_GOOD (1<<9)
#define PRINT_WARN (1<<10)
#define PRINT_INFO (1<<11)
#define PRINT_DEBUG (1<<12)

typedef enum {
	PC_BLACK,
	PC_RED,
	PC_GREEN,
	PC_YELLOW,
	PC_BLUE,
	PC_MAGENTA,
	PC_CYAN,
	PC_WHITE
} print_colors;

static void cprintf(int color, const char *fmt, ...) {
	va_list va;
	char text[1024] = {0};
	
	extern cvar_t *developer;
	if(color & PRINT_DEBUG && !developer->integer)
		return;
	
	if(color & PRINT_RESET)
		printf("\e[0m");
	if(color & PRINT_BOLD)
		printf("\e[1m");
	if(color & PRINT_UNDERLINE)
		printf("\e[4m");
	if(color & PRINT_BLINK)
		printf("\e[6m");
	if(color & PRINT_DEF)
		printf("\e[30m");
	if(color & PRINT_ERR)
		printf("\e[31m");
	if(color & PRINT_GOOD)
		printf("\e[32m");
	if(color & PRINT_WARN)
		printf("\e[33m");
	if(color & PRINT_INFO)
		printf("\e[34m");
	va_start(va, fmt);
	vsnprintf(text, sizeof(text), fmt, va);
	printf("%s", text);
	printf("\e[0m");
	va_end(va);
}

#endif // SHARED_H
