#include "stdafx.h"

#include <algorithm>
#include <AnimationSystem.h>
#include "CharacterAnimationNote.h"
#include <ModelLoader.h>
#include <Instance.h>
#include "LoseBodyPartNote.h"
#include "PlayerGraphicsComponent.h"
#include <PostMaster.h>
#include "PollingStation.h"
#include <PlayerActiveMessage.h>
#include <Scene.h>
#include <ScrapMessage.h>
#include <InputWrapper.h>
#include "InputComponent.h"
#include <OnDeathMessage.h>
#include "VibrationNote.h"

PlayerGraphicsComponent::PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
	, const CU::Matrix44<float>& aEntityOrientation, Prism::Scene* aScene, int aPlayerID)
	: Component(aEntity)
	, myData(aData)
	, myEntityOrientation(aEntityOrientation)
	, myScene(aScene)
	, myPlayerID(aPlayerID)
	, myShowArrow(false)
	, myPreviousAnimation(eCharacterAnimationType::FLY)
	, myCurrentAnimationType(eCharacterAnimationType::FLY)
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
	myDashAimAnimation.CreateAnimation(myData.myDashAimAnimation, myData.myAnimationShader, myEntityOrientation);
	myDashFlyAnimation.CreateAnimation(myData.myDashFlyAnimation, myData.myAnimationShader, myEntityOrientation);

	myArrowOrientation.SetPos(myEntityOrientation.GetPos4() + CU::Vector4f(0.f, 1.5f, 0.f, 0.f));

	std::string body(myData.myBody);
	std::replace(body.begin(), body.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	myBody.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(body, myData.myShader), myBody.myOrientation);

	std::string leftLeg(myData.myLeftLeg);
	std::replace(leftLeg.begin(), leftLeg.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	myLeftLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(leftLeg, myData.myShader), myLeftLeg.myOrientation);

	std::string rightLeg(myData.myRightLeg);
	std::replace(rightLeg.begin(), rightLeg.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	myRightLeg.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(rightLeg, myData.myShader), myRightLeg.myOrientation);

	std::string head(myData.myHead);
	std::replace(head.begin(), head.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	myHead.myInstance = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModel(head, myData.myShader), myHead.myOrientation);

	if (myPlayerID == 1)
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerOneArrow, myData.myShader), myArrowOrientation);
	}
	else
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerTwoArrow, myData.myShader), myArrowOrientation);
	}
	myArrow->SetShouldRender(false);

	while (Prism::ModelLoader::GetInstance()->IsLoading())
		;


	myIdleAnimation.CreateJoints(myData.myIdleAnimation);
	myWalkAnimation.CreateJoints(myData.myWalkAnimation);
	myFlyAnimation.CreateJoints(myData.myFlyAnimation);
	myDashAimAnimation.CreateJoints(myData.myDashAimAnimation);
	myDashFlyAnimation.CreateJoints(myData.myDashFlyAnimation);

	myScene->AddInstance(myBody.myInstance, true);
	myScene->AddInstance(myLeftLeg.myInstance, true);
	myScene->AddInstance(myRightLeg.myInstance, true);
	myScene->AddInstance(myHead.myInstance, true);
	myScene->AddInstance(myArrow, true);

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
	myPreviousAnimation = eCharacterAnimationType::FLY;
	myCurrentAnimationType = eCharacterAnimationType::FLY;
	myCurrentAnimation = &myFlyAnimation;
	//myArrow->SetShouldRender(true);
}


void PlayerGraphicsComponent::Update(float aDeltaTime)
{
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

	if (PollingStation::GetInstance()->GetPlayersAlive() > 1 && myShowArrow == false)
	{
		myShowArrow = true;
		myArrow->SetShouldRender(true);
		myLerping = 0.f;
	}
	else if (PollingStation::GetInstance()->GetPlayersAlive() < 2 && myShowArrow == true)
	{
		myShowArrow = false;
		
		myLerping = 0.f;
	}

	myLerping += aDeltaTime * 4.f;

	float scale = 0.f;
	if (myShowArrow == true)
	{
		scale = CU::Math::Lerp(0.f, 1.f, myLerping);
		scale = min(1.f, scale); 
	}
	else
	{
		scale = CU::Math::Lerp(1.f, 0.f, myLerping);
		scale = max(0.f, scale);
	}

	if (myShowArrow == false && scale <= 0.f)
	{
		myArrow->SetShouldRender(false);
	}


	myArrow->SetScale(CU::Vector3<float>(scale, scale, scale));
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
				, myEntity.GetOrientation().GetPos(), CU::Vector2<float>(), myEntity.GetComponent<InputComponent>()->GetPlayerID()));
			myEntity.SendNote(VibrationNote(16000, 16000, 0.3f));
		}
		myHead.SetActive(false);
		break;
	case eScrapPart::LEGS:
		if (myLeftLeg.GetActive() == true)
		{
			PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
				, myEntity.GetOrientation().GetPos(), CU::Vector2<float>(), myEntity.GetComponent<InputComponent>()->GetPlayerID()));
			myLeftLeg.SetActive(false);
			myEntity.SendNote(VibrationNote(16000, 16000, 0.3f));
		}

		if (myRightLeg.GetActive() == true)
		{
			PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
				, myEntity.GetOrientation().GetPos(), CU::Vector2<float>(), myEntity.GetComponent<InputComponent>()->GetPlayerID()));
			myRightLeg.SetActive(false);
			myEntity.SendNote(VibrationNote(16000, 16000, 0.3f));
		}
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


void PlayerGraphicsComponent::ReceiveNote(const CharacterAnimationNote& aMessage)
{
	//if (myPreviousAnimation != aMessage.myAnimationType)
	//{
		myPreviousAnimation = myCurrentAnimationType;

		switch (aMessage.myAnimationType)
		{
		case eCharacterAnimationType::IDLE:
			myCurrentAnimation = &myIdleAnimation;
			break;
		case eCharacterAnimationType::WALK:
			myCurrentAnimation = &myWalkAnimation;
			break;
		case eCharacterAnimationType::FLY:
			myCurrentAnimation = &myFlyAnimation;
			break;
		case eCharacterAnimationType::DASH_AIM:
			myCurrentAnimation = &myDashAimAnimation;
			break;
		case eCharacterAnimationType::DASH_FLY:
			myCurrentAnimation = &myDashFlyAnimation;
			break;
		}

		myCurrentAnimationType = aMessage.myAnimationType;
	//}
}

BodyAnimation* PlayerGraphicsComponent::GetCurrentAnimation()
{
	return myCurrentAnimation;
}
