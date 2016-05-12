#pragma once
#include "Message.h"

struct ReturnToMenuMessage : public Message
{
	ReturnToMenuMessage();
};

inline ReturnToMenuMessage::ReturnToMenuMessage()
	: Message(eMessageType::RETURN_TO_MENU)
{
}