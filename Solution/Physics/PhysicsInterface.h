#pragma once
#include <functional>
#include <Vector.h>
#include <Matrix44.h>

struct InputComponentData;

namespace physx
{
	class PxSimulationEventCallback;
}

class Entity;
class PhysicsComponent;
struct PhysicsComponentData;

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxShape;
	class PxActor;
}



namespace Prism
{
	struct PhysicsCallbackStruct;
	class PhysicsManager;


	class PhysicsInterface
	{
	public:
		static void Create(std::function<void(PhysicsComponent*, PhysicsComponent*, bool)> anOnTriggerCallback
			, std::function<void(PhysicsComponent*, PhysicsComponent*, CU::Vector3<float>, CU::Vector3<float>, bool)> aOnContactCallback);
		static void Destroy();
		static PhysicsInterface* GetInstance();

#ifdef THREAD_PHYSICS
		void InitThread();
		void ShutdownThread();
#endif
		bool GetInitDone() const;
		void FrameUpdate();

		void Add(const PhysicsCallbackStruct& aCallbackStruct);
		
		void EndFrame();

		void RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance
			, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall, const PhysicsComponent* aComponent);
		void AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude);
		void SetVelocity(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity);
		void TeleportToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition);
		void TeleportToPosition(physx::PxRigidStatic* aStaticBody, const CU::Vector3<float>& aPosition);
		void TeleportToPosition(int aID, const CU::Vector3<float>& aPosition);
		void MoveToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition);

		int CreatePlayerController(const CU::Vector3<float>& aStartPosition, PhysicsComponent* aComponent, bool aShouldAddToScene);
		void Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement);

		void UpdateOrientation(physx::PxRigidDynamic* aDynamicBody, physx::PxShape** aShape, float* aThread4x4);
		void UpdateOrientation(physx::PxRigidStatic* aStaticBody, physx::PxShape** aShape, float* aThread4x4);
		bool GetAllowedToJump(int aId);
		void SetPosition(int aId, const CU::Vector3<float>& aPosition);
		void GetPosition(int aId, CU::Vector3<float>& aPositionOut);

		void SubscribeToTriggers(physx::PxSimulationEventCallback* aSubscriber);

		void Create(PhysicsComponent* aComponent, const PhysicsCallbackStruct& aPhysData
			, float* aOrientation, const std::string& aFBXPath
			, physx::PxRigidDynamic** aDynamicBodyOut, physx::PxRigidStatic** aStaticBodyOut
			, physx::PxShape*** someShapesOut, bool aShouldAddToScene, bool aShouldBeSphere);
		void Add(physx::PxRigidDynamic* aDynamic);
		void Add(physx::PxRigidStatic* aStatic);
		void Add(int aCapsuleID);
		void Remove(physx::PxRigidDynamic* aDynamic, const PhysicsComponentData& aData);
		void Remove(physx::PxRigidStatic* aStatic, const PhysicsComponentData& aData);
		void Remove(int aCapsuleID);
		void Sleep(physx::PxRigidDynamic* aDynamic);
		void Sleep(int aCapsuleID);
		void Wake(physx::PxRigidDynamic* aDynamic);
		void Wake(int aCapsuleID);
		int GetFPS();
	private:
		// Requires PhysX includes!!
		PhysicsManager* GetManager() const;
		PhysicsInterface(std::function<void(PhysicsComponent*, PhysicsComponent*, bool)> anOnTriggerCallback
			, std::function<void(PhysicsComponent*, PhysicsComponent*, CU::Vector3<float>, CU::Vector3<float>, bool)> aOnContactCallback);
		~PhysicsInterface();
		PhysicsManager* myManager;
		static PhysicsInterface* myInstance;
	};
}