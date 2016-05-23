#include "stdafx.h"
#include "ParticlePass.h"
#include "EffectContainer.h"

namespace Prism
{
	ParticlePass::ParticlePass()
	{
	}


	ParticlePass::~ParticlePass()
	{
	}

	Effect* ParticlePass::GetEffect()
	{
		return myEffect;
	}

	ID3DX11EffectShaderResourceVariable* ParticlePass::GetShaderResource()
	{
		return myTexture;
	}

};