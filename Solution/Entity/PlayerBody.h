#pragma once

#include <AnimationJoint.h>
#include <Matrix.h>
#include <string>

namespace Prism
{
	class Instance;
}

struct BodyPart
{
	~BodyPart();

	void UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint);

	void SetActive(bool aValue);

	Prism::Instance* myInstance;
	CU::Matrix44<float> myOrientation;
};

struct BodyAnimation
{
	~BodyAnimation();
	void CreateAnimation(const std::string& aAnimationPath, const std::string& aShaderPath
		, const CU::Matrix44<float>& aOrientation);
	void CreateJoints(const std::string& aAnimationPath);

	Prism::Instance* myAnimation;

	AnimationJoint myHead;
	AnimationJoint myBody;
	AnimationJoint myLeftLeg;
	AnimationJoint myRightLeg;
	AnimationJoint myJetPack;
};