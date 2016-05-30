#include "stdafx.h"
#include "Hat.h"
#include <ModelLoader.h>
#include <Instance.h>
#include "HatManager.h"
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

bool BodyPart::GetActive() const
{
	return myInstance->GetShouldRender();
}

Head::Head()
	: myHat(nullptr)
{

}

Head::~Head()
{
	SAFE_DELETE(myInstance);
	if (myHat != nullptr)
	{
		SAFE_DELETE(myHat->myInstance);
		SAFE_DELETE(myHat);
	}
}

void Head::CreateJoints(const std::string& aAnimationPath)
{
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "hat_jnt-1", myHatJoint);
}

void Head::UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint, float aDeltaTime)
{
	myInstance->Update(aDeltaTime);
	if (aJoint.IsValid() == true)
	{
		myOrientation = CU::InverseSimple(*aJoint.myBind) * (*aJoint.myJoint) * aEntityOrientation;
	}
	else
	{
		myOrientation = aEntityOrientation;
	}
	if (myHat != nullptr)
	{
		myHat->Update(myOrientation, myHatJoint, aDeltaTime);
	}
}

void Head::SetHat(int aHatID)
{
	myHat = new Hat();
	myHat->myInstance = new Prism::Instance(*HatManager::GetInstance()->GetHat(aHatID), myHat->myOrientation);
}

void Head::SetActive(bool aValue)
{
	myInstance->SetShouldRender(aValue);
	if (myHat != nullptr)
	{
		myHat->myInstance->SetShouldRender(aValue);
	}
}

bool Head::GetActive() const
{
	return myInstance->GetShouldRender();
}

BodyAnimation::~BodyAnimation()
{
	SAFE_DELETE(myAnimation);
}

void BodyAnimation::CreateAnimation(const std::string& aAnimationPath, const std::string& aShaderPath
	, const CU::Matrix44<float>& aOrientation)
{
	myAnimation = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModelAnimated(aAnimationPath, aShaderPath), aOrientation);
	SetActive(false);
}

void BodyAnimation::CreateJoints(const std::string& aAnimationPath)
{
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "root_jnt-1", myBody);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "head_jnt0", myHead);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "l_leg_jnt0", myLeftLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "r_leg_jnt0", myRightLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(aAnimationPath, "jetpack_jnt0", myJetPack);
}

void BodyAnimation::SetActive(bool aValue)
{
	myAnimation->SetShouldRender(aValue);
}

bool BodyAnimation::GetActive() const
{
	return myAnimation->GetShouldRender();
}
