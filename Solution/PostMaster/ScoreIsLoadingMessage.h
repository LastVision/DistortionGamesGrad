#pragma once
#include "Message.h"
struct ScoreIsLoadingMessage : public Message
{
	ScoreIsLoadingMessage(bool aIsLoading);

	const bool myIsLoading;
};

inline ScoreIsLoadingMessage::ScoreIsLoadingMessage(bool aIsLoading)
	: Message(eMessageType::SCORE_IS_LOADING)
	, myIsLoading(aIsLoading)
{
}