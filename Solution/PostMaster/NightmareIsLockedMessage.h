#pragma once
#include "Message.h"

struct NightmareIsLockedMessage : public Message
{
	NightmareIsLockedMessage();

	CU::Vector2<float> myPosition;
};

inline NightmareIsLockedMessage::NightmareIsLockedMessage()
	: Message(eMessageType::NIGHTMARE_IS_LOCKED)
{
}

