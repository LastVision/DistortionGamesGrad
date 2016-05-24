#pragma once
#include "Widget.h"

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
class StarWidget;

namespace GUI
{
	class LevelButtonWidget;
	class ButtonWidget;
	class ButtonMatrixWidget : public Widget
	{
	public:
		ButtonMatrixWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~ButtonMatrixWidget();

		const CU::GrowingArray<ButtonWidget*>& GetButtons() const;
	private:
		const int GetAmountOfStarsFromFile(const int aLevelID);


		CU::GrowingArray<ButtonWidget*> myButtonMatrix;
		CU::GrowingArray<StarWidget*> myStarMatrix;
		CU::Vector2<int> myButtonMatrixIndex;
	};

	inline const CU::GrowingArray<ButtonWidget*>& ButtonMatrixWidget::GetButtons() const
	{
		return myButtonMatrix;
	}
}