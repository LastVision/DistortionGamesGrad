#pragma once

class Entity;

struct ContactNote
{
	ContactNote(Entity* anOther, bool aHasEntered)
		: myOther(anOther)
		, myHasEntered(aHasEntered) {}

	void operator=(ContactNote&) = delete;


	Entity* myOther;
	bool myHasEntered;
};