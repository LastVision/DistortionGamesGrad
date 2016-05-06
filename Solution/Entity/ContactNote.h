#pragma once

class Entity;

struct ContactNote
{
	ContactNote() : myOther(nullptr), myHasEntered(false){}
	ContactNote(Entity* anOther, const CU::Vector3<float>& aContactPoint
		, const CU::Vector3<float>& aContactNormal, bool aHasEntered)
		: myOther(anOther)
		, myContactPoint(aContactPoint)
		, myContactNormal(aContactNormal)
		, myHasEntered(aHasEntered) {}

	//void operator=(const ContactNote&) = delete;



	Entity* myOther;
	CU::Vector3<float> myContactPoint;
	CU::Vector3<float> myContactNormal;
	bool myHasEntered;
};