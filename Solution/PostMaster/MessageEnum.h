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
	SCRAP = 256,
	_POSTMASTER_COUNT,
};