#pragma once
#include "Component.h"
#include <PhysicsCallbackStruct.h>
#include "Subscriber.h"

struct PhysicsComponentData;

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxTriangleMesh;
	class PxShape;
}

class PhysicsComponent : public Component, public Subscriber
{
public:
	PhysicsComponent(Entity& aEntity, const PhysicsComponentData& aPhysicsComponentData, const std::string& aFBXPath);
	~PhysicsComponent();

	void Update(float aDeltaTime) override;
	void Reset() override;

	void Sleep();
	void Wake();

	void SwapOrientations();

	float* GetPosition();
	float* GetOrientation();

	void UpdateOrientation();
	void UpdateOrientationStatic();

	void AddForce(const CU::Vector3<float>& aDirection, float aMagnitude);
	void SetVelocity(const CU::Vector3<float>& aVelocity);
	void TeleportToPosition(const CU::Vector3<float>& aPosition);
	void MoveToPosition(const CU::Vector3<float>& aPosition);
	void SetPlayerCapsulePosition(const CU::Vector3<float>& aPosition);

	ePhysics GetPhysicsType() const;

	void AddCallback();
	void AddToScene();
	void RemoveFromScene();

	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;

	const int GetCapsuleControllerId() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool IsInScene() const;
	float GetHeight() const;

private:
	const PhysicsComponentData& myData;
	Prism::PhysicsCallbackStruct myCallbackStruct;
	physx::PxTriangleMesh* GetPhysMesh(const std::string& aFBXPath);


	float myThread4x4Float[16];
	float my4x4Float[16];
	float myPosition[3];
	physx::PxRigidDynamic* myDynamicBody;
	physx::PxRigidStatic* myStaticBody;
	physx::PxShape** myShapes;
	ePhysics myPhysicsType;

	int myCapsuleControllerId;

	bool myIsAwake;
	bool myIsInScene;
};

inline const int PhysicsComponent::GetCapsuleControllerId() const
{
	return myCapsuleControllerId;
}

inline eComponentType PhysicsComponent::GetTypeStatic()
{
	return eComponentType::PHYSICS;
}

inline eComponentType PhysicsComponent::GetType()
{
	return GetTypeStatic();
}

inline ePhysics PhysicsComponent::GetPhysicsType() const
{
	return myPhysicsType;
}

inline bool PhysicsComponent::IsInScene() const
{
	return myIsInScene;
}