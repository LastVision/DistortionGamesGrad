#pragma once
#include "GrowingArray.h"


namespace Prism
{
	class Camera;
}

class SmartCamera
{
public:
	SmartCamera(Prism::Camera& aCamera);
	~SmartCamera();

	void Update(float aDeltaTime);
	void AddOrientation(const CU::Matrix44f* aPlayerOrientation);

private:
	void operator=(SmartCamera&) = delete;
	CU::Matrix44f myOrientation;
	Prism::Camera& myCamera;
	CU::GrowingArray<const CU::Matrix44f*> myPlayerOrientations;
	float myMinZoom;
	float myMaxZoom;

};

