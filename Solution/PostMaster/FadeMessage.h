#pragma once
#include "Message.h"
struct FadeMessage : public Message
{
	FadeMessage(float aFadeTime);
	
	const float myFadeTime;
};

inline FadeMessage::FadeMessage(float aFadeTime)
	: Message(eMessageType::FADE)
	, myFadeTime(aFadeTime)
{
}