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
	LevelSelectState(CU::ControllerInput* aController);
	~LevelSelectState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:

	GUI::GUIManager* myGUIManager;

	CU::ControllerInput* myController;
};

