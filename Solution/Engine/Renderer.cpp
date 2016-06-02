#include "stdafx.h"

#include "GBufferData.h"
#include "FullScreenHelper.h"
#include "Texture.h"
#include "Renderer.h"
#include "Scene.h"
#include "SpotLightShadow.h"
#include <D3DX11.h>

namespace Prism
{
	Renderer::Renderer()
	{
		CU::Vector2<float> screenSize(static_cast<float>(Engine::GetInstance()->GetWindowSize().x)
			, static_cast<float>(Engine::GetInstance()->GetWindowSize().y));

		myFinalTexture = new Texture();
		myFinalTexture->Init(screenSize.x, screenSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
			, DXGI_FORMAT_R16G16B16A16_FLOAT);

		myFullScreenHelper = new FullScreenHelper();

		myClearColor[0] = 0.f;
		myClearColor[1] = 0.f;
		myClearColor[2] = 0.f;
		myClearColor[3] = 0.f;

		myShadowViewport = new D3D11_VIEWPORT[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
		myShadowViewport->Height = 1024 * 2;
		myShadowViewport->Width = 1024 * 2;
		myShadowViewport->TopLeftX = 0;
		myShadowViewport->TopLeftY = 0;
		myShadowViewport->MinDepth = 0;
		myShadowViewport->MaxDepth = 1;
	}
	
	Renderer::~Renderer()
	{
		SAFE_DELETE(myShadowViewport);
		SAFE_DELETE(myFinalTexture);
		SAFE_DELETE(myFullScreenHelper);
	}

	void Renderer::ProcessShadow(SpotLightShadow* aShadowSpotLight, Scene* aScene)
	{
		aShadowSpotLight->ClearTexture();
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		UINT nbrOfVp = 1;
		D3D11_VIEWPORT oldVp;
		context->RSGetViewports(&nbrOfVp, &oldVp);
		context->RSSetViewports(1, myShadowViewport);
		

		ID3D11RenderTargetView* originalRenderTargetView;
		ID3D11DepthStencilView* originalDepthStencilView;
		context->OMGetRenderTargets(1, &originalRenderTargetView, &originalDepthStencilView);

		ID3D11RenderTargetView* view = aShadowSpotLight->GetTexture()->GetRenderTargetView();

		context->OMSetRenderTargets(1, &view, aShadowSpotLight->GetTexture()->GetDepthStencilView());
		const Camera* oldCamera = aScene->GetCamera();
		//aScene->SetViewCamera(*oldCamera);
		aScene->SetCamera(*aShadowSpotLight->GetCamera());

		aScene->RenderDepth();

		context->OMSetRenderTargets(1, &originalRenderTargetView, originalDepthStencilView);

		int IsThisNeededToPreventGPUMemoryCrash = 5;
		//originalDepthStencilView->Release();
		//originalRenderTargetView->Release();

		context->RSSetViewports(1, &oldVp);
		aScene->SetCamera(*oldCamera);
	}

	void Renderer::Render(Texture* aSource, Texture* aEmissiveTexture, Texture* aDepthStencilTexture, int aEffect)
	{
		Engine::GetInstance()->GetContex()->ClearRenderTargetView(myFinalTexture->GetRenderTargetView(), myClearColor);

		myFullScreenHelper->Process(aSource, myFinalTexture, aEmissiveTexture, aEffect);

		myFullScreenHelper->RenderToScreen(myFinalTexture);

		Engine::GetInstance()->SetDepthStencil(aDepthStencilTexture->GetDepthStencilView());
	}

	void Renderer::DebugRender(GBufferData* aGBuffer)
	{
		std::string renderMode;
		if (GC::DebugRenderTexture == 1)
		{
			renderMode = "Albedo";
			myFullScreenHelper->RenderToScreen(aGBuffer->myAlbedoTexture);
		}
		else if (GC::DebugRenderTexture == 2)
		{
			renderMode = "Normal";
			myFullScreenHelper->RenderToScreen(aGBuffer->myNormalTexture);
		}
		else if (GC::DebugRenderTexture == 3)
		{
			renderMode = "Emissive";
			myFullScreenHelper->RenderToScreen(aGBuffer->myEmissiveTexture);
		}
		else if (GC::DebugRenderTexture == 4)
		{
			renderMode = "Depth";
			myFullScreenHelper->RenderToScreen(aGBuffer->myDepthTexture);
		}

		DEBUG_PRINT(renderMode);
	}

	void Renderer::SetRenderTargets(ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthBuffer)
	{
		ID3D11RenderTargetView* target = aRenderTarget;
		ID3D11DepthStencilView* depth = aDepthBuffer;
		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &target, depth);
	}
}
