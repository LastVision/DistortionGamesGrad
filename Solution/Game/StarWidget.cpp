#include "stdafx.h"

#include <ModelLoader.h>
#include "StarWidget.h"
#include <SpriteProxy.h>
#include <Tweener.h>

StarWidget::StarWidget(bool anActive, int anID)
	: GUI::Widget()
	, myID(anID)
	, myActive(anActive)
{
	mySize = CU::Vector2<float>(128.f, 128.f);

	myTime = static_cast<float>(-myID) * 1.f - 1.5f;
	
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/ScoreScreen/T_star_background.dds", mySize, mySize / 2.f);
	myStar = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/ScoreScreen/T_star.dds", mySize, mySize / 2.f);
}

StarWidget::~StarWidget()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myStar);
}

void StarWidget::Update(float aDeltaTime)
{
	myTime += aDeltaTime * 2.f;
	myTime = fminf(1.f, myTime);
}

void StarWidget::Render(const CU::Vector2<float>& aParentPosition)
{
	if (myIsVisible == true)
	{
		myBackground->Render(myPosition + aParentPosition);

		if (myActive == true)
		{
			float alpha = fmaxf(0, myTime);
			Tweener<float> tweener;
			float scale = tweener.DoTween(alpha, 0, 1.f, 1.f, eTweenType::SINUS_HALF);
			myStar->Render(myPosition + aParentPosition, CU::Vector2<float>(scale, scale));
		}
	}
}

void StarWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
{
	GUI::Widget::OnResize(aNewSize, anOldSize);
	myBackground->SetSize(mySize, mySize / 2.f);
	myStar->SetSize(mySize, mySize / 2.f);
}