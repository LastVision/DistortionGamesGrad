#pragma once
#include "Movement.h"

class FlyMovement : public Movement
{
public:
	FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~FlyMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Activate() override;
	void DeActivate() override;

	void ReceiveNote(const ContactNote& aNote) override;

private:
	void operator=(FlyMovement&) = delete;

	void HandleContact();
	void Drag(float aDeltaTime);
	void Rotate(float aDeltaTime);
	void Translate();

	CU::Vector2<float> myPreviousPosition;

	struct Contact
	{
		Contact() : myFoundTouch(false), myOther(nullptr){};
		bool myFoundTouch;
		Entity* myOther;
		CU::Vector2<float> myContactPoint;
		CU::Vector2<float> myContactNormal;
	};
	volatile Contact myContact;
};

