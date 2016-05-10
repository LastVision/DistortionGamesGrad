#pragma once
#include "Component.h"
#include <Subscriber.h>
namespace CU
{
	class ControllerInput;
}

class MovementComponent;

struct InputComponentData;


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
	int GetPlayerID();
	bool GetIsFlipped() const;
	void SetIsFlipped(bool aIsFlipped);
	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveMessage(const OnPlayerLevelComplete& aMessage) override;
	bool GetIsActive();

private:

	const InputComponentData& myComponentData;
	CU::ControllerInput* myController;

	MovementComponent* myMovement;
	int myPlayerID;
	bool myIsActive;
	bool myHasCompletedLevel;
	CU::Matrix44<float>& myOrientation;
	bool myIsFlipped;
};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::INPUT;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}