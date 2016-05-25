#include "stdafx.h"
#include <CommonHelper.h>
#include "Cursor.h"
#include "../InputWrapper/InputWrapper.h"

namespace GUI
{
	Cursor::Cursor(const CU::Vector2<int>& aWindowSize)
		: myCurrentType(eCursorType::NORMAL)
		, mySprites(8)
		, myShouldRender(false)
		, myIsUsingController(false)
	{
		myWindowSize.x = float(aWindowSize.x);
		myWindowSize.y = float(aWindowSize.y);

		Prism::SpriteProxy* normalSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_cursor.dds", { 70.f, 70.f }, { 35.f, 35.f });
		mySprites.Add(normalSprite);

		myPosition = myWindowSize / 2.f;
		myPreviousPosition = myPosition;

		myPositionZeroToOne = myPosition / myWindowSize;

	}

	Cursor::~Cursor()
	{
		mySprites.DeleteAll();
	}

	void Cursor::Update()
	{
		// uncomment for software cursor:
		//myPosition.x += CU::InputWrapper::GetInstance()->GetMouseDX();
		//myPosition.y -= CU::InputWrapper::GetInstance()->GetMouseDY();

		myPreviousPosition = myPosition;

		myPosition.x = CU::InputWrapper::GetInstance()->GetMousePosition().x;
		myPosition.y = myWindowSize.y - CU::InputWrapper::GetInstance()->GetMousePosition().y;

		myPositionZeroToOne = myPosition / myWindowSize;
		myPositionZeroToOne.x = CU::Clip(myPositionZeroToOne.x, 0, 1.f);
		myPositionZeroToOne.y = CU::Clip(myPositionZeroToOne.y, 0, 1.f);

		if (myIsUsingController == true
			&& (CU::InputWrapper::GetInstance()->GetMouseDX() > 0.f
			|| CU::InputWrapper::GetInstance()->GetMouseDY() > 0.f))
		{
			myIsUsingController = false;
		}
	}

	void Cursor::Render()
	{
		if (myShouldRender == true && myIsUsingController == false)
		{
			mySprites[static_cast<int>(myCurrentType)]->Render(myPosition);
		}
	}

	const CU::Vector2<float>& Cursor::GetMousePosition() const
	{
		return myPosition;
	}

	const CU::Vector2<float>& Cursor::GetMousePositionZeroToOne() const
	{
		return myPositionZeroToOne;
	}

	void Cursor::OnResize(int aWidth, int aHeight)
	{
		myWindowSize.x = float(aWidth);
		myWindowSize.y = float(aHeight);
	}

	void Cursor::ClipCursor()
	{
		if (myPosition.x < 0.f)
		{
			myPosition.x = 0.f;
		}
		else if (myPosition.x > myWindowSize.x)
		{
			myPosition.x = myWindowSize.x;
		}

		if (myPosition.y < 0.f)
		{
			myPosition.y = 0.f;
		}
		else if (myPosition.y > myWindowSize.y)
		{
			myPosition.y = myWindowSize.y;
		}
	}
}