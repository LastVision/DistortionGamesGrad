#pragma once
#include "GrowingArray.h"


namespace Prism
{
	class Camera;
}

class SmartCamera
{
public:
	SmartCamera();
	~SmartCamera();

	void Update(float aDeltaTime);


private:

	CU::Matrix44f myOrientation;
	Prism::Camera* myCamera;



};

