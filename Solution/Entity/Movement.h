#pragma once
class Movement
{
public:
	Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation);
	virtual ~Movement();

	virtual void Reset(){};

	virtual void Update(float aDeltaTime) = 0;
	virtual void ReceiveNote(const ContactNote& aNote) = 0;
	virtual void SetDirectionTarget(const CU::Vector2<float>& aDirection) = 0;
	virtual void Impulse() = 0;


protected:
	const MovementComponentData& myData;
	CU::Matrix44f& myOrientation;
	CU::Vector2<float> myVelocity;
	CU::Vector2<float> myDirectionTarget;

};