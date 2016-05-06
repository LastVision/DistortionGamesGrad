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
	void Activate() override;
	void DeActivate() override;

	void ReceiveNote(const ContactNote& aNote) override;
	
private:
	void operator=(DashAimMovement&) = delete;

	void Rotate(float aDeltaTime);

	float myTimer;
	Prism::Instance* myArrow;
	Prism::Scene* myScene;
};

