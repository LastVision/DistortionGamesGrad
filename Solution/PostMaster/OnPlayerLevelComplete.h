#pragma once
#include "Message.h"
struct OnPlayerLevelComplete : public Message
{
	OnPlayerLevelComplete(int aPlayerID);
	int myPlayerID;
};

inline OnPlayerLevelComplete::OnPlayerLevelComplete(int aPlayerID)
	: Message(eMessageType::ON_PLAYER_LEVEL_COMPLETE)
	, myPlayerID(aPlayerID)
{

}

