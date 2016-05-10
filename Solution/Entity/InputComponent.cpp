#include "stdafx.h"
#include <ControllerInput.h>
#include "InputComponent.h"
#include "MovementComponent.h"
#include <PostMaster.h>
#include <PlayerActiveMessage.h>
#include "PlayerGraphicsComponent.h"
#include <OnDeathMessage.h>

#include <ScrapMessage.h>


InputComponent::InputComponent(Entity& aEntity, const InputComponentData& aInputData, CU::Matrix44<float>& aOrientation)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myMovement(nullptr)
	, myOrientation(aOrientation)
	, myIsFlipped(false)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_DEATH);

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
	myController = new CU::ControllerInput(anID);
}

void InputComponent::Reset()
{
	myIsFlipped = false;
}

void InputComponent::Update(float aDeltaTime)
{
	myController->Update(aDeltaTime);

	if (myController->IsConnected() == true)
	{
		if (myIsActive == true)
		{
			if (myController->ButtonOnDown(eXboxButton::A))
			{
				myMovement->Impulse();
			}
			else
			{
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
					, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity()));

				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::BODY
					, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity()));

				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
					, myEntity.GetOrientation().GetPos(), myEntity.GetComponent<MovementComponent>()->GetVelocity()));
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
				PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myPlayerID));
				myEntity.Reset();
				myEntity.GetComponent<PlayerGraphicsComponent>()->Activate();
				myIsActive = true;
				myMovement->Impulse();
			}
		}
	}
}

void InputComponent::SetPlayerID(int aPlayerID)
{
	myPlayerID = aPlayerID;
}

int InputComponent::GetPlayerID()
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
	myOrientation = CU::Matrix44<float>::CreateRotateAroundY(M_PI) * myOrientation;
	myOrientation.SetPos(pos);

}

void InputComponent::ReceiveMessage(const OnDeathMessage& aMessage)
{
	if (myPlayerID == aMessage.myPlayerID)
	{
		myIsActive = false;
	}
}
