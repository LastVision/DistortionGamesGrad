#pragma once
#include "Component.h"
#include <Matrix.h>
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class Scene;
	class ParticleEmitterInstance;
}

class Component;

struct EntityData;

class Entity
{
public:
	Entity(const EntityData& aEntityData, Prism::Scene* aScene, const CU::Vector3<float>& aStartPosition, 
		const CU::Vector3f& aRotation, const CU::Vector3f& aScale, const std::string& aSubType, int aPlayerID = -1);
	~Entity();

	void Reset();
	virtual void Update(float aDeltaTime);

	void AddComponent(Component* aComponent);
	void RemoveComponent(eComponentType aComponent);

	template <typename T> T* GetComponent();
	template <typename T> const T* GetComponent() const;
	template <typename T> void SendNote(const T& aNote);

	void AddToScene();
	void RemoveFromScene();

	const CU::Matrix44<float>& GetOrientation() const;
	float* GetOrientationAsFloatPtr();
	void SetRotation(const CU::Vector3<float>& aRotation);
	void SetPosition(const CU::Vector3<float>& aPosition);
	const CU::Vector3<float>& GetStartPosition() const;
	void ResetPosition();

	Prism::Scene* GetScene();
	eEntityType GetType() const;

	void AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection);
	Prism::ParticleEmitterInstance* GetEmitter();

	void Kill(bool aRemoveFromPhysics = true);

	bool IsAlive() const;

	void SetGID(unsigned int aGID);
	unsigned int GetGID() const;

	bool IsInScene() const;
	const std::string& GetSubType() const;

	bool GetIsEnemy();
	void SetIsEnemy(bool aIsEnemy);

	bool IsActive() const;
	void SetActive(bool aIsActive);
	void SetDelayedAddToScene();

	void SetShouldBeRemoved(bool aShouldBeRemoved);
	bool ShouldBeRemoved() const;

private:
	void operator=(Entity&) = delete;

	const EntityData& myEntityData;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;

	Prism::ParticleEmitterInstance* myEmitterConnection;

	bool myAlive;
	bool myIsActive;
	bool myIsInScene;
	bool myShouldBeRemoved;

	std::string mySubType;

	Prism::Scene* myScene;

	CU::Matrix44<float> myOrientation;

	unsigned int myGID;
	bool myIsEnemy;

	float myTimeActiveBeforeKill;
	float myTimeActiveBeforeKillTimer;

	float myDelayAddToSceneTimer;
	bool myDelayedAddToScene;

	CU::Vector3<float> myStartPosition;
	CU::Vector3<float> myStartRotation;
};

template <typename T>
inline T* Entity::GetComponent()
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetTypeStatic())]);
}

template <typename T>
inline const T* Entity::GetComponent() const
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetTypeStatic())]);
}

template <typename T>
inline void Entity::SendNote(const T& aMessage)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->ReceiveNote(aMessage);
		}
	}
}

inline const CU::Matrix44<float>& Entity::GetOrientation() const
{
	return myOrientation;
}

inline float* Entity::GetOrientationAsFloatPtr()
{
	return &myOrientation.myMatrix[0];
}

inline void Entity::SetRotation(const CU::Vector3<float>& aRotation)
{
	CU::Vector3f position(myOrientation.GetPos());
	myOrientation.SetPos(CU::Vector3f());

	myOrientation = myOrientation * CU::Matrix44f::CreateRotateAroundZ(aRotation.z);
	myOrientation = myOrientation * CU::Matrix44f::CreateRotateAroundX(aRotation.x);
	myOrientation = myOrientation * CU::Matrix44f::CreateRotateAroundY(aRotation.y);

	myOrientation.SetPos(position);
}


inline void Entity::SetPosition(const CU::Vector3<float>& aPosition)
{
	myOrientation.SetPos(aPosition);
}

inline const CU::Vector3<float>& Entity::GetStartPosition() const
{
	return myStartPosition;
}

inline void Entity::ResetPosition()
{
	myOrientation = CU::Matrix44<float>();
	myOrientation.SetPos(myStartPosition);
	SetRotation(myStartRotation);
}

inline Prism::Scene* Entity::GetScene()
{
	return myScene;
}

inline bool Entity::IsAlive() const
{
	return myAlive;
}

inline void Entity::SetGID(unsigned int aGID)
{
	myGID = aGID;
}

inline unsigned int Entity::GetGID() const
{
	return myGID;
}

inline bool Entity::IsInScene() const
{
	return myIsInScene;
}

inline const std::string& Entity::GetSubType() const
{
	return mySubType;
}

inline bool Entity::GetIsEnemy()
{
	return myIsEnemy;
}

inline void Entity::SetIsEnemy(bool aIsEnemy)
{
	myIsEnemy = aIsEnemy;
}

inline bool Entity::IsActive() const
{
	return myIsActive;
}

inline void Entity::SetActive(bool aIsActive)
{
	myIsActive = aIsActive;
}

inline void Entity::SetDelayedAddToScene()
{
	myDelayAddToSceneTimer = 0.1f;
	myDelayedAddToScene = true;
}

inline void Entity::SetShouldBeRemoved(bool aShouldBeRemoved)
{
	myShouldBeRemoved = aShouldBeRemoved;
}

inline bool Entity::ShouldBeRemoved() const
{
	return myShouldBeRemoved;
}