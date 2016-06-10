#pragma once
#include <map>
#include "SpriteAnimation.h"
#include <functional>

namespace Prism
{
	class SpriteProxy;

	class SpriteAnimator
	{
	public:
		SpriteAnimator(std::string aXMLPath);
		~SpriteAnimator();

		void Update(float aDelta);
		void Render(CU::Vector2<float> aPosition);

		void StartAnimation(int anID = 0);
		void RestartAnimation(int anID = 0);
		void StopAnimation();
		void ResetAnimation();
		void PauseAnimation();
		void PauseAnimationAtLastFrame();
		void UnPauseAnimation();

		void AddAnimation(float aFPS, int aNumberOfFrames, CU::Vector2<float> aFrameSize
			, SpriteProxy* aSpriteSheet, bool aIsLooping);

		SpriteProxy* GetSpriteFrame();
		bool IsPlayingAnimation() const;
		int GetCurrentFrame();

		const CU::Vector2<float>& GetFrameSize() const;

		void SetTimesToRunAnimation(int aNumber);

		void SetShouldStopAtLastFrame(bool aShouldStop);

		void SetAnimationDoneCallback(std::function<void()> anAnimationDoneCallback);

	private:
		float myTimeSinceLastFrame;
		int myCurrentFrame;
		CU::GrowingArray<SpriteAnimation*> myAnimations;
		SpriteAnimation *myCurrentAnimation;
		SpriteProxy* emptySprite;

		bool myIsPaused;
		bool myShouldStopAtLastFrame;
		bool myCallCallbackLastTime;
		
		int myTimesToRunAnimation;

		std::function<void()> myAnimationDoneCallback;
	};

	inline void SpriteAnimator::SetTimesToRunAnimation(int aNumber)
	{
		myTimesToRunAnimation = aNumber;
	}

	inline void SpriteAnimator::SetShouldStopAtLastFrame(bool aShouldStop)
	{
		myShouldStopAtLastFrame = aShouldStop;
	}

	inline void SpriteAnimator::SetAnimationDoneCallback(std::function<void()> anAnimationDoneCallback)
	{
		myAnimationDoneCallback = anAnimationDoneCallback;
	}
}