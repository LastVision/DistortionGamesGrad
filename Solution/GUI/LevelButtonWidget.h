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
			const std::string& aButtonText = "default", const std::string& aHoverText = "", const int someStars = 0);
		virtual ~LevelButtonWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;
		void Update(float aDeltaTime) override;
		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

	private:
		CU::GrowingArray<StarWidget*> myStars;
		CU::Vector2<float> myStarPosition;
		CU::Vector2<float> myStarOffset;
	};
}