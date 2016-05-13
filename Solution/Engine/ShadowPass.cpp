#include "stdafx.h"

#include <d3dx11effect.h>
#include "Effect.h"
#include "ShadowPass.h"

namespace Prism
{
	void ShadowPass::OnEffectLoad()
	{
		mySceneAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		mySceneDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myShadowDepth = myEffect->GetEffect()->GetVariableByName("ShadowDepth")->AsShaderResource();

		myShadowMVP = myEffect->GetEffect()->GetVariableByName("ShadowMVP")->AsMatrix();
		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();
	}
}
