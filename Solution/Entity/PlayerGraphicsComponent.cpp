#include "stdafx.h"

#include <AnimationSystem.h>
#include <ModelLoader.h>
#include <Instance.h>
#include "LoseBodyPartNote.h"
#include "PlayerGraphicsComponent.h"
#include <PostMaster.h>
#include <PlayerActiveMessage.h>
#include <Scene.h>
#include <ScrapMessage.h>
#include <InputWrapper.h>
#include "InputComponent.h"
#include <OnDeathMessage.h>

PlayerGraphicsComponent::PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
	, const CU::Matrix44<float>& aEntityOrientation, Prism::Scene* aScene, int aPlayerID)
	: Component(aEntity)
	, myData(aData)
	, myEntityOrientation(aEntityOrientation)
	, myScene(aScene)
	, myPlayerID(aPlayerID)
{
}


PlayerGraphicsComponent::~PlayerGraphicsComponent()
{
	SAFE_DELETE(myArrow);
}

void PlayerGraphicsComponent::Init()
{
	myIdleAnimation.CreateAnimation(myData.myIdleAnimation, myData.myAnimationShader, myEntityOrientation);
	myWalkAnimation.CreateAnimation(myData.myWalkAnimation, myData.myAnimationShader, myEntityOrientation);
	myFlyAnimation.CreateAnimation(myData.myFlyAnimation, myData.myAnimationShader, myEntityOrientation);

	myArrowOrientation.SetPos(myEntityOrientation.GetPos4() + CU::Vector4f(0.f, 1.5f, 0.f, 0.f));

	myBody.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myBody, myData.myShader), myBody.myOrientation);
	myLeftLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myLeftLeg, myData.myShader), myLeftLeg.myOrientation);
	myRightLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myRightLeg, myData.myShader), myRightLeg.myOrientation);
	myHead.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(myData.myHead, myData.myShader), myHead.myOrientation);

	if (myPlayerID == 1)
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerOneArrow, myData.myShader), myArrowOrientation);
	}
	else
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerTwoArrow, myData.myShader), myArrowOrientation);
	}


	while (Prism::ModelLoader::GetInstance()->IsLoading())
		;


	myIdleAnimation.CreateJoints(myData.myIdleAnimation);
	myWalkAnimation.CreateJoints(myData.myWalkAnimation);
	myFlyAnimation.CreateJoints(myData.myFlyAnimation);

	myScene->AddInstance(myBody.myInstance);
	myScene->AddInstance(myLeftLeg.myInstance);
	myScene->AddInstance(myRightLeg.myInstance);
	myScene->AddInstance(myHead.myInstance);
	myScene->AddInstance(myArrow);

	myCurrentAnimation = &myIdleAnimation;
}

void PlayerGraphicsComponent::Reset()
{
	myBody.SetActive(false);
	myLeftLeg.SetActive(false);
	myRightLeg.SetActive(false);
	myHead.SetActive(false);
	myArrow->SetShouldRender(false);
}

void PlayerGraphicsComponent::Activate()
{
	myBody.SetActive(true);
	myLeftLeg.SetActive(true);
	myRightLeg.SetActive(true);
	myHead.SetActive(true);
	myArrow->SetShouldRender(true);
}


void PlayerGraphicsComponent::Update(float aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1))
	{
		myCurrentAnimation = &myIdleAnimation;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2))
	{
		myCurrentAnimation = &myWalkAnimation;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3))
	{
		myCurrentAnimation = &myFlyAnimation;
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_Q))
	{
		myBody.SetActive(false);
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_W))
	{
		myLeftLeg.SetActive(false);
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_E))
	{
		myRightLeg.SetActive(false);
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_R))
	{
		myHead.SetActive(false);
	}

	myCurrentAnimation->myAnimation->Update(aDeltaTime);
	myBody.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myBody);
	myLeftLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myLeftLeg);
	myRightLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myRightLeg);
	myHead.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myHead);

	myArrowOrientation.SetPos(myEntityOrientation.GetPos4() + CU::Vector4f(0.f, 1.5f, 0.f, 0.f));
}

void PlayerGraphicsComponent::ReceiveNote(const LoseBodyPartNote& aMessage)
{
	switch (aMessage.myBodyPart)
	{
	case eScrapPart::BODY:
		myBody.SetActive(false);
		break;
	case eScrapPart::HEAD:
		if (myHead.GetActive() == true)
		{
			PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
				, myEntity.GetOrientation().GetPos(), CU::Vector2<float>()));
		}
		myHead.SetActive(false);
		break;
	case eScrapPart::LEGS:
		myRightLeg.SetActive(false);
		myLeftLeg.SetActive(false);
		break;
	default:
		DL_ASSERT("Unknown body part, see a doctor.");
		break;
	}
}

void PlayerGraphicsComponent::ReceiveNote(const SpawnNote&)
{
	Activate();
}

void PlayerGraphicsComponent::ReceiveNote(const DeathNote&)
{
	Reset();
}
