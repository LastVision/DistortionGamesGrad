#include "stdafx.h"
#include <ModelLoader.h>
#include <Instance.h>
#include "PlayerBody.h"

BodyPart::~BodyPart()
{
	SAFE_DELETE(myInstance);
}

void BodyPart::UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint)
{
	if (aJoint.IsValid() == true)
	{
		myOrientation = CU::InverseSimple(*aJoint.myBind) * (*aJoint.myJoint) * aEntityOrientation;
	}
	else
	{
		myOrientation = aEntityOrientation;
	}
}

void BodyPart::SetActive(bool aValue)
{
	myInstance->SetShouldRender(aValue);
}

BodyAnimation::~BodyAnimation()
{
	SAFE_DELETE(myAnimation);
}

void BodyAnimation::CreateAnimation(const std::string& aAnimationPath, const std::string& aShaderPath
	, const CU::Matrix44<float>& aOrientation)
{
	myAnimation = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModelAnimated(aAnimationPath, aShaderPath), aOrientation);
}

void BodyAnimation::CreateJoints(const std::string& aAnimationPath)
{
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "root_jnt-1", myBody);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "head_jnt0", myHead);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "l_leg_jnt0", myLeftLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "r_leg_jnt0", myRightLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "jetpack_jnt0", myJetPack);
}