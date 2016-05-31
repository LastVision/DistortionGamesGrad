#include "stdafx.h"
#include "SmartCamera.h"
#include <Camera.h>
#include <XMLReader.h>
#include <CommonHelper.h>
#include <PostMaster.h>
#include <PlayerActiveMessage.h>
#include <OnDeathMessage.h>
#include <OnPlayerLevelComplete.h>
#include <MathHelper.h>
SmartCamera::SmartCamera(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myPlayerOrientations(2)
	, myPlayerVelocities(2)
	, myCameraAlpha(0)
{
	myShouldResetCameraPos = true;
	XMLReader reader;
	reader.OpenDocument("Data/Setting/Smart_Camera.xml");

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "minZoom"), "value", myMinZoom);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "maxZoom"), "value", myMaxZoom);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "responsiveness"), "value", myResponsiveness);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "velocityMultiplier"), "x", myVelocityMultiplier.x);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "velocityMultiplier"), "y", myVelocityMultiplier.y);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "2PlayersMoveBackMultiplier"), "value", my2PlayersMoveBackMultiplier);

	reader.CloseDocument();

	myMinZoom = -myMinZoom;
	myMaxZoom = -myMaxZoom;
	myCamera.SetPosition(CU::Vector3f(0, 0, myMinZoom));
	PostMaster::GetInstance()->Subscribe(this, eMessageType::PLAYER_ACTIVE | eMessageType::ON_DEATH | eMessageType::ON_PLAYER_LEVEL_COMPLETE);

	myActivePlayers.reset();
}


SmartCamera::~SmartCamera()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void SmartCamera::Update(float aDeltaTime)
{
	DEBUG_PRINT(myActivePlayerCount);
	myCamera.Update(aDeltaTime);
	if (myActivePlayerCount == 2)
	{
		CU::Vector3<float> position((CalcTargetPosition(0) + CalcTargetPosition(1)) * 0.5f);
		position.z = -CU::Length(CalcTargetPosition(0) - CalcTargetPosition(1)) * my2PlayersMoveBackMultiplier;
		if (position.z < myMinZoom)
		{

			if (position.z < myMaxZoom)
			{
				position.z = myMaxZoom;
			}

			myCameraAlpha += aDeltaTime * myResponsiveness;
			if (myCameraAlpha <= 1.f)
			{

				myCamera.SetPosition(CU::Math::Lerp(myCamera.GetOrientation().GetPos(), position, myCameraAlpha));
				myCameraAlpha = 0.f;
			}
			DEBUG_PRINT(position);
		}
		else
		{
			myCameraAlpha += aDeltaTime * myResponsiveness;
			position.z = myMinZoom;
			myCamera.SetPosition(CU::Math::Lerp(myCamera.GetOrientation().GetPos(), position, myCameraAlpha));
			//myCamera.SetPosition(CU::Vector3f(position.x, position.y, myMinZoom));
			myCameraAlpha = 0.f;
		}
	}
	else if (myActivePlayerCount == 1)
	{
		myCameraAlpha += aDeltaTime * myResponsiveness;
		if (myActivePlayers[PLAYER_1] == TRUE)
		{
			if (myCameraAlpha <= 1.f)
			{
				CU::Vector3f targetPos(CalcTargetPosition(0));
				targetPos.z = myMinZoom;
				myCamera.SetPosition(CU::Math::Lerp(myCamera.GetOrientation().GetPos(), targetPos, myCameraAlpha));
				myCameraAlpha = 0.f;
			}
			else
			{
				myCamera.SetPosition({ myPlayerOrientations[0]->GetPos().x, myPlayerOrientations[0]->GetPos().y, myMinZoom });
			}
		}
		else if (myActivePlayers[PLAYER_2] == TRUE)
		{

			if (myCameraAlpha <= 1.f)
			{
				CU::Vector3f targetPos(CalcTargetPosition(1));
				targetPos.z = myMinZoom;
				myCamera.SetPosition(CU::Math::Lerp(myCamera.GetOrientation().GetPos(), targetPos, myCameraAlpha));
				myCameraAlpha = 0.f;
			}
			else
			{
				myCamera.SetPosition({ myPlayerOrientations[1]->GetPos().x, myPlayerOrientations[1]->GetPos().y, myMinZoom });
			}
		}
	}
	else
	{
		CU::Vector3f cameraPos = myCamera.GetOrientation().GetPos();
		myCamera.SetPosition(cameraPos);
	}

}

void SmartCamera::AddPlayer(const CU::Matrix44f* aPlayerOrientation, const CU::Vector2<float>* aPlayerVelocity)
{
	myPlayerOrientations.Add(aPlayerOrientation);
	myPlayerVelocities.Add(aPlayerVelocity);
}

void SmartCamera::SetActivePlayerCount(int aPlayerCount)
{
	myActivePlayerCount = aPlayerCount;
}

void SmartCamera::SetStartPosition(const CU::Vector3f& aPosition)
{
	CU::Vector3f pos(aPosition);
	pos.z = myMinZoom;
	myOrientation.SetPos(pos);
	myCamera.SetPosition(pos);
	myStartPosition = pos;
}

void SmartCamera::ReceiveMessage(const PlayerActiveMessage& aMessage)
{
	if (aMessage.myPlayerID == PLAYER_1)
	{
		myActivePlayers[PLAYER_1] = TRUE;
	}
	else if (aMessage.myPlayerID == PLAYER_2)
	{
		myActivePlayers[PLAYER_2] = TRUE;
	}

	myActivePlayerCount++;
}

void SmartCamera::ReceiveMessage(const OnDeathMessage& aMessage)
{
	if (aMessage.myPlayerID == PLAYER_1 && myActivePlayers[PLAYER_1] == TRUE)
	{
		myActivePlayers[PLAYER_1] = FALSE;
		myActivePlayerCount--;
	}
	else if (aMessage.myPlayerID == PLAYER_2 && myActivePlayers[PLAYER_2] == TRUE)
	{
		myActivePlayers[PLAYER_2] = FALSE;
		myActivePlayerCount--;
	}

	DL_ASSERT_EXP(myActivePlayerCount >= 0, "Player count less than 0.");
}

void SmartCamera::ReceiveMessage(const OnPlayerLevelComplete& aMessage)
{
	if (aMessage.myPlayerID == PLAYER_1 && myActivePlayers[PLAYER_1] == TRUE)
	{
		myActivePlayers[PLAYER_1] = FALSE;
		myActivePlayerCount--;
	}
	else if (aMessage.myPlayerID == PLAYER_2 && myActivePlayers[PLAYER_2] == TRUE)
	{
		myActivePlayers[PLAYER_2] = FALSE;
		myActivePlayerCount--;
	}
}

const CU::Matrix44<float>& SmartCamera::GetOrientation() const
{
	return myCamera.GetOrientation();
}

int SmartCamera::GetPlayerCount()
{
	return myPlayerOrientations.Size();
}

int SmartCamera::GetActivePlayerCount()
{
	return myActivePlayerCount;
}

CU::Vector3<float> SmartCamera::CalcTargetPosition(int aPlayer) const
{
	return myPlayerOrientations[aPlayer]->GetPos() + CU::Vector3<float>(*myPlayerVelocities[aPlayer], 0) * myVelocityMultiplier;
}