#pragma once
#include <Widget.h>

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

namespace GUI
{
	class BarWidget;
}

class StarWidget;

struct Score;

class ScoreWidget : public GUI::Widget
{
public:
	ScoreWidget(const Score* aScore, const ScoreInfo& aScoreInfo);
	~ScoreWidget();

	void Update(float aDeltaTime) override;
	void Render(const CU::Vector2<float>& aParentPosition) override;

	void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

private:
	void operator=(const ScoreWidget&) = delete;
	float CalculateGoalValue();
	const Score* myScore;
	const ScoreInfo& myScoreInfo;
	Prism::SpriteProxy* myBackground;
	GUI::BarWidget* myBar;
	CU::GrowingArray<StarWidget*> myStars;
	float myMaxValue;
	float myCurrentValue;
	float myGoalValue;
};