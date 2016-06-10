#include "stdafx.h"

#include <AudioInterface.h>
#include <algorithm>
#include <AnimationSystem.h>
#include "CharacterAnimationNote.h"
#include <ModelLoader.h>
#include <Instance.h>
#include "HatManager.h"
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
#include <EmitterMessage.h>
#include "Hat.h"
#include "SoundComponent.h"

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
	, mySpawnCountdown(1)
{
	int hatID = HatManager::GetInstance()->GetHatIDOnPlayer(myPlayerID);
	if (hatID != -1)
	{
		myHead.SetHat(hatID);
	}
}

PlayerGraphicsComponent::~PlayerGraphicsComponent()
{
	SAFE_DELETE(myArrow);
}

void PlayerGraphicsComponent::Init()
{
	std::string idle(myData.myIdleAnimation);
	std::string walk(myData.myWalkAnimation);
	std::string fly(myData.myFlyAnimation);
	std::string dashAim(myData.myDashAimAnimation);
	std::string dashFly(myData.myDashFlyAnimation);
	std::replace(idle.begin(), idle.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(walk.begin(), walk.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(fly.begin(), fly.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(dashAim.begin(), dashAim.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(dashFly.begin(), dashFly.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);

	myIdleAnimation.CreateAnimation(idle, myData.myAnimationShader, myEntityOrientation);
	myWalkAnimation.CreateAnimation(walk, myData.myAnimationShader, myEntityOrientation);
	myFlyAnimation.CreateAnimation(fly, myData.myAnimationShader, myEntityOrientation);
	myDashAimAnimation.CreateAnimation(dashAim, myData.myAnimationShader, myEntityOrientation);
	myDashFlyAnimation.CreateAnimation(dashFly, myData.myAnimationShader, myEntityOrientation);

	myArrowOrientation.SetPos(myEntityOrientation.GetPos4() + CU::Vector4f(0.f, 1.5f, 0.f, 0.f));

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
		*Prism::ModelLoader::GetInstance()->LoadModelAnimated(head, myData.myAnimationShader), myHead.myOrientation);

	if (myPlayerID == 1)
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerOneArrow, myData.myShader), myArrowOrientation);
	}
	else
	{
		myArrow = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myPlayerTwoArrow, myData.myShader), myArrowOrientation);
	}
	myArrow->SetShouldRender(false);


	myScene->AddInstance(myIdleAnimation.myAnimation, true);
	myScene->AddInstance(myWalkAnimation.myAnimation, true);
	myScene->AddInstance(myFlyAnimation.myAnimation, true);
	myScene->AddInstance(myDashAimAnimation.myAnimation, true);
	myScene->AddInstance(myDashFlyAnimation.myAnimation, true);

	myScene->AddInstance(myLeftLeg.myInstance, true);
	myScene->AddInstance(myRightLeg.myInstance, true);
	myScene->AddInstance(myHead.myInstance, true);
	myScene->AddInstance(myArrow, true);

	if (myHead.myHat != nullptr)
	{
		myScene->AddInstance(myHead.myHat->myInstance, true);
	}
	myCurrentAnimation = &myIdleAnimation;
	myCurrentAnimation->SetActive(true);
}

void PlayerGraphicsComponent::Reset()
{
	myLeftLeg.SetActive(false);
	myRightLeg.SetActive(false);
	myHead.SetActive(false);
	myArrow->SetShouldRender(false);
	myCurrentAnimation->SetActive(false);
}

void PlayerGraphicsComponent::Activate()
{
	myPreviousAnimation = eCharacterAnimationType::FLY;
	myCurrentAnimationType = eCharacterAnimationType::FLY;
	myCurrentAnimation = &myFlyAnimation;
	//myArrow->SetShouldRender(true);
	mySpawnCountdown = 1;
}


void PlayerGraphicsComponent::Update(float aDeltaTime)
{
	if (mySpawnCountdown != 0)
	{
		--mySpawnCountdown;
		if (mySpawnCountdown == 0)
		{
			myLeftLeg.SetActive(true);
			myRightLeg.SetActive(true);
			myHead.SetActive(true);
			myCurrentAnimation->SetActive(true);
		}
	}

#ifndef RELEASE_BUILD
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_Q))
	{
		myCurrentAnimation->SetActive(false);
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
#endif

	myCurrentAnimation->myAnimation->Update(aDeltaTime);
	myLeftLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myLeftLeg);
	myRightLeg.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myRightLeg);
	myHead.UpdateOrientation(myEntityOrientation, myCurrentAnimation->myHead, aDeltaTime);

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
		myCurrentAnimation->SetActive(false);
		break;
	case eScrapPart::HEAD:
		if (myHead.GetActive() == true)
		{
			PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
				, myEntity.GetOrientation().GetPos(), CU::Vector2<float>(), myEntity.GetComponent<InputComponent>()->GetPlayerID()));
			myEntity.SendNote(VibrationNote(16000, 16000, 0.3f));
			PostMaster::GetInstance()->SendMessage(EmitterMessage("Drop_Head", myEntity.GetOrientation().GetPos()));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_LooseHead", myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());

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
			PostMaster::GetInstance()->SendMessage(EmitterMessage("Drop_Legs", myEntity.GetOrientation().GetPos()));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_LooseLegs", myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
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

		myCurrentAnimation->SetActive(false);

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

		myCurrentAnimation->SetActive(true);
		myCurrentAnimationType = aMessage.myAnimationType;
	//}
}

BodyAnimation* PlayerGraphicsComponent::GetCurrentAnimation()
{
	return myCurrentAnimation;
}

void PlayerGraphicsComponent::CreateJoints()
{
	std::string idle(myData.myIdleAnimation);
	std::string walk(myData.myWalkAnimation);
	std::string fly(myData.myFlyAnimation);
	std::string dashAim(myData.myDashAimAnimation);
	std::string dashFly(myData.myDashFlyAnimation);
	std::replace(idle.begin(), idle.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(walk.begin(), walk.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(fly.begin(), fly.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(dashAim.begin(), dashAim.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);
	std::replace(dashFly.begin(), dashFly.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);

	std::string head(myData.myHead);
	std::replace(head.begin(), head.end(), '%', CU::Concatenate("%i", myPlayerID)[0]);

	myIdleAnimation.CreateJoints(idle);
	myWalkAnimation.CreateJoints(walk);
	myFlyAnimation.CreateJoints(fly);
	myDashAimAnimation.CreateJoints(dashAim);
	myDashFlyAnimation.CreateJoints(dashFly);
	myHead.CreateJoints(head);
}
