#pragma once
#include "AnimationJoint.h"
namespace Prism
{
	class Instance;
}

class Hat
{
public:
	Hat();
	~Hat();

	void Update(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint, float aDeltaTime);

	Prism::Instance* myInstance;
	CU::Matrix44<float> myOrientation;

};

