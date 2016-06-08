#pragma once

#include <GrowingArray.h>
#include <unordered_map>
#include <GameEnum.h>



namespace Prism
{
	class Camera;
	class Model;

	struct ModelData
	{
		CU::GrowingArray<CU::Matrix44<float>> myMatrices;
		CU::GrowingArray<CU::Vector3<float>> myScales;
		CU::GrowingArray<float> myHeights;
	};
	typedef std::unordered_map<Model*, ModelData> ModelMap;

	class InstancingHelper
	{
	public:
		InstancingHelper();

		void SetCamera(const Camera* aCamera);
		void AddModel(Model* aModel, const CU::Matrix44<float>& aWorldMatrix
			, const CU::Vector3<float>& aScale, float aHeight);

		void Render(bool aIsOnlyDepth);
		void DebugPrint();

	private:
		void RenderModel(Model* aModel, Effect* aEffect, bool aIsOnlyDepth);

		const Camera* myCamera;
		int myDrawCalls;
		int myRenderedCount;
		int myIndexCount;
		int myOver10kCount;

		ModelMap myRenderInfo;
		CU::GrowingArray<Model*> myModels;
	};
}