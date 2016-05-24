#pragma once
#include <GameEnum.h>

struct CharacterAnimationNote
{
	CharacterAnimationNote(eCharacterAnimationType anAnimationType)
		: myAnimationType(anAnimationType) {}

	eCharacterAnimationType myAnimationType;
};