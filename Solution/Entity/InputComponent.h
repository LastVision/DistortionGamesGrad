#pragma once
#include "Component.h"
#include "../PostMaster/Subscriber.h"
namespace CU
{
	class ControllerInput;
}

class MovementComponent;

struct InputComponentData;
struct AnimationJoint;

class InputComponent : public Component, public Subscriber
{
public:
	InputComponent(Entity& aEntity, const InputComponentData& aInputData, CU::Matrix44<float>& aOrientation);
	~InputComponent();

	void Init() override;
	void AddController(int anID);

	void Reset() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void SetPlayerID(int aPlayerID);
	int GetPlayerID() const;
	bool GetIsFlipped() const;
	void SetIsFlipped(bool aIsFlipped);
	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const VibrationNote& aMessage) override;
	void ReceiveMessage(const OnPlayerLevelComplete& aMessage) override;
	void ReceiveMessage(const PlayerActiveMessage& aMessage) override;
	bool GetIsActive();
	void ResetIsInLevel();
	bool GetControllerIsConnected();
	void SetPlayersWinCount(int aWinCount);

	void SetStandingOnStomper(Entity* aStomper);
	const CU::Matrix44f& GetParticleOrientation();
private:

	const InputComponentData& myComponentData;
	CU::ControllerInput* myController;

	MovementComponent* myMovement;
	int myPlayerID;
	bool myIsActive;
	bool myHasCompletedLevel;
	CU::Matrix44<float>& myOrientation;
	bool myIsFlipped;
	bool myIsInLevel;

	float myTimeToSpawn;
	bool myIntendToSpawn;

	bool myAllowedToSpawn;
	AnimationJoint* myParticlePoint; //for particles
	CU::Matrix44<float> myParticleOrientation;

	Entity* myStandingOnStomper;
};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::INPUT;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}

inline void InputComponent::SetPlayersWinCount(int aWinCount)
{
	myAllowedToSpawn = (aWinCount <= 0);
}

inline void InputComponent::SetStandingOnStomper(Entity* aStomper)
{
	myStandingOnStomper = aStomper;
}