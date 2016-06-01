#pragma once
#include "Subscriber.h"
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class HatUnlockState : public GameState, public Subscriber
{
public:
	HatUnlockState();
	~HatUnlockState();

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
	void Spin();
	void WinHat(int aHatID);
	bool myIsSpinning;
	struct HatUnlock
	{
		HatUnlock(){};
		HatUnlock(Prism::SpriteProxy* aSprite, int aID)
			: mySprite(aSprite)
			, myID(aID)
		{}
		Prism::SpriteProxy* mySprite;
		int myID;
	};
	CU::GrowingArray<HatUnlock> myHats;

	int myCurrentHatToWin;

	float mySpinTimer;
	float myMaxSpinTime;
	float myTotalTime;
	bool myHasWonAllHats;

	Prism::SpriteProxy* myHatWon;
	Prism::SpriteProxy* mySpinBox;
	Prism::SpriteProxy* myAllHatsWonText;

};
