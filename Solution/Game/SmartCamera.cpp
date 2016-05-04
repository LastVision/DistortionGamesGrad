#include "stdafx.h"
#include "SmartCamera.h"
#include <Camera.h>

SmartCamera::SmartCamera(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myPlayerOrientations(2)
{
}


SmartCamera::~SmartCamera()
{
}

void SmartCamera::Update(float aDeltaTime)
{
	myCamera.Update(aDeltaTime);
	myCamera.SetPosition({ myPlayerOrientations[0]->GetPos().x, myPlayerOrientations[0]->GetPos().y, myCamera.GetOrientation().GetPos().z });
}

void SmartCamera::AddOrientation(const CU::Matrix44f* aPlayerOrientation)
{
	myPlayerOrientations.Add(aPlayerOrientation);
}
