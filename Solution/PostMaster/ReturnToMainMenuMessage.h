#pragma once
#include "Message.h"

struct ReturnToMainMenuMessage : public Message
{
	ReturnToMainMenuMessage();
};

inline ReturnToMainMenuMessage::ReturnToMainMenuMessage()
	: Message(eMessageType::RETURN_TO_MAIN_MENU)
{
}