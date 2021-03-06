#pragma once
#include <Subscriber.h>

namespace CU
{
	class InputWrapper;
}

class Entity;

namespace Prism
{
	class Camera;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class WidgetContainer;
	class Widget;
	class ButtonWidget;
	class Cursor;
	class LevelButtonWidget;

	class GUIManager : public Subscriber
	{
	public:
		GUIManager(Cursor* aCursor, const std::string& aXMLPath, const Prism::Camera* aCamera, int aLeveID);
		~GUIManager();

		void AddWidget(Widget* aWidget);
		void Update(float aDelta);
		void Render(float anAlpha = 1.f);

		void OnResize(int aWidth, int aHeight);

		CU::Vector3<float> CalcCameraMovement() const;

		bool MouseOverGUI();

		void ReadXML(const std::string& aXMLPath);

		const Widget* GetActiveWidget() const;

		void SetPosition(const CU::Vector2<float>& aPosition);

		WidgetContainer* GetWidgetContainer();

		void SetButtonText(int aButtonId, const std::string& aText);

		void SetMouseShouldRender(bool aShouldRender);

		void SetSelectedButton(int aX, int aY);

		void SelectButtonDown();
		void SelectButtonUp();
		void SelectButtonRight();
		void SelectButtonLeft();
		void PressSelectedButton();
		void HoverSelectedButton();
		void HoverSelectedButtonWithoutSound();

		void CheckUnlockedNightmareLevels();

		void ReceiveMessage(const UnhoverControllerMessage& aMessage) override;

		void Unhover();

		void Pause();
		void UnPause();

		bool IsPaused() const;

	private:
		void ReadContainers(XMLReader& aReader, tinyxml2::XMLElement* aContainerElement);
		void ReadFiles(XMLReader& aReader, tinyxml2::XMLElement* aFilePathElement);

		void CheckMousePressed();
		void CheckMouseDown();
		void CheckMouseReleased();
		void CheckMouseMoved();
		void CheckMouseEntered();
		void CheckMouseExited();

		WidgetContainer* myWidgets;

		CU::GrowingArray<CU::GrowingArray<Widget*>> myButtons;
		CU::GrowingArray<LevelButtonWidget*> myLevelButtons; // for nightmare mode

		const Prism::Camera* myCamera;

		Widget* myActiveWidget;
		Cursor* myCursor;

		CU::Vector2<float> myMousePosition;
		CU::Vector2<float> myWindowSize;

		int myLevelID;

		int myControllerButtonIndexX;
		int myControllerButtonIndexY;
		bool myUseController;
		bool myIsPaused;
	};

	inline const Widget* GUIManager::GetActiveWidget() const
	{
		return myActiveWidget;
	}

	inline WidgetContainer* GUIManager::GetWidgetContainer()
	{
		return myWidgets;
	}

	inline void GUIManager::SetSelectedButton(int aX, int aY)
	{
		myControllerButtonIndexX = abs(aX);
		myControllerButtonIndexY = abs(aY);

		if (myControllerButtonIndexX > myButtons.Size() - 1)
		{
			myControllerButtonIndexX = myButtons.Size() - 1;
		}

		if (myControllerButtonIndexY > myButtons[myControllerButtonIndexX].Size() - 1)
		{
			myControllerButtonIndexY = myButtons[myControllerButtonIndexX].Size() - 1;
		}
	}

	inline bool GUIManager::IsPaused() const
	{
		return myIsPaused;
	}
}