#include "stdafx.h"
#include "Hat.h"


Hat::Hat()
	: myID(-1)
{
}


Hat::~Hat()
{
	SAFE_DELETE(myInstance);
}


void Hat::Update(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint, float aDeltaTime)
{
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