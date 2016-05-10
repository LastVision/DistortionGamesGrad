#pragma once
#include "Message.h"

struct OnPlayerJoin : public Message
{
	OnPlayerJoin();
};

inline OnPlayerJoin::OnPlayerJoin()
	: Message(eMessageType::ON_PLAYER_JOIN)
{
}