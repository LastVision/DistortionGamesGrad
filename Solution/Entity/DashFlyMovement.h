#pragma once
#include "Movement.h"

class DashFlyMovement : public Movement
{
public:
	DashFlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~DashFlyMovement();

	void Reset() override;
	void Update(float aDeltaTime) override;

	void ReceiveNote(const ContactNote& aNote) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Activate() override;
	void DeActivate() override;

private:
	float myTimer;
};

