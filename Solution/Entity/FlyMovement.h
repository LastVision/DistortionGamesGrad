#pragma once
#include "Movement.h"
#include "PhysicsComponent.h"

class PhysicsComponent;

class FlyMovement : public Movement
{
public:
	FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~FlyMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Impulse(const CU::Vector2<float>& aVelocity) override;
	void Activate(const CU::Vector2<float>& aVelocity) override;
	void DeActivate() override;
	void SetVelocity(const CU::Vector2<float>& aVelocity) override;

	void ReceiveNote(const ContactNote& aNote) override;
	
	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal) override;


private:
	void operator=(FlyMovement&) = delete;

	bool myHasContact;

	void HandleContact();
	void Drag(float aDeltaTime);
	void Rotate(float aDeltaTime);
	void Translate();
};

