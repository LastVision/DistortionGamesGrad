#include "stdafx.h"

#include "DeferredPasses.h"
#include <d3dx11effect.h>
#include "Effect.h"

namespace Prism
{
	void RenderToScreenData::OnEffectLoad()
	{
		mySource = myEffect->GetEffect()->GetVariableByName("DiffuseTexture")->AsShaderResource();
	}
}