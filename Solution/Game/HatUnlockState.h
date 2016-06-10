#pragma once
#include "Subscriber.h"
#include "GameState.h"
#include <Tweener.h>
namespace GUI
{
	class GUIManager;
}

namespace Prism
{
	class SpriteAnimator;
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
	void ReadXML();
	void Spin();
	void WinHat(int aHatID);
	void WrapIndex();
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

	float mySoundAmount;
	float mySpinTimer;
	float myMaxSpinTime;
	bool myHasWonAllHats;

	Prism::SpriteProxy* myHatWon;
	float myTotalTime;
	float myHatWonScaling;
	Prism::SpriteProxy* mySpinBox;
	Prism::SpriteProxy* myAllHatsWonText;
	Prism::SpriteProxy* myGoldBagSprite;
	Prism::SpriteProxy* myGoldCostBox;

	int myLeftIndex;
	int myMiddleIndex;

	float myMoveAmount;
	CU::Vector2<float> myRenderPosition;

	Tweener<float> myTweener;

	int mySpinCost;
	bool myShowGoldCost;
	float myGoldCostMovement;
	float myGoldCostFade;

	Prism::SpriteAnimator* myAnimator;

	Prism::SpriteAnimator* mySpinHandleAnimator;

	Prism::SpriteProxy* myNotEnoughCashSprite;
	float myNotEnoughCashScale;
	float myNotEnoughCashTimer;
	float myTimeToNotEnoughCash;
	bool myAnimateNotEnoughCash;
	bool myRenderNotEnoughCash;
};

