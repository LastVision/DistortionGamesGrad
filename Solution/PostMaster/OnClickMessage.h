#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	START_LEVEL, 
	RESTART_LEVEL,
	NEXT_LEVEL,
	LEVEL_SELECT,
	GAME_QUIT,
	CREDITS,
	RETURN_TO_GAME,
	OPTIONS,
	OPTIONS_TOGGLE_VIBRATION,
	OPTIONS_TOGGLE_SHADOWS,
	OPTIONS_INCREASE_VOLUME,
	OPTIONS_DECREASE_VOLUME,
	OPTIONS_INCREASE_MUSIC,
	OPTIONS_DECREASE_MUSIC,
	OPTIONS_INCREASE_VOICE,
	OPTIONS_DECREASE_VOICE,
};

struct OnClickMessage : public Message
{
	OnClickMessage(eOnClickEvent anEvent);
	OnClickMessage(eOnClickEvent anEvent, int anID);
	OnClickMessage(eOnClickEvent anEvent, int anID, bool aIsNightmare);

	const eOnClickEvent myEvent;
	const int myID;
	const bool myIsNightmareLevel;
};

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(-1)
	, myIsNightmareLevel(false)
{
}

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent, int anID)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(anID)
	, myIsNightmareLevel(false)
{
}

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent, int anID, bool aIsNightmare)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(anID)
	, myIsNightmareLevel(aIsNightmare)
{
}