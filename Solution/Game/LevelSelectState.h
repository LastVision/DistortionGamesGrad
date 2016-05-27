#pragma once
#include "GameState.h"
#include <Subscriber.h>

namespace GUI
{
	class GUIManager;
	class Cursor;
}

namespace CU
{
	class ControllerInput;
}

class LevelSelectState : public GameState, public Subscriber
{
public:
	LevelSelectState();
	~LevelSelectState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;
	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:

	CU::GrowingArray<bool> RetrieveUnlockedLevelsFromFile();
	const int GetAmountOfStarsFromFile(const int aLevelID);

	GUI::GUIManager* myGUIManager;

};

