#pragma once
#include "Message.h"
struct PlayerActiveMessage : public Message
{
	PlayerActiveMessage(bool aPlayerIsActive, int aPlayerID);
	bool myPlayerIsActive;
	int myPlayerID;
};

inline PlayerActiveMessage::PlayerActiveMessage(bool aPlayerIsActive, int aPlayerID)
	: Message(eMessageType::PLAYER_ACTIVE)
	, myPlayerIsActive(aPlayerIsActive)
	, myPlayerID(aPlayerID)
{
}


