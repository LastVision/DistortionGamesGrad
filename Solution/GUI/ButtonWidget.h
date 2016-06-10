#pragma once
#include "Widget.h"

#include <Tweener.h>

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
struct OnClickMessage;

namespace GUI
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		ButtonWidget(const CU::Vector2<float>& aSize, const CU::Vector2<float>& aPosition, 
			const std::string& aSpritePath, const std::string& aSpriteHoverPath, const std::string& aSpritePressedPath, 
			const std::string& aButtonText = "default", const std::string& = "", const CU::Vector2<float>& aTextOffset = { 0.f, 0.f });
		virtual ~ButtonWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition, float anAlpha) override;

		void OnLeftMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnLeftMouseUp() override;
		void OnMouseEnter(bool aShouldSound = true) override;
		void OnMouseExit() override;
		void Update(float aDeltaTime) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true) override;

		void ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		
		void SetEvent(OnClickMessage* anEvent);
		const OnClickMessage* GetEvent() const;

		void SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut) override;
		void SetButtonText(const std::string& aText, const CU::Vector2<float>& aOffset = { 0.f, 0.f }) override;

		void SetActive(const bool aActiveFlag);

		void SwitchGradient(bool aShouldGradient) override;

	protected:
		void Click();

		CU::Vector4<float> myColor;

		int myId;

		std::string myHoverText;
		std::string myButtonText;
		bool myIsTextButton;
		bool myCanBeClicked;
		
		Prism::SpriteProxy* myImageNormal;
		Prism::SpriteProxy* myImagePressed;
		Prism::SpriteProxy* myImageHover;
		Prism::SpriteProxy* myImageCurrent;

		OnClickMessage* myClickEvent;

		CU::Vector2<float> myTextOffset;

		bool myShouldBeBigger;
		bool myShouldBeSmaller;
		float myLerpAlpha;
		float myScale;
		float myLerpScale;
		CU::Vector2<float> myOriginalSize;
		CU::Vector2<float> myOriginalHotSpot;

		bool myShouldAnimate;
		float myAnimationAlpha;
		float myAnimationStart;

		Tweener<float> myTweener;

		bool myIsGradient;
		bool myGradientIsIncreasing;
		float myGradient;

		CU::Vector2<float> myHotSpot;
	};

	inline const OnClickMessage* ButtonWidget::GetEvent() const
	{
		return myClickEvent;
	}

	inline void ButtonWidget::SetEvent(OnClickMessage* anEvent)
	{
		myClickEvent = anEvent;
	}

	inline void ButtonWidget::SetButtonText(const std::string& aText, const CU::Vector2<float>& aOffset)
	{
		myTextOffset = aOffset;
		myButtonText = aText;
		myIsTextButton = true;
	}
}