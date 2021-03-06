#include "stdafx.h"
#include "Widget.h"

namespace GUI
{
	Widget::Widget()
	{
		myIsVisible = true;
		myIsFullscreen = false;
		myIsClickable = true;
	}

	Widget::~Widget()
	{
	}

	void Widget::Destroy()
	{
	}

	void Widget::Update(float)
	{
	}

	void Widget::Render(const CU::Vector2<float>&, float)
	{
	}

	void Widget::OnLeftMouseDown(const CU::Vector2<float>&)
	{
	}

	void Widget::OnRightMouseDown(const CU::Vector2<float>&)
	{
	}

	void Widget::OnLeftMousePressed(const CU::Vector2<float>&)
	{
	}

	void Widget::OnRightMousePressed(const CU::Vector2<float>&)
	{
	}

	void Widget::OnLeftMouseUp()
	{
	}

	void Widget::OnRightMouseUp()
	{
	}

	void Widget::OnMouseEnter(bool)
	{
	}

	void Widget::OnMouseExit()
	{
	}

	void Widget::OnMouseMoved(const CU::Vector2<float>&)
	{
	}

	Widget*	Widget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

	bool Widget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return	aPosition.x >= myPosition.x &&
			aPosition.y >= myPosition.y &&
			aPosition.x <= myPosition.x + mySize.x &&
			aPosition.y <= myPosition.y + mySize.y;
	}

	void Widget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		if (myIsFullscreen == false)
		{
			CU::Vector2<float> ratioPostion = myPosition / anOldWindowSize.x;
			myPosition = ratioPostion * aNewWindowSize.x;

			CU::Vector2<float> ratioSize = mySize / anOldWindowSize.x;
			mySize = ratioSize * aNewWindowSize.x;
		}
		else
		{
			CU::Vector2<float> ratioPostion = myPosition / anOldWindowSize;
			myPosition = ratioPostion * aNewWindowSize;

			CU::Vector2<float> ratioSize = mySize / anOldWindowSize;
			mySize = ratioSize * aNewWindowSize;
		}
	}
}