#include "stdafx.h"
#include "LevelButtonWidget.h"
#include "StarWidget.h"

namespace GUI
{
	LevelButtonWidget::LevelButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: ButtonWidget(aReader, anXMLElement)
		, myStars(3)
	{
	}

	LevelButtonWidget::LevelButtonWidget(const CU::Vector2<float>& aSize, const CU::Vector2<float>& aPosition, 
		const CU::Vector2<float>& aStarPosition, const CU::Vector2<float>& aStarOffest,
		const std::string& aSpritePath, const std::string& aSpriteHoverPath, const std::string& aSpritePressedPath,
		const std::string& aButtonText, const std::string& aHoverText, const int someStars, const CU::Vector2<float>& aTextOffset)
		: ButtonWidget(aSize, aPosition, aSpritePath, aSpriteHoverPath, aSpritePressedPath, aButtonText, aHoverText, aTextOffset)
		, myStars(3)
		, myStarPosition(aStarPosition)
		, myStarOffset(aStarOffest)
	{
		myStars.Add(new StarWidget(someStars > 0, 1, mySize * 0.5f));
		myStars.Add(new StarWidget(someStars > 1, 2, mySize * 0.5f));
		myStars.Add(new StarWidget(someStars > 2, 3, mySize * 0.5f));
	}

	LevelButtonWidget::~LevelButtonWidget()
	{
		myStars.DeleteAll();
	}

	void LevelButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		__super::Render(aParentPosition);
		for (int i = 0; i < myStars.Size(); ++i)
		{
			CU::Vector2<float> position = myPosition + myStarPosition;
			position.x += myStarOffset.x * i;

			myStars[i]->Render(position + aParentPosition);
		}
	}

	void LevelButtonWidget::Update(float aDeltaTime)
	{
		__super::Update(aDeltaTime);
		for each(StarWidget* star in myStars)
		{
			star->Update(aDeltaTime);
		}
	}

	void LevelButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		__super::OnResize(aNewSize, anOldSize);
		for each(StarWidget* star in myStars)
		{
			star->OnResize(aNewSize, anOldSize);
		}
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myStarOffset *= ratio;
		myStarPosition *= ratio;
	}

	void LevelButtonWidget::SetStars(const int someStars)
	{
		myStars.Add(new StarWidget(someStars > 0, 1, mySize * 0.5f));
		myStars.Add(new StarWidget(someStars > 1, 2, mySize * 0.5f));
		myStars.Add(new StarWidget(someStars > 2, 3, mySize * 0.5f));

		myStarPosition = { -64.f, -38.f };
		myStarOffset.x = 64.f;
	}
}