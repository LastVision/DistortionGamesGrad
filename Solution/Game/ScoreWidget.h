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

struct Score;

class ScoreWidget : public GUI::Widget
{
public:
	ScoreWidget(const Score* aScore);
	~ScoreWidget();

	void Render(const CU::Vector2<float>& aParentPosition) override;

	void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

private:
	const Score* myScore;
	Prism::SpriteProxy* myBackground;
};