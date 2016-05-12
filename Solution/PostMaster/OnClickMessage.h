#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	START_LEVEL, 
	LEVEL_SELECT,
	GAME_QUIT,
};

struct OnClickMessage : public Message
{
	OnClickMessage(eOnClickEvent anEvent);
	OnClickMessage(eOnClickEvent anEvent, int anID);

	const eOnClickEvent myEvent;
	const int myID;
};

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(-1)
{
}

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent, int anID)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(anID)
{

}