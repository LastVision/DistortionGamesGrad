#include "stdafx.h"
#include "Movement.h"


Movement::Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation)
	: myData(aData)
	, myOrientation(anOrientation)
{
}


Movement::~Movement()
{
}
