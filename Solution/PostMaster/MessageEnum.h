#pragma once
enum eMessageType : int
{
	GAME_STATE = 1,
	RESIZE = 2,
	ON_CLICK = 4,
	FADE = 8,
	PARTICLE = 16,
	LEVEL_FINISHED = 32,
	PLAYER_ACTIVE = 64,
	ON_DEATH = 128,
	SPAWN_SCRAP = 256,
	ON_PLAYER_LEVEL_COMPLETE = 512,
	ON_PLAYER_JOIN = 1024,
	RETURN_TO_MENU = 2048,
	REACHED_GOAL = 4096,
	UNHOVER_CONTROLLER = 8192,
	NIGHTMARE_IS_LOCKED = 16384,
	_POSTMASTER_COUNT,
};