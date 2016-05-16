#pragma once

struct ID3DX11EffectShaderResourceVariable;
namespace Prism
{

	class Effect;

	class ParticlePass
	{
	public:
		ParticlePass();
		~ParticlePass();

		Effect* GetEffect();
		ID3DX11EffectShaderResourceVariable* GetShaderResource();

	private:

		ID3DX11EffectShaderResourceVariable* myTexture;
		Effect* myEffect;

	};
};