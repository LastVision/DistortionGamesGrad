#pragma once
#include "Movement.h"
class DashAimMovement : public Movement
{
public:
	DashAimMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~DashAimMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Activate() override;

	void ReceiveNote(const ContactNote& aNote) override;
	
private:
	void operator=(DashAimMovement&) = delete;

	void Rotate(float aDeltaTime);

	float myTimer;
};

