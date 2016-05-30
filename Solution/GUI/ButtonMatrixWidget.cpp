#include "stdafx.h"

#include "ButtonMatrixWidget.h"
#include "ButtonWidget.h"
#include "LevelButtonWidget.h"
#include <OnClickMessage.h>
#include <CommonHelper.h>
#include <GameConstants.h>

namespace GUI
{
	ButtonMatrixWidget::ButtonMatrixWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myButtonMatrix(50)
	{
		std::string spritePathNormal = "";
		std::string spritePathHover = "";
		std::string spritePathPressed = "";

		std::string buttonEventType = "";

		CU::Vector2<float> buttonSize;
		CU::Vector2<float> buttonOffset;

		CU::Vector2<float> starPosition;
		CU::Vector2<float> starOffset;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonmatrix"), "x", myButtonMatrixIndex.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonmatrix"), "y", myButtonMatrixIndex.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonsize"), "x", buttonSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonsize"), "y", buttonSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonoffset"), "x", buttonOffset.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonoffset"), "y", buttonOffset.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonevent"), "type", buttonEventType);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starposition"), "x", starPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starposition"), "y", starPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "staroffset"), "x", starOffset.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "staroffset"), "y", starOffset.y);
		int index = 0;
		CU::GrowingArray<bool> unlockedlevels = RetrieveUnlockedLevelsFromFile();
		for (int y = 0; y < myButtonMatrixIndex.y; ++y)
		{
			CU::GrowingArray<Widget*> buttons(myButtonMatrixIndex.x);
			for (int x = 0; x < myButtonMatrixIndex.x; ++x)
			{
				CU::Vector2<float> textOffset;
				textOffset.x = index + 1 > 9 ? -20.f : -10.f;
				textOffset.y = 20.f;

				ButtonWidget* button = new LevelButtonWidget(buttonSize, { (x * buttonSize.x + buttonOffset.x * x) + myPosition.x,
					(-y * buttonSize.y + buttonOffset.y * -y) + myPosition.y }, starPosition, starOffset, spritePathNormal, spritePathHover, spritePathPressed,
					std::to_string(index + 1), "", GetAmountOfStarsFromFile(index + 1), textOffset);
				if (buttonEventType == "start_level")
				{
					button->SetEvent(new OnClickMessage(eOnClickEvent::START_LEVEL, index));
				}
#ifdef RELEASE_BUILD
				if (GC::EnableCheat == false)
				{
					if (index == 0)
					{
						button->SetActive(true);
					}
					else if (index >= unlockedlevels.Size())
					{
						button->SetActive(false);
					}
					else
					{
						button->SetActive(unlockedlevels[index]);
					}
				}
				else
				{
					button->SetActive(true);
				}
#else
				button->SetActive(true);
#endif
				buttons.Add(button);
				index++;
			}
			myButtonMatrix.Add(buttons);
		}

		/*for (int i = 0; i < myButtonMatrixIndex.x * myButtonMatrixIndex.y; ++i)
		{
			ButtonWidget* button = new LevelButtonWidget(buttonSize, { (x * buttonSize.x + buttonOffset.x * x) + myPosition.x, 
				(y * buttonSize.y + buttonOffset.y * y) + myPosition.y }, starPosition, starOffset, spritePathNormal, spritePathHover, spritePathPressed, 
				std::to_string(i + 1), "", GetAmountOfStarsFromFile(i + 1));
			if (buttonEventType == "start_level")
			{
				button->SetEvent(new OnClickMessage(eOnClickEvent::START_LEVEL, i));
			}
			
			myButtonMatrix.Add(button);
			x++;
			if (x >= myButtonMatrixIndex.x)
			{
				y--;
				x = 0;
			}
		}*/
	}

	ButtonMatrixWidget::~ButtonMatrixWidget()
	{
		for (int i = 0; i < myButtonMatrix.Size(); ++i)
		{
			myButtonMatrix[i].DeleteAll();
		}
	}

	const int ButtonMatrixWidget::GetAmountOfStarsFromFile(const int aLevelID)
	{
		int toReturn = 0;
		std::fstream file;
		file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(aLevelID) + ".bin", std::ios::binary | std::ios::in);

		if (file.is_open() == true)
		{
			int levelID = 0;
			float time = 0;
			int stars = 0;
			bool isEndOfFile = false;
			while (isEndOfFile == false)
			{
				if (file.eof())
				{
					isEndOfFile = true;
					break;
				}
				file >> levelID >> time >> stars;
				toReturn = stars;
			}

			file.close();
		}
		return toReturn;
	}

	CU::GrowingArray<bool> ButtonMatrixWidget::RetrieveUnlockedLevelsFromFile()
	{
		CU::GrowingArray<bool> toReturn(64);

		std::fstream file;
		file.open(CU::GetMyDocumentFolderPath() + "Data/UnlockedLevels.bin", std::ios::binary | std::ios::in);

		if (file.is_open() == true)
		{
			int levelID;
			bool isUnlocked = false;
			bool isEndOfFile = false;
			while (isEndOfFile == false)
			{
				if (file.eof())
				{
					isEndOfFile = true;
					break;
				}
				file >> levelID >> isUnlocked;
				toReturn.Add(isUnlocked);
			}

			file.close();
		}

		if (toReturn.GetLast() == true)
		{
			GC::HasWonGame = true;
		}

		return toReturn;
	}
}