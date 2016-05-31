#pragma once
#include "Subscriber.h"
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class HatState : public GameState, public Subscriber
{
public:
	HatState();
	~HatState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;


private:
	GUI::GUIManager* myGUIManager;



};

