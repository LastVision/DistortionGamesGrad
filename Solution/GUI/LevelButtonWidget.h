#pragma once
#include "ButtonWidget.h"

class StarWidget;
namespace GUI
{
	class LevelButtonWidget : public ButtonWidget
	{
	public:
		LevelButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		LevelButtonWidget(const CU::Vector2<float>& aSize, const CU::Vector2<float>& aPosition, 
			const CU::Vector2<float>& aStarPosition, const CU::Vector2<float>& aStarOffest,
			const std::string& aSpritePath, const std::string& aSpriteHoverPath, const std::string& aSpritePressedPath,
			const std::string& aButtonText = "default", const std::string& aHoverText = "", const int someStars = 0
			, const CU::Vector2<float>& aTextOffset = { 0.f, 0.f }, const int aLevelIndex = 0);
		virtual ~LevelButtonWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition, float anAlpha) override;
		void Update(float aDeltaTime) override;
		void OnMouseEnter(bool aShouldSound = true) override;
		void OnMouseExit() override;
		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		void SetStars(const int someStars);

	private:
		CU::GrowingArray<StarWidget*> myStars;
		CU::Vector2<float> myStarPosition;
		CU::Vector2<float> myStarOffset;
		int myLevelIndex;
		float mySetHighscoreLevelTimer;
	};
}