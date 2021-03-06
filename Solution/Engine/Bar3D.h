#pragma once

#include "BaseModel.h"
#include <Matrix.h>

enum class eBarPosition
{
	AIM,
	LEFT,
	TOP,
	RIGHT,
	HEALTH,
	HEALTH_ICON,
	PISTOL_ICON,
	SHOTGUN_ICON,
	GRENADE_LAUNCHER_ICON,
};

namespace Prism
{
	class Effect;
	class Bar3D : public BaseModel
	{
	public:
		Bar3D(float aInner, float aOuter, int aNumberOfQuads, Effect* aEffect
			, eBarPosition aBarPosition, const std::string& aTexturePath);
		~Bar3D();

		void Render(const Camera& aCamera, const CU::Matrix44<float>& aWorld
			, const CU::Vector4<float>& aColor = { 0.f, 0.f, 0.f, 0.f });
		void Render(bool aIsOnlyDepth) override;

		void SetValue(float aValue);

		void Rebuild(int aNumberOfQuads);

	private:
		void CreateVertices(int aNumberOfQuads);
		float myInner;
		float myOuter;

		float myValue;
		int myNbrOfQuads;

		eBarPosition myBarPosition;
	};

}