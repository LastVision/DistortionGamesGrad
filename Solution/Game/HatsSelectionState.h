#pragma once
#include "Subscriber.h"
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class HatsSelectionState : public GameState, public Subscriber
{
public:
	HatsSelectionState();
	~HatsSelectionState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void HandleHatSelection(CU::ControllerInput* aController, int& aCurrentPlayerHat, int aPlayerID
		, bool& aControllerPressedLeft, bool& aControllerPressedRight);
	void Render() override;
	void ResumeState() override;
	void PauseState() override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

	void HandleControllerInSelection(CU::ControllerInput* aController, GUI::GUIManager* aManager, GUI::Cursor* aCursor);


private:
	GUI::GUIManager* myGUIManager;

	Prism::SpriteProxy* myLockSprite;
	Prism::SpriteProxy* myPlayerOnePortrait;
	Prism::SpriteProxy* myPlayerTwoPortrait;
	int myPlayerOneCurrentHat;
	int myPlayerTwoCurrentHat;

	bool mySecondControllerPressedLeft;
	bool mySecondControllerPressedRight;

	CU::GrowingArray<Prism::SpriteProxy*> myHats;

	CU::ControllerInput* mySecondController;

	Prism::SpriteProxy* myLeftArrow;
	Prism::SpriteProxy* myRightArrow;
	Prism::SpriteProxy* myArrowBox;

	float myUVScrollingTime;
};

