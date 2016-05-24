#pragma once

#include "LightStructs.h"

namespace Prism
{
	class DirectionalLight
	{
	public:
		const CU::Vector4<float>& GetColor() const;
		const CU::Vector4<float>& GetDir() const;

		void SetColor(const CU::Vector4<float>& aVector);
		void SetDir(const CU::Vector4<float>& aVector);

		const DirectionalLightData& GetData() const;

	private:
		void Update();

		CU::Vector4<float> myColor;
		CU::Vector4<float> myDirection;
		DirectionalLightData myData;
	};

	inline void DirectionalLight::Update()
	{
		myData.myColor = GetColor();
		myData.myDirection = GetDir();
	}

	inline const CU::Vector4<float>& DirectionalLight::GetColor() const
	{
		return myColor;
	}

	inline const CU::Vector4<float>& DirectionalLight::GetDir() const
	{
		return myDirection;
	}

	inline void DirectionalLight::SetColor(const CU::Vector4<float>& aVector)
	{
		myColor = aVector;
		Update();
	}

	inline void DirectionalLight::SetDir(const CU::Vector4<float>& aVector)
	{
		myDirection = aVector;
		Update();
	}

	inline const DirectionalLightData& DirectionalLight::GetData() const
	{
		return myData;
	}
}