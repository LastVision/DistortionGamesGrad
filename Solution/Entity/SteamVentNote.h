#pragma once

struct SteamVentNote
{
	SteamVentNote(bool aIsActivate)
		: myIsActivate(aIsActivate)
	{}

	void operator=(SteamVentNote&) = delete;

	const bool myIsActivate;
};