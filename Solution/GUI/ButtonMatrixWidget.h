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

		const CU::GrowingArray<Widget*>& GetButtons(const int aRow) const;
		const CU::Vector2<int>& GetSize() const;
	private:
		const int GetAmountOfStarsFromFile(const int aLevelID);
		CU::GrowingArray<bool> RetrieveUnlockedLevelsFromFile();

		CU::GrowingArray<CU::GrowingArray<Widget*>> myButtonMatrix;
		CU::GrowingArray<StarWidget*> myStarMatrix;
		CU::Vector2<int> myButtonMatrixIndex;
	};

	inline const CU::GrowingArray<Widget*>& ButtonMatrixWidget::GetButtons(const int aRow) const
	{
		return myButtonMatrix[aRow];
	}

	inline const CU::Vector2<int>& ButtonMatrixWidget::GetSize() const
	{
		return myButtonMatrixIndex;
	}
}