#pragma once
#include "GameState.h"
#include <Subscriber.h>

namespace GUI
{
	class GUIManager;
}

namespace Prism
{
	class Text;
}

class OptionState : public GameState, public Subscriber
{
public:
	OptionState(bool aIsMainMenu);
	~OptionState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;
	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	GUI::GUIManager* myGUIManager;

	bool myIsMainMenu;
};

