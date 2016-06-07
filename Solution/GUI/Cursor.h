#pragma once
#include <Vector2.h>
#include <GameEnum.h>

namespace Prism
{
	class SpriteProxy;
}

namespace CU
{
	class InputWrapper;
}

namespace GUI
{
	class Cursor
	{
	public:
		Cursor(const CU::Vector2<int>& aWindowSize);
		~Cursor();

		void Update();
		void Render();

		const CU::Vector2<float>& GetMousePosition() const;
		const CU::Vector2<float>& GetMousePositionZeroToOne() const;
		void OnResize(int aWidth, int aHeight);

		void ClipCursor(); // for software cursor

		void SetCurrentCursor(eCursorType aType);
		eCursorType GetCurrentCursor() const;

		void SetShouldRender(bool aShouldRender);
		void SetIsUsingController(bool aIsUsingController);

		bool IsUsingController() const;

	private:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> myPreviousPosition;
		CU::Vector2<float> myPositionZeroToOne;
		CU::Vector2<float> myWindowSize;
		CU::GrowingArray<Prism::SpriteProxy*> mySprites;

		eCursorType myCurrentType;

		bool myShouldRender;
		bool myIsUsingController;
	};

	inline void Cursor::SetCurrentCursor(eCursorType aType)
	{
		myCurrentType = aType;
	}

	inline eCursorType Cursor::GetCurrentCursor() const
	{
		return myCurrentType;
	}

	inline void Cursor::SetShouldRender(bool aShouldRender)
	{
		myShouldRender = aShouldRender;
	}

	inline void Cursor::SetIsUsingController(bool aIsUsingController)
	{
		myIsUsingController = aIsUsingController;
		if (myIsUsingController == true)
		{
			myPreviousPosition = myPosition;
		}
	}

	inline bool Cursor::IsUsingController() const
	{
		return myIsUsingController;
	}
}