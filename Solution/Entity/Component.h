#pragma once
#include <GameEnum.h>

class Entity;

struct BounceNote;
struct CharacterAnimationNote;
struct ContactNote;
struct CollisionNote;
struct DeathNote;
struct LoseBodyPartNote;
struct ShouldDieNote;
struct SpawnNote;
struct VibrationNote;
struct SteamVentNote;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	//TODO: Make = 0 ?
	virtual void Reset();

	virtual void ReceiveNote(const BounceNote&) {}
	virtual void ReceiveNote(const CharacterAnimationNote&) {}
	virtual void ReceiveNote(const CollisionNote&) {}
	virtual void ReceiveNote(const ContactNote&) {}
	virtual void ReceiveNote(const DeathNote&) {}
	virtual void ReceiveNote(const LoseBodyPartNote&) {}
	virtual void ReceiveNote(const ShouldDieNote&) {}
	virtual void ReceiveNote(const SpawnNote&) {}
	virtual void ReceiveNote(const VibrationNote&) {}
	virtual void ReceiveNote(const SteamVentNote&) {}

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