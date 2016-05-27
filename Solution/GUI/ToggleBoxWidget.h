#pragma once
#include "Widget.h"

struct OnClickMessage;

namespace GUI
{
	class ToggleBoxWidget : public Widget
	{
	public:
		ToggleBoxWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~ToggleBoxWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnLeftMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnLeftMouseUp() override;
		void OnMouseEnter() override;
		void OnMouseExit() override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true) override;

		void ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);

		void SetEvent(OnClickMessage* anEvent);
		const OnClickMessage* GetEvent() const;

		void SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut) override;

		void SetToggleState(const bool aFlag);
	protected:
		void Click();

		CU::Vector4<float> myColor;

		int myId;
		std::string myText;
		bool myCanBeClicked;

		bool myToggledState;

		Prism::SpriteProxy* myImageActive;
		Prism::SpriteProxy* myImageActiveHover;
		Prism::SpriteProxy* myImageDeactive;
		Prism::SpriteProxy* myImageDeactiveHover;
		Prism::SpriteProxy* myImageCurrent;

		OnClickMessage* myClickEvent;
	};

	inline void ToggleBoxWidget::SetEvent(OnClickMessage* anEvent)
	{
		myClickEvent = anEvent;
	}

	inline const OnClickMessage* ToggleBoxWidget::GetEvent() const
	{
		return myClickEvent;
	}
}
