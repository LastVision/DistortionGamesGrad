#pragma once
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class VictoryState : public GameState
{
public:
	VictoryState();
	~VictoryState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;
	void ResumeState() override;
	void PauseState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;

	void OnResize(int aWidth, int aHeight) override;
private:
	GUI::GUIManager* myGUIManager;
};

