#pragma once
#include "Message.h"

struct UnhoverControllerMessage : public Message
{
	UnhoverControllerMessage();
};

inline UnhoverControllerMessage::UnhoverControllerMessage()
	: Message(eMessageType::UNHOVER_CONTROLLER)
{
}

