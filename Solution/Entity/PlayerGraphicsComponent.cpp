#include "stdafx.h"

#include <AnimationSystem.h>
#include <ModelLoader.h>
#include <Instance.h>
#include "PlayerGraphicsComponent.h"
#include <Scene.h>

PlayerGraphicsComponent::BodyPart::~BodyPart()
{
	SAFE_DELETE(myInstance);
}

void PlayerGraphicsComponent::BodyPart::UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint)
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


PlayerGraphicsComponent::PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
		, const CU::Matrix44<float>& aEntityOrientation, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aData)
	, myEntityOrientation(aEntityOrientation)
	, myScene(aScene)
{
}


PlayerGraphicsComponent::~PlayerGraphicsComponent()
{
}

void PlayerGraphicsComponent::Init()
{
	myBindPose = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModelAnimated(myData.myIdleAnimation, myData.myAnimationShader)
		, myEntityOrientation);

	myBody.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myBody, myData.myShader), myBody.myOrientation);
	myLeftLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myLeftLeg, myData.myShader), myLeftLeg.myOrientation);
	myRightLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myRightLeg, myData.myShader), myRightLeg.myOrientation);
	myHead.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myHead, myData.myShader), myHead.myOrientation);


	while (Prism::ModelLoader::GetInstance()->IsLoading())
		;


	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myData.myIdleAnimation, "root_jnt-01", myIdleJoints.myBody);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myData.myIdleAnimation, "head_jnt0", myIdleJoints.myHead);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myData.myIdleAnimation, "l_leg_jnt0", myIdleJoints.myLeftLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myData.myIdleAnimation, "r_leg_jnt0", myIdleJoints.myRightLeg);
	Prism::ModelLoader::GetInstance()->GetHierarchyToBone(myData.myIdleAnimation, "jetpack_jnt0", myIdleJoints.myJetPack);

	myScene->AddInstance(myBody.myInstance);
	myScene->AddInstance(myLeftLeg.myInstance);
	myScene->AddInstance(myRightLeg.myInstance);
	myScene->AddInstance(myHead.myInstance);

	myCurrentAnimation = myBindPose;
	myCurrentAnimationJoints = &myIdleJoints;
}

void PlayerGraphicsComponent::Update(float aDeltaTime)
{
	myCurrentAnimation->Update(aDeltaTime);
	myBody.UpdateOrientation(myEntityOrientation, myCurrentAnimationJoints->myBody);
	myLeftLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimationJoints->myLeftLeg);
	myRightLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimationJoints->myRightLeg);
	myHead.UpdateOrientation(myEntityOrientation, myCurrentAnimationJoints->myHead);
}