#pragma once
#include "Movement.h"

namespace Prism
{
	class Instance;
	class Scene;
}

class DashAimMovement : public Movement
{
public:
	DashAimMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation
		, MovementComponent& aMovementComponent, Prism::Scene* aScene);
	~DashAimMovement();

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
	void operator=(DashAimMovement&) = delete;

	void Rotate(float aDeltaTime);

	float myTimer;
	Prism::Instance* myArrow;
	Prism::Scene* myScene;
};

