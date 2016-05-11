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
	, myCameraAlpha(0)
{
	myShouldResetCameraPos = true;
	XMLReader reader;
	reader.OpenDocument("Data/Setting/Smart_Camera.xml");

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	reader.ForceReadAttribute(reader.FindFirstChild(rootElement, "minZoom"), "value", myMinZoom);
	reader.CloseDocument();

	myMinZoom = -myMinZoom;
	myMaxZoom = -50;
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
	myCamera.Update(aDeltaTime);
	if (myActivePlayerCount > 1)
	{
		CU::Vector3<float> position;
		position.x = (myPlayerOrientations[0]->GetPos().x + myPlayerOrientations[1]->GetPos().x) * 0.5f;
		position.y = (myPlayerOrientations[0]->GetPos().y + myPlayerOrientations[1]->GetPos().y) * 0.5f;
		position.z = -CU::Length(myPlayerOrientations[0]->GetPos() - myPlayerOrientations[1]->GetPos()) * 1.2f;
		if (position.z < myMinZoom)
		{
			myCamera.SetPosition(position);
			DEBUG_PRINT(position);
		}
		else
		{
			myCamera.SetPosition(CU::Vector3f(position.x, position.y, myMinZoom));
		}
	}
	else if (myActivePlayerCount < 2 && myActivePlayerCount > 0)
	{
		if (myActivePlayers[PLAYER_1] == TRUE)
		{
			myCamera.SetPosition({ myPlayerOrientations[0]->GetPos().x, myPlayerOrientations[0]->GetPos().y, myCamera.GetOrientation().GetPos().z });
		}
		else if (myActivePlayers[PLAYER_2] == TRUE)
		{
			myCamera.SetPosition({ myPlayerOrientations[1]->GetPos().x, myPlayerOrientations[1]->GetPos().y, myCamera.GetOrientation().GetPos().z });
		}
	}
	else
	{
		myCameraAlpha += aDeltaTime * 4;

		if (myCameraAlpha <= 1.f)
		{
			CU::Vector3f &cameraPos = myCamera.GetOrientation().GetPos();
			CU::Vector3f pos = CU::Math::Lerp(cameraPos, myStartPosition, myCameraAlpha);
			myCamera.SetPosition(pos);
			myCameraAlpha = 0.f;
		}
		/*else
		{
		myCamera.SetPosition(myStartPosition);
		}*/
	}

}

void SmartCamera::AddOrientation(const CU::Matrix44f* aPlayerOrientation)
{
	myPlayerOrientations.Add(aPlayerOrientation);
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


int SmartCamera::GetPlayerCount()
{
	return myPlayerOrientations.Size();
}

int SmartCamera::GetActivePlayerCount()
{
	return myActivePlayerCount;
}
