#pragma once

#include <AnimationJoint.h>
#include <Matrix.h>
#include <string>

namespace Prism
{
	class Instance;
}

class Hat;

struct BodyPart
{
	~BodyPart();

	void UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint);

	void SetActive(bool aValue);
	bool GetActive() const;


	Prism::Instance* myInstance;
	CU::Matrix44<float> myOrientation;
};

struct Head
{
	Head();
	~Head();

	void UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint, float aDeltaTime);
	void CreateJoints(const std::string& aAnimationPath);
	void SetActive(bool aValue);
	bool GetActive() const;


	Prism::Instance* myInstance;
	CU::Matrix44<float> myOrientation;

	Hat* myHat;
	AnimationJoint myHatJoint;
};

struct BodyAnimation
{
	~BodyAnimation();
	void CreateAnimation(const std::string& aAnimationPath, const std::string& aShaderPath
		, const CU::Matrix44<float>& aOrientation);
	void CreateJoints(const std::string& aAnimationPath);

	void SetActive(bool aValue);
	bool GetActive() const;

	Prism::Instance* myAnimation;

	AnimationJoint myHead;
	AnimationJoint myBody;
	AnimationJoint myLeftLeg;
	AnimationJoint myRightLeg;
	AnimationJoint myJetPack;
};