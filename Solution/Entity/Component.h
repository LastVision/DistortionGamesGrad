#pragma once
#include <GameEnum.h>

class Entity;

struct BounceNote;
struct ContactNote;
struct CollisionNote;
struct DeathNote;
struct ShouldDieNote;
struct SpawnNote;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	//TODO: Make = 0 ?
	virtual void Reset();

	virtual void ReceiveNote(const BounceNote&) {}
	virtual void ReceiveNote(const CollisionNote&) {}
	virtual void ReceiveNote(const ContactNote&) {}
	virtual void ReceiveNote(const DeathNote&) {}
	virtual void ReceiveNote(const ShouldDieNote&) {}
	virtual void ReceiveNote(const SpawnNote&) {}

	virtual void Init();

	virtual void Update(float aDeltaTime);

	Entity& GetEntity();

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();


protected:
	void operator=(Component&) = delete;

	Entity& myEntity;
};

inline eComponentType Component::GetTypeStatic()
{
	return eComponentType::NOT_USED;
}

inline eComponentType Component::GetType()
{
	return GetTypeStatic();
}


inline Entity& Component::GetEntity()
{
	return myEntity;
}