#pragma once
#include "Widget.h"
#include <Subscriber.h>
namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class Text;
}

class XMLReader;

namespace GUI
{
	class ButtonWidget;
	class VolumeWidget : public Widget, public Subscriber
	{
	public:
		VolumeWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~VolumeWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void ReceiveMessage(const OnClickMessage& aMessage) override;

		ButtonWidget* GetDecreseButton();
		ButtonWidget* GetIncreseButton();
	private:
		enum eVolumeType
		{
			VOLUME,
			MUSIC,
			VOICE,
		};

		ButtonWidget* myDecreaseButton;
		ButtonWidget* myIncreaseButton;
		std::string myText;
		std::string myTextValue;

		CU::Vector2<float> myTextPosition;

		int myValue;

		eVolumeType myType;
	};

	inline ButtonWidget* VolumeWidget::GetDecreseButton()
	{
		return myDecreaseButton;
	}

	inline ButtonWidget* VolumeWidget::GetIncreseButton()
	{
		return myIncreaseButton;
	}
}