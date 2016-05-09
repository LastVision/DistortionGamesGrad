#pragma once
#include "Message.h"
struct OnDeathMessage : public Message
{
	OnDeathMessage(int aPlayerID);
	int myPlayerID;
};

inline OnDeathMessage::OnDeathMessage(int aPlayerID)
	: Message(eMessageType::ON_DEATH)
	, myPlayerID(aPlayerID)
{
}


