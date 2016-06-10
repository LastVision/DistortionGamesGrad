#include "stdafx.h"

#include "ButtonMatrixWidget.h"
#include "ButtonWidget.h"
#include <CommonHelper.h>
#include "Cursor.h"
#include <Engine.h>
#include "GUIManager.h"
#include <OnClickMessage.h>
#include "HighscoreWidget.h"
#include "../InputWrapper/InputWrapper.h"
#include "LevelButtonWidget.h"
#include <PostMaster.h>
#include "SpriteWidget.h"
#include "TextWidget.h"
#include "ToggleBoxWidget.h"
#include "VolumeWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath, const Prism::Camera* aCamera, int aLeveID)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myMousePosition({ 0.f, 0.f })
		, myCamera(aCamera)
		, myLevelID(aLeveID)
		, myButtons(16)
		, myLevelButtons(16)
		, myUseController(false)
		, myControllerButtonIndexX(0)
		, myControllerButtonIndexY(0)
		, myIsPaused(false)
	{
		myWindowSize = { 1920.f, 1080.f }; // XML coordinates respond to this resolution, will be resized

		myWidgets = new WidgetContainer(nullptr, nullptr, myWindowSize, true);
		myWidgets->SetPosition({ 0.f, 0.f });

		ReadXML(aXMLPath);

		OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);

		PostMaster::GetInstance()->Subscribe(this, eMessageType::UNHOVER_CONTROLLER);
	}

	GUIManager::~GUIManager()
	{
		myButtons.RemoveAll();
		SAFE_DELETE(myWidgets);
		myActiveWidget = nullptr;
		PostMaster::GetInstance()->UnSubscribe(this, 0);
	}

	void GUIManager::AddWidget(Widget* aWidget)
	{
		myWidgets->AddWidget(aWidget);
	}

	void GUIManager::Update(float aDelta)
	{
		myMousePosition = myCursor->GetMousePosition();
		myWidgets->Update(aDelta);

		if (myIsPaused == false)
		{
			if (myCursor->IsUsingController() == false)
			{
				CheckMouseMoved();
				CheckMouseExited();
				CheckMouseDown();
				CheckMousePressed();
				CheckMouseReleased();

				CheckMouseEntered();
			}
		}
		else if (myActiveWidget != nullptr)
		{
			myActiveWidget->OnMouseExit();
			myActiveWidget = nullptr;
		}
	}

	void GUIManager::Render(float anAlpha)
	{
		myWidgets->Render({ 0.f, 0.f }, anAlpha);
	}

	void GUIManager::OnResize(int aWidth, int aHeight)
	{
		CU::Vector2<float> newSize = { float(aWidth), float(aHeight) };
		myWidgets->OnResize(newSize, myWindowSize);
		myWindowSize = newSize;
	}

	CU::Vector3<float> GUIManager::CalcCameraMovement() const
	{
		CU::Vector3<float> movement;

		float epsilon = 0.01f;
		if (myCursor->GetMousePositionZeroToOne().x < epsilon)
		{
			movement.x = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().x > 1.f - epsilon)
		{
			movement.x = 1.f;
		}

		if (myCursor->GetMousePositionZeroToOne().y < epsilon)
		{
			movement.z = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().y > 1.f - epsilon)
		{
			movement.z = 1.f;
		}

		return movement;
	}

	bool GUIManager::MouseOverGUI()
	{
		return myActiveWidget != nullptr && myActiveWidget != myWidgets;
	}

	void GUIManager::ReadXML(const std::string& aXMLPath)
	{
		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
		tinyxml2::XMLElement* containerElement = reader.FindFirstChild(rootElement, "container");
		tinyxml2::XMLElement* XMLPathElement = reader.FindFirstChild(rootElement, "file");

		ReadContainers(reader, containerElement);
		ReadFiles(reader, XMLPathElement);

		reader.CloseDocument();
	}

	void GUIManager::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myWidgets->SetPosition(aPosition);
	}

	void GUIManager::SetButtonText(int aButtonId, const std::string& aText)
	{
		bool success(false);
		myWidgets->SetButtonText(aButtonId, aText, success);
		DL_ASSERT_EXP(success == true, CU::Concatenate("ID: %d, Text: %s, Failed to set button text.", aButtonId, aText.c_str()));
	}

	void GUIManager::SetMouseShouldRender(bool aShouldRender)
	{
		myCursor->SetShouldRender(aShouldRender);
	}

	void GUIManager::SelectButtonDown()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();

		myControllerButtonIndexX++;
		if (myControllerButtonIndexX > myButtons.Size() - 1)
		{
			myControllerButtonIndexX = 0;
		}

		if (myControllerButtonIndexY > myButtons[myControllerButtonIndexX].Size() - 1)
		{
			myControllerButtonIndexY = myButtons[myControllerButtonIndexX].Size() - 1;
		}

		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::SelectButtonUp()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();

		myControllerButtonIndexX--;
		if (myControllerButtonIndexX < 0)
		{
			myControllerButtonIndexX = myButtons.Size() - 1;
		}

		if (myControllerButtonIndexY > myButtons[myControllerButtonIndexX].Size() - 1)
		{
			myControllerButtonIndexY = myButtons[myControllerButtonIndexX].Size() - 1;
		}

		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::SelectButtonRight()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();

		myControllerButtonIndexY++;
		if (myControllerButtonIndexY > myButtons[myControllerButtonIndexX].Size() - 1)
		{
			if (myControllerButtonIndexX + 1 < myButtons.Size() &&
				myButtons[myControllerButtonIndexX].Size() > 1 &&
				myButtons[myControllerButtonIndexX + 1].Size() > 1)
			{
				myControllerButtonIndexX++;
				myControllerButtonIndexY = 0;
			}
			else
			{
				myControllerButtonIndexY--;
			}
		}

		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::SelectButtonLeft()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();

		myControllerButtonIndexY--;
		if (myControllerButtonIndexY < 0)
		{
			if (myControllerButtonIndexX > 0 &&
				myButtons[myControllerButtonIndexX].Size() > 1 &&
				myButtons[myControllerButtonIndexX - 1].Size() > 1)
			{
				myControllerButtonIndexX--;
				myControllerButtonIndexY = myButtons[myControllerButtonIndexX].Size() - 1;
			}
			else
			{
				myControllerButtonIndexY++;
			}
		}

		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::PressSelectedButton()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnLeftMouseUp();
	}

	void GUIManager::HoverSelectedButton()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::HoverSelectedButtonWithoutSound()
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter(false);
	}

	void GUIManager::CheckUnlockedNightmareLevels()
	{
		CU::GrowingArray<bool> unlockedLevels(64);

		std::fstream file;
		file.open(CU::GetMyDocumentFolderPath() + "Data/UnlockedLevels_Nightmare.bin", std::ios::binary | std::ios::in);

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
				unlockedLevels.Add(isUnlocked);
			}

			file.close();
		}

		for (int i = 0; i < myLevelButtons.Size(); i++)
		{
#ifdef RELEASE_BUILD
			if (i == 0)
			{
				myLevelButtons[i]->SetActive(true);
			}
			else if (i > unlockedLevels.Size())
			{
				myLevelButtons[i]->SetActive(false);
			}
			else
			{
				myLevelButtons[i]->SetActive(unlockedLevels[i]);
			}
#else
			myLevelButtons[i]->SetActive(true);
#endif
			if (GC::EnableCheat == true)
			{
				myLevelButtons[i]->SetActive(true);
			}
			CU::Vector2<float> textOffset;
			textOffset.x = i + 1 > 9 ? -20.f : -10.f;
			textOffset.y = 20.f;
			myLevelButtons[i]->SetButtonText(std::to_string(i + 1), textOffset);
		}

		if (GC::TotalNightmareLevels == unlockedLevels.Size() && unlockedLevels.GetLast() == true)
		{
			GC::HasBeenInVictoryScreenNightmare = true;
		}

		for (int i = 0; i < myLevelButtons.Size(); i++)
		{
			int stars = 0;
			std::fstream file;
			file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(i + 1 + 1000) + ".bin", std::ios::binary | std::ios::in);

			if (file.is_open() == true)
			{
				int levelID = 0;
				float time = 0;
				file >> levelID >> time >> stars;
			}

			myLevelButtons[i]->SetStars(stars);
			file.close();
		}
	}

	void GUIManager::ReceiveMessage(const UnhoverControllerMessage&)
	{
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();
		myControllerButtonIndexX = 0;
		myControllerButtonIndexY = 0;
	}

	void GUIManager::Unhover()
	{
		if (myActiveWidget != nullptr)
		{
			myActiveWidget->OnMouseExit();
			myActiveWidget = nullptr;
		}
	}

	void GUIManager::Pause()
	{
		myIsPaused = true;
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseExit();
	}

	void GUIManager::UnPause()
	{
		myIsPaused = false;
		myButtons[myControllerButtonIndexX][myControllerButtonIndexY]->OnMouseEnter();
	}

	void GUIManager::ReadContainers(XMLReader& aReader, tinyxml2::XMLElement* aContainerElement)
	{
		std::string path = "";
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		for (; aContainerElement != nullptr; aContainerElement = aReader.FindNextElement(aContainerElement, "container"))
		{
			bool isFullscreen = false;
			bool isClickable = true;
			bool isScrolling = false;
			Prism::SpriteProxy* backgroundSprite = nullptr;
			Prism::SpriteProxy* vignetteSprite = nullptr;

			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "size"), "x", size.x);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "size"), "y", size.y);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "position"), "x", position.x);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "position"), "y", position.y);

			tinyxml2::XMLElement* spriteElement = aReader.FindFirstChild(aContainerElement, "backgroundsprite");
			tinyxml2::XMLElement* spriteSizeElement = aReader.FindFirstChild(aContainerElement, "backgroundsize");
			tinyxml2::XMLElement* fullscreenElement = aReader.FindFirstChild(aContainerElement, "isfullscreen");
			tinyxml2::XMLElement* clickableElement = aReader.FindFirstChild(aContainerElement, "isclickable");
			tinyxml2::XMLElement* scrollingElement = aReader.FindFirstChild(aContainerElement, "isscrolling");
			tinyxml2::XMLElement* vignetteElement = aReader.FindFirstChild(aContainerElement, "vignettesprite");

			if (spriteElement != nullptr)
			{
				aReader.ForceReadAttribute(spriteElement, "path", path);

				if (scrollingElement != nullptr)
				{
					aReader.ForceReadAttribute(scrollingElement, "value", isScrolling);
				}
				if (spriteSizeElement != nullptr)
				{
					CU::Vector2<float> spriteSize;
					aReader.ForceReadAttribute(spriteSizeElement, "x", spriteSize.x);
					aReader.ForceReadAttribute(spriteSizeElement, "y", spriteSize.y);
					if (isScrolling == true)
					{
						spriteSize.x = spriteSize.y * 2.f;
					}
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, spriteSize);
				}
				else
				{
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, size);
				}
			}

			if (vignetteElement != nullptr)
			{
				aReader.ForceReadAttribute(vignetteElement, "path", path);

				vignetteSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, size);
			}

			if (fullscreenElement != nullptr)
			{
				aReader.ForceReadAttribute(fullscreenElement, "value", isFullscreen);
			}

			if (clickableElement != nullptr)
			{
				aReader.ForceReadAttribute(clickableElement, "value", isClickable);
			}

			GUI::WidgetContainer* container = new WidgetContainer(backgroundSprite, vignetteSprite, size, isFullscreen, isScrolling);
			container->SetPosition(position);

			tinyxml2::XMLElement* widgetElement = aReader.FindFirstChild(aContainerElement, "widget");
			for (; widgetElement != nullptr; widgetElement = aReader.FindNextElement(widgetElement))
			{
				std::string type = "";

				aReader.ForceReadAttribute(widgetElement, "type", type);

				if (type == "button")
				{
					ButtonWidget* widget = new ButtonWidget(&aReader, widgetElement);
					container->AddWidget(widget);
					if (widget->GetEvent()->myEvent == eOnClickEvent::PLAYER_1_LEFT
						|| widget->GetEvent()->myEvent == eOnClickEvent::PLAYER_1_RIGHT
						|| widget->GetEvent()->myEvent == eOnClickEvent::PLAYER_2_LEFT
						|| widget->GetEvent()->myEvent == eOnClickEvent::PLAYER_2_RIGHT)
					{
						continue;
					}
					if (myButtons.Size() == 0 || widget->GetPosition().y > myButtons.GetLast().GetLast()->GetPosition().y)
					{
						myButtons.Add(CU::GrowingArray<Widget*>(8));
						myButtons.GetLast().Add(widget);
					}
					else if (widget->GetPosition().x > myButtons.GetLast().GetLast()->GetPosition().x)
					{
						myButtons.GetLast().Add(widget);
					}
					else
					{
						myButtons.Add(CU::GrowingArray<Widget*>(8));
						myButtons.GetLast().Add(widget);
					}

				}
				else if (type == "sprite")
				{
					SpriteWidget* widget = new SpriteWidget(&aReader, widgetElement);
					container->AddWidget(widget);
				}
				else if (type == "toggleBox")
				{
					ToggleBoxWidget* widget = new ToggleBoxWidget(&aReader, widgetElement);
					myButtons.Add(CU::GrowingArray<Widget*>(1));
					myButtons.GetLast().Add(widget);
					container->AddWidget(widget);
				}
				else if (type == "highscore")
				{
					HighscoreWidget* widget = new HighscoreWidget(&aReader, widgetElement, myLevelID);
					container->AddWidget(widget);
				}
				else if (type == "volume")
				{
					VolumeWidget* widget = new VolumeWidget(&aReader, widgetElement);
					myButtons.Add(CU::GrowingArray<Widget*>(2));
					myButtons.GetLast().Add(widget->GetDecreseButton());
					container->AddWidget(widget->GetDecreseButton());
					myButtons.GetLast().Add(widget->GetIncreseButton());
					container->AddWidget(widget->GetIncreseButton());
					container->AddWidget(widget);
				}
				else if (type == "buttonMatrix")
				{
					ButtonMatrixWidget* widget = new ButtonMatrixWidget(&aReader, widgetElement);
					for (int row = 0; row < widget->GetSize().y; ++row)
					{
						myButtons.Add(widget->GetButtons(row));
						for each(ButtonWidget* button in widget->GetButtons(row))
						{
							container->AddWidget(button);
						}
					}
					container->AddWidget(widget);
				}
				else if (type == "text")
				{
					TextWidget* widget = new TextWidget(&aReader, widgetElement);
					container->AddWidget(widget);
				}
				else if (type == "levelButton")
				{
					LevelButtonWidget* widget = new LevelButtonWidget(&aReader, widgetElement);
					container->AddWidget(widget);

					if (myButtons.Size() == 0 || widget->GetPosition().y > myButtons.GetLast().GetLast()->GetPosition().y)
					{
						myButtons.Add(CU::GrowingArray<Widget*>(8));
						myButtons.GetLast().Add(widget);
					}
					else if (widget->GetPosition().x > myButtons.GetLast().GetLast()->GetPosition().x)
					{
						myButtons.GetLast().Add(widget);
					}
					else
					{
						myButtons.Add(CU::GrowingArray<Widget*>(8));
						myButtons.GetLast().Add(widget);
					}
					myLevelButtons.Add(widget);
				}
				else
				{
					std::string message = "[GUI manager] no widget type named " + type;
					DL_ASSERT(message);
				}
			}
			container->SetIsClickable(isClickable);
			myWidgets->AddWidget(container);
		}
	}

	void GUIManager::ReadFiles(XMLReader& aReader, tinyxml2::XMLElement* aFilePathElement)
	{
		for (; aFilePathElement != nullptr; aFilePathElement = aReader.FindNextElement(aFilePathElement, "file"))
		{
			std::string filePath = "";

			aReader.ForceReadAttribute(aFilePathElement, "path", filePath);

			ReadXML(filePath);
		}
	}

	void GUIManager::CheckMousePressed()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
			{
				myActiveWidget->OnLeftMousePressed(myMousePosition);
			}
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(1) == true)
			{
				myActiveWidget->OnRightMousePressed(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseDown()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
			{
				myActiveWidget->OnLeftMouseDown(myMousePosition);
			}
			if (CU::InputWrapper::GetInstance()->MouseDown(1) == true)
			{
				myActiveWidget->OnRightMouseDown(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseReleased()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseUp(0) == true)
			{
				myActiveWidget->OnLeftMouseUp();
				myActiveWidget->OnMouseEnter(); // hover button again after pressing it
			}
			if (CU::InputWrapper::GetInstance()->MouseUp(1) == true)
			{
				myActiveWidget->OnRightMouseUp();
				myActiveWidget->OnMouseEnter(); // hover button again after pressing it
			}
		}
	}

	void GUIManager::CheckMouseMoved()
	{
		if (myActiveWidget != nullptr)
		{
			if (myMousePosition.x != 0 || myMousePosition.y != 0)
			{
				myActiveWidget->OnMouseMoved(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseEntered()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (activeWidget != nullptr && activeWidget->IsClickable() == false)
		{
			return;
		}

		if (activeWidget != nullptr && activeWidget != myActiveWidget)
		{
			activeWidget->OnMouseEnter();
			myActiveWidget = activeWidget;
		}
	}

	void GUIManager::CheckMouseExited()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (myActiveWidget != activeWidget)
		{
			if (myActiveWidget != nullptr)
			{
				myActiveWidget->OnMouseExit();
				myActiveWidget = nullptr;
			}
		}
	}
}