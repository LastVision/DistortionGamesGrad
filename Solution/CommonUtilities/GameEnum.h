#pragma once

enum class eGameType : unsigned char
{
	SINGLEPLAYER,
	MULTIPLAYER,
};

enum class eWeaponType
{
	PISTOL,
	SHOTGUN,
	GRENADE_LAUNCHER
};

enum class eMissionType
{
	NONE,
	KILL_X,
	DEFEND,
	EVENT,
};


enum class eActionEventType
{
	LOCK,
	UNLOCK,
	TEXT,
	SPAWN,
	MARKER,
};

enum class eInGameState
{
	LEVEL,
	LOAD_LEVEL,
	SCORE,
};

enum class eServerInGameState
{
	LEVEL_UPDATE,
	LEVEL_COMPLETE,
	LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED,
	LEVEL_LOAD,
	LEVEL_WAIT_FOR_OTHER,
};

enum eOwnerType
{
	NOT_USED = -1,
	ENEMY = 1,
	NEUTRAL = 2
};

enum eEntityType
{
	EMPTY = -1,
	PROP = 1,
	SPIKE = 2,
	SAW_BLADE = 4,
	PLAYER = 8,
	STEAM = 16,
	BOUNCER = 32,
	SPAWN_POINT = 64,
	GOAL_POINT = 128,
	STEAM_VENT = 256,
	_COUNT,
};

enum class eTriggerType : int
{
	EMPTY = -1,
	FINISH,
	HAZARD,
	FORCE,
	
	_COUNT,
};

enum class ePhysics
{
	STATIC,
	DYNAMIC,
	PHANTOM,
	CAPSULE,
	KINEMATIC,
};

enum class eEntityState : unsigned char
{
	IDLE,
	WALK,
	ATTACK,
	DIE,
	_COUNT,
};

enum class ePlayerState : int
{
	PISTOL_IDLE,
	PISTOL_FIRE,
	PISTOL_RELOAD,
	PISTOL_DRAW,
	PISTOL_HOLSTER,
	SHOTGUN_IDLE,
	SHOTGUN_FIRE,
	SHOTGUN_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	GRENADE_LAUNCHER_IDLE,
	GRENADE_LAUNCHER_FIRE,
	GRENADE_LAUNCHER_RELOAD,
	GRENADE_LAUNCHER_DRAW,
	GRENADE_LAUNCHER_HOLSTER,
	_COUNT,
};

enum class eNetRayCastType : int
{
	CLIENT_SHOOT_PISTOL,
	CLIENT_SHOOT_SHOTGUN,
	_COUNT,
};

enum class eComponentType
{
	NOT_USED = -1,
	ANIMATION,
	GRAPHICS,
	TRIGGER,
	INPUT,
	MOVEMENT,
	PHYSICS,
	SOUND,
	SAW_BLADE,
	SPIKE,
	STEAM,
	PLAYER_GRAPHICS,
	_COUNT,
};

#undef ERROR
enum class eHistoryType
{
	ERROR,
	HISTORY,
	HELP,
	GENERATED_COMMAND,
	WARNING
};

enum class eFuzzyAI
{
	SPAWN_GRUNT,
	SPAWN_RANGER,
	SPAWN_TANK,
	TAKE_RESOURCE_POINT,
	TAKE_VICTORY_POINT,
	TAKE_ARTIFACT,
	UPGRADE_GRUNT,
	UPGRADE_RANGER,
	UPGRADE_TANK,
	ROAM,
	DO_NOTHING,
	_COUNT,
};

enum class eCursorType
{
	NORMAL
};

enum class eDifficulty
{
	EASY,
	NORMAL,
	HARD
};