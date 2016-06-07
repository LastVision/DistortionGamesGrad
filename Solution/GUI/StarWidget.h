#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
}

class StarWidget : public GUI::Widget
{
public:
	StarWidget(bool anActive, int anID, bool aStartInstantly, const CU::Vector2<float>& aSize = { 128.f, 128.f });
	~StarWidget();

	void Update(float aDeltaTime) override;
	void UpdateScoreStars(float aDeltaTime);
	void Render(const CU::Vector2<float>& aParentPosition) override;

	void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;
	const bool GetIsDone();
private:
	Prism::SpriteProxy* myBackground;
	Prism::SpriteProxy* myStar;
	float myTime;
	int myID;
	bool myActive;

	float myTotalTime;

	bool myHasPlayedSound;

	CU::Vector2<float> myParentPosition;
};

inline const bool StarWidget::GetIsDone()
{
	if (myActive == false) return false;

	if (myTime >= 0.9f && myHasPlayedSound == false)
	{
		myHasPlayedSound = true;
		return true;
	}

	return false;
}