#pragma once
#include "Movement.h"
#include "PhysicsComponent.h"

class PhysicsComponent;

struct PlayerComponentData;

class FlyMovement : public Movement
{
public:
	FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~FlyMovement();

	void Init() override;
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
	void HandleRaycastHead(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

	void HandleRaycastLegs(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);


private:
	void operator=(FlyMovement&) = delete;

	void RaycastBody();
	void RaycastHead();
	void RaycastLegs();
	void Drag(float aDeltaTime);
	void Rotate(float aDeltaTime);
	void Translate();

	const PlayerComponentData* myPlayerData;

	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&
		, const CU::Vector3<float>&)> myRaycastHandlerHead;

	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&
		, const CU::Vector3<float>&)> myRaycastHandlerLegs;

	bool myHasContact;
};

