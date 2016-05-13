#pragma once

#include "EffectListener.h"

struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;

namespace Prism
{
	class Effect;

	struct ShadowPass : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* mySceneAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* mySceneDepth = nullptr;
		ID3DX11EffectShaderResourceVariable* myShadowDepth = nullptr;

		ID3DX11EffectMatrixVariable* myShadowMVP;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;
	};
}
