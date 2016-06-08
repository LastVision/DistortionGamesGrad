#pragma once
#include "Message.h"
struct SoundMessage : public Message
{
	SoundMessage(bool aShouldPlaySound);

	const bool myShouldPlaySound;
};

inline SoundMessage::SoundMessage(bool aShouldPlaySound)
	: Message(eMessageType::SOUND)
	, myShouldPlaySound(aShouldPlaySound)
{
}