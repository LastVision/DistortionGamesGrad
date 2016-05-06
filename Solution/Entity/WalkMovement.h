#pragma once
#include "Movement.h"

class WalkMovement : public Movement
{
public:
	WalkMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~WalkMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Impulse(const CU::Vector2<float>& aVelocity) override;
	void Activate() override;

	void ReceiveNote(const ContactNote& aNote) override;

private:
	void HandleContact();
	void Drag(float aDeltaTime);
	void Walk(float aDeltaTime);
	void Translate();

	CU::Vector2<float> myPreviousPosition;

	struct Contact
	{
		Contact() : myLostTouch(false), myOther(nullptr){};
		bool myLostTouch;
		Entity* myOther;
		CU::Vector2<float> myContactPoint;
		CU::Vector2<float> myContactNormal;
	};
	volatile Contact myCurrentContact;
	volatile Contact myPreviousContact;
	bool myHasContact;
};

