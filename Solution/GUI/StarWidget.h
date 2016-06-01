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
	void Render(const CU::Vector2<float>& aParentPosition) override;

	void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

private:
	Prism::SpriteProxy* myBackground;
	Prism::SpriteProxy* myStar;
	float myTime;
	int myID;
	bool myActive;
};