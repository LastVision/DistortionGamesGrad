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

	struct RenderToScreenData : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* mySource = nullptr;
	};
}
