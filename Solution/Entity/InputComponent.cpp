#include "stdafx.h"
#include <ControllerInput.h>
#include <EmitterMessage.h>
#include "InputComponent.h"
#include "MovementComponent.h"
#include <PostMaster.h>
#include <PlayerActiveMessage.h>
#include "PlayerGraphicsComponent.h"
#include <OnDeathMessage.h>
#include <OnPlayerLevelComplete.h>

#include <ScrapMessage.h>
#include "SpawnNote.h"
#include <OnPlayerJoin.h>
#include "ShouldDieNote.h"

#include "VibrationNote.h"
#include <GameConstants.h>
#include <ModelLoader.h>

#include "LoseBodyPartNote.h"

InputComponent::InputComponent(Entity& aEntity, const InputComponentData& aInputData, CU::Matrix44<float>& aOrientation)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myMovement(nullptr)
	, myOrientation(aOrientation)
	, myIsFlipped(false)
	, myHasCompletedLevel(false)
	, myTimeToSpawn(0.f)
	, myIntendToSpawn(false)
	, myAllowedToSpawn(true)
	, myStandingOnStomper(nullptr)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_PLAYER_LEVEL_COMPLETE | eMessageType::PLAYER_ACTIVE);
}

InputComponent::~InputComponent()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	SAFE_DELETE(myController);
}

void InputComponent::Init()
{
	myMovement = myEntity.GetComponent<MovementComponent>();
	DL_ASSERT_EXP(myMovement != nullptr, "Input component needs movement component to work correctly.");


}

void InputComponent::AddController(int anID)
{
	DL_ASSERT_EXP(myController == nullptr, "Controller was not null. Don't add twice.");
	myController = new CU::ControllerInput(anID);
}

void InputComponent::Reset()
{
	myIsFlipped = false;
}

void InputComponent::Update(float aDeltaTime)
{
	myTimeToSpawn -= aDeltaTime;
	myController->Update(aDeltaTime);

	if (myAllowedToSpawn == true && myIntendToSpawn == true && myTimeToSpawn < 0.f)
	{
		if (myIsInLevel == false)
		{
			myIsInLevel = true;
			PostMaster::GetInstance()->SendMessage(OnPlayerJoin());
		}
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Spawn", myOrientation.GetPos(), myOrientation.GetUp(), 2.f));
		myIntendToSpawn = false;
		myEntity.SendNote(SpawnNote());
		myIsActive = true;
		myMovement->Impulse();
	}

	myParticlePoint = &myEntity.GetComponent<PlayerGraphicsComponent>()->GetCurrentAnimation()->myJetPack;
	myParticleOrientation = CU::InverseSimple(*myParticlePoint->myBind) * (*myParticlePoint->myJoint) * myOrientation;
	CU::Vector3f pos = myParticleOrientation.GetPos();
	myParticleOrientation = CU::Matrix44f::CreateRotateAroundZ(CU::Math::DegreeToRad(-25));
	myParticleOrientation.SetPos(pos);
	if (myController->IsConnected() == true)
	{
		if (myHasCompletedLevel == false)
		{
			if (myIsActive == true)
			{
				if (myController->ButtonOnDown(eXboxButton::A))
				{
					myMovement->Impulse();
					PostMaster::GetInstance()->SendMessage(EmitterMessage("Impulse", &myEntity));
				}
				else
				{
				}

				if (myController->ButtonOnDown(eXboxButton::X))
				{
					myEntity.SendNote(ShouldDieNote());
				}

				if (myController->ButtonOnDown(eXboxButton::RTRIGGER))
				{
					myMovement->RightTriggerDown();
				}
				else if (myController->ButtonOnUp(eXboxButton::RTRIGGER))
				{
					myMovement->RightTriggerUp();
				}
#ifndef RELEASE_BUILD
				else if (myController->ButtonOnDown(eXboxButton::Y))
				{
					PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
						, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity(), myPlayerID));

					PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::BODY
						, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity(), myPlayerID));

					PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
						, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity(), myPlayerID));
				}
#endif
				if (myController->ButtonOnDown(eXboxButton::LTRIGGER))
				{
					SetIsFlipped(!myIsFlipped);
				}

				myMovement->SetDirectionTarget(CU::Vector2<float>(myController->LeftThumbstickX(), myController->LeftThumbstickY()));
			}
			else if (myIsActive == false)
			{
				if (myController->ButtonOnDown(eXboxButton::A))
				{


					myIntendToSpawn = true;
				}
			}
		}
	}

	if (myStandingOnStomper != nullptr && myStandingOnStomper->IsStomperMoving() == true)
	{
		myEntity.SendNote(ShouldDieNote());
	}
}

void InputComponent::SetPlayerID(int aPlayerID)
{
	myPlayerID = aPlayerID;
}

int InputComponent::GetPlayerID() const
{
	return myPlayerID;
}

bool InputComponent::GetIsFlipped() const
{
	return myIsFlipped;
}

void InputComponent::SetIsFlipped(bool aIsFlipped)
{
	myIsFlipped = aIsFlipped;

	CU::Vector3<float> pos = myOrientation.GetPos();
	myOrientation.SetPos(CU::Vector3<float>());
	myOrientation = CU::Matrix44<float>::CreateRotateAroundY(static_cast<float>(M_PI)) * myOrientation;
	myOrientation.SetPos(pos);

}

void InputComponent::ReceiveNote(const DeathNote&)
{
	myIsActive = false;
	if (myController->IsConnected() == true && myHasCompletedLevel == false && GC::OptionsUseViberations == true)
	{
		myController->Vibrate(32000, 16000, 0.5f);
	}
}

void InputComponent::ReceiveNote(const VibrationNote& aMessage)
{
	if (myController->IsConnected() == true && GC::OptionsUseViberations == true)
	{
		myController->Vibrate(aMessage.myLeftMotorValue, aMessage.myRightMotorValue, aMessage.myTime);
	}
}

void InputComponent::ReceiveMessage(const OnPlayerLevelComplete& aMessage)
{
	if (myPlayerID == aMessage.myPlayerID)
	{
		myHasCompletedLevel = true;
		myEntity.GetComponent<PlayerGraphicsComponent>()->Reset();
		myOrientation.SetPos({ -15, -15, 0 });
	}
}

void InputComponent::ReceiveMessage(const PlayerActiveMessage& aMessage)
{
	if (myPlayerID != aMessage.myPlayerID)
	{
		myTimeToSpawn = 0.4f;
	}
}

bool InputComponent::GetIsActive()
{
	return myIsActive;
}

void InputComponent::ResetIsInLevel()
{
	myIsInLevel = false;
}

const CU::Matrix44f& InputComponent::GetParticleOrientation()
{
	return myParticleOrientation;
}
