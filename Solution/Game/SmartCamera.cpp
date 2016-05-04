#include "stdafx.h"
#include "SmartCamera.h"
#include <Camera.h>
#include <XMLReader.h>
#include <CommonHelper.h>
SmartCamera::SmartCamera(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myPlayerOrientations(2)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/Smart_Camera.xml");

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	reader.ForceReadAttribute(reader.FindFirstChild(rootElement, "minZoom"), "value", myMinZoom);
	reader.CloseDocument();

	myMinZoom = -myMinZoom;
	myMaxZoom = -50;
	myCamera.SetPosition(CU::Vector3f(0, 0, myMinZoom));

}


SmartCamera::~SmartCamera()
{
}

void SmartCamera::Update(float aDeltaTime)
{
	myCamera.Update(aDeltaTime);
	
	if (myPlayerOrientations.Size() > 1)
	{
		CU::Vector3<float> position;
		position.x = (myPlayerOrientations[0]->GetPos().x + myPlayerOrientations[1]->GetPos().x) * 0.5f;
		position.y = (myPlayerOrientations[0]->GetPos().y + myPlayerOrientations[1]->GetPos().y) * 0.5f;
		position.z = -CU::Length(myPlayerOrientations[0]->GetPos() - myPlayerOrientations[1]->GetPos()) * 1.2f;
		/*CU::Vector3f position;
		position.x = abs((myPlayerOrientations[1]->GetPos().x - myPlayerOrientations[0]->GetPos().x) * 0.5f);
		position.y = abs((myPlayerOrientations[1]->GetPos().y - myPlayerOrientations[0]->GetPos().y) * 0.5f);*/
		//position.z -= abs((position.x + position.x) - (position.y + position.y));

		if (position.z < myMinZoom)
		{
			/*if (position.z > myMaxZoom)
			{*/
				myCamera.SetPosition(position);
				DEBUG_PRINT(position);
			//}
		/*	else
			{
				myCamera.SetPosition(CU::Vector3f(position.x, position.y, myMaxZoom));
			}*/
		}
		else
		{
			myCamera.SetPosition(CU::Vector3f(position.x, position.y, myMinZoom));
		}
	}
	else
	{
		myCamera.SetPosition({ myPlayerOrientations[0]->GetPos().x, myPlayerOrientations[0]->GetPos().y, myCamera.GetOrientation().GetPos().z });
	}




}

void SmartCamera::AddOrientation(const CU::Matrix44f* aPlayerOrientation)
{
	myPlayerOrientations.Add(aPlayerOrientation);
}
