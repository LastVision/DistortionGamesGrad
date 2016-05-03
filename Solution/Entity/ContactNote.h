#pragma once

class Entity;

struct ContactNote
{
	ContactNote(Entity* anOther, const CU::Vector3<float>& aContactPoint, bool aHasEntered)
		: myOther(anOther)
		, myContactPoint(aContactPoint)
		, myHasEntered(aHasEntered) {}

	void operator=(ContactNote&) = delete;


	Entity* myOther;
	CU::Vector3<float> myContactPoint;
	bool myHasEntered;
};