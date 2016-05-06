#pragma once

class MovementComponent;
class Movement
{
public:
	Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	virtual ~Movement();

	virtual void Reset(){};

	virtual void Update(float aDeltaTime) = 0;
	virtual void ReceiveNote(const ContactNote& aNote) = 0;
	virtual void SetDirectionTarget(const CU::Vector2<float>& aDirection) = 0;
	virtual void Impulse() = 0;
	virtual void Impulse(const CU::Vector2<float>& aVelocity) = 0;
	virtual void Activate() = 0;
	virtual void DeActivate() = 0;
	virtual void SetVelocity(const CU::Vector2<float>& aVelocity) = 0;

protected:
	void operator=(Movement&) = delete;

	const MovementComponentData& myData;
	MovementComponent& myMovementComponent;
	CU::Matrix44f& myOrientation;
	CU::Vector2<float> myVelocity;
	CU::Vector2<float> myDirectionTarget;

};