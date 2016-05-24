#pragma once

class MovementComponent;
class PhysicsComponent;

class Movement
{
public:
	Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	virtual ~Movement();

	virtual void Reset(){};

	virtual void Init(){};

	virtual void Update(float aDeltaTime, bool aShouldCollide) = 0;
	virtual void Render() {}
	virtual void ReceiveNote(const ContactNote& aNote) = 0;
	virtual void SetDirectionTarget(const CU::Vector2<float>& aDirection) = 0;
	virtual void Impulse() = 0;
	virtual void Impulse(const CU::Vector2<float>& aVelocity) = 0;
	virtual void Activate(const CU::Vector2<float>& aVelocity) = 0;
	virtual void DeActivate() = 0;
	virtual void SetVelocity(const CU::Vector2<float>& aVelocity) = 0;

	virtual void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal) = 0;

	const CU::Vector2<float>& GetVelocity() const;

	void SetIsInSteam(bool aIsInSteam);

protected:
	void operator=(Movement&) = delete;
	bool myIsActive;
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&
		, const CU::Vector3<float>&)> myRaycastHandler;

	const MovementComponentData& myData;
	MovementComponent& myMovementComponent;
	CU::Matrix44f& myOrientation;
	CU::Vector2<float> myVelocity;
	CU::Vector2<float> myDirectionTarget;

	bool myIsInSteam;

};

inline const CU::Vector2<float>& Movement::GetVelocity() const
{
	return myVelocity;
}

inline void Movement::SetIsInSteam(bool aIsInSteam)
{
	myIsInSteam = aIsInSteam;
}