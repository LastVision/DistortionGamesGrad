#pragma once
#include "Message.h"
struct FinishLevelMessage : public Message
{
	FinishLevelMessage(int aLevelID);
	int myLevelID;
};

inline FinishLevelMessage::FinishLevelMessage(int aLevelID)
	: Message(eMessageType::LEVEL_FINISHED)
	, myLevelID(aLevelID)
{
}

