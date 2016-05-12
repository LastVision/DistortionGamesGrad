#pragma once
#include <GameEnum.h>

struct LoseBodyPartNote
{
	LoseBodyPartNote(eScrapPart aBodyPart)
		: myBodyPart(aBodyPart)
	{}
	
	void operator=(const LoseBodyPartNote&) = delete;
	
	const eScrapPart myBodyPart;
};