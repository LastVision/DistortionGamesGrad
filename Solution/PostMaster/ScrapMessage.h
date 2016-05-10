#pragma once
#include "Message.h"
#include <Vector3.h>
#include "GameEnum.h"

struct ScrapMessage : public Message
{
	ScrapMessage(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity);
	

	const eScrapPart myScrapPart;
	const CU::Vector3<float> myPosition;
	const CU::Vector2<float> myVelocity;
};

inline ScrapMessage::ScrapMessage(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity)
	: Message(eMessageType::SPAWN_SCRAP)
	, myScrapPart(aPart)
	, myPosition(aPosition)
	, myVelocity(aVelocity)
{
}


