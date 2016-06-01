#include "stdafx.h"
#include "Hat.h"


Hat::Hat()
{
}


Hat::~Hat()
{
}


void Hat::Update(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint, float aDeltaTime)
{
	aDeltaTime;

	if (aJoint.IsValid() == true)
	{
		myOrientation = CU::InverseSimple(*aJoint.myBind) * (*aJoint.myJoint) * aEntityOrientation;
	}
	else
	{
		myOrientation = aEntityOrientation;
	}

	//switch(hattype)
	//make awesome
}