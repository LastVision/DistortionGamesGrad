#include "stdafx.h"
#include "BarWidget.h"

namespace GUI
{
	BarWidget::BarWidget(const float& aMaxValue, const float& aCurrentValue, CU::Vector2<float> aSize, CU::Vector4<float> aColor)
		: myMaxValueFloat(&aMaxValue)
		, myCurrentValueFloat(&aCurrentValue)
		, myIsFloat(true)
		, myColor(aColor)
	{
		mySize = aSize;

		CU::Vector2<float> backgroundSize = mySize + 8.f;

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/Menu/ScoreScreen/T_bar_background.dds", mySize, mySize / 2.f);
		myForegroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/Menu/ScoreScreen/T_bar_foreground.dds", mySize, mySize / 2.f);
		myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/Menu/ScoreScreen/T_bar_fill.dds", mySize, mySize / 2.f);
	}

	BarWidget::~BarWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myForegroundSprite);
		SAFE_DELETE(myValueSprite);
	}

	void BarWidget::Update(float)
	{
		if (myIsFloat == false)
		{
			if (*myCurrentValueInt * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
			}
		}
		else
		{
			if (*myCurrentValueFloat * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = *myCurrentValueFloat / *myMaxValueFloat;
				newSize *= 0.841796875f;
				newSize += 0.0791015625f;
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
				myValueSprite->SetUVZeroToOne(CU::Vector2<float>(0, 0), CU::Vector2<float>(newSize, 1.f));
			}
		}
	}

	void BarWidget::Render(const CU::Vector2<float>& aParentPosition, float anAlpha)
	{
		myColor.w = anAlpha;
		myBackgroundSprite->Render(myPosition + aParentPosition, { 1.f, 1.f }, { 1.f, 1.f, 1.f, anAlpha });
		myValueSprite->Render(myPosition + aParentPosition, { 1.f, 1.f }, myColor);
		myForegroundSprite->Render(myPosition + aParentPosition, { 1.f, 1.f }, { 1.f, 1.f, 1.f, anAlpha });
	}

	void BarWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		float newSize;

		if (myIsFloat == false)
		{
			newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
		}
		else
		{
			newSize = *myCurrentValueFloat / *myMaxValueFloat;
		}

		//CU::Vector2<float> ratio = myBackgroundSprite->GetSize() / anOldWindowSize.x;
		myBackgroundSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
		myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
		myForegroundSprite->SetSize({ mySize.x, mySize.y }, mySize / 2.f);
	}
}