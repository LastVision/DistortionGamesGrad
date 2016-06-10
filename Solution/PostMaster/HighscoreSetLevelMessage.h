#pragma once
#include "Message.h"
struct HighscoreSetLevelMessage : public Message
{
	HighscoreSetLevelMessage(int aLevelID);
	int myLevelID;
};

inline HighscoreSetLevelMessage::HighscoreSetLevelMessage(int aLevelID)
	: Message(eMessageType::HIGHSCORE_SET_LEVEL)
	, myLevelID(aLevelID)
{
}