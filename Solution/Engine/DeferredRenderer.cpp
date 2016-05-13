#include "stdafx.h"

#include "AmbientPass.h"
#include "PointLightPass.h"
#include "GBufferData.h"
#include "SpotLightPass.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "DeferredRenderer.h"
#include "IndexBufferWrapper.h"
#include "Texture.h"
#include "Scene.h"
#include "SpriteProxy.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SpotLightShadow.h"
#include "SpotLightTextureProjection.h"
#include <XMLReader.h>
#include "EmitterManager.h"
namespace Prism
{
	DeferredRenderer::DeferredRenderer()
	{
		CU::Vector2<float> windowSize = Engine::GetInstance()->GetWindowSize();

		myViewPort = new D3D11_VIEWPORT();
		ZeroMemory(myViewPort, sizeof(D3D11_VIEWPORT));

		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = windowSize.x;
		myViewPort->Height = windowSize.y;
		myViewPort->MinDepth = 0.f;
		myViewPort->MaxDepth = 1.f;

		myDepthStencilTexture = new Texture();
		myDepthStencilTexture->InitAsDepthBuffer(windowSize.x, windowSize.y);

		myClearColor[0] = 0.f;
		myClearColor[1] = 0.f;
		myClearColor[2] = 0.f;
		myClearColor[3] = 0.f;

		myRenderToScreenData.myEffect = 
			EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_render_to_texture.fx");
		myRenderToScreenData.OnEffectLoad();
		myRenderToScreenData.myEffect->AddListener(&myRenderToScreenData);

		myAmbientPass = new AmbientPass();
		myPointLightPass = new PointLightPass();
		mySpotLightPass = new SpotLightPass("Data/Resource/Shader/S_effect_deferred_light_mesh_spot.fx");
		mySpotLightTextureProjectionPass = new SpotLightPass("Data/Resource/Shader/S_effect_deferred_light_mesh_spot_textureprojection.fx");

		myGBufferData = new GBufferData();
		SetupShadowData();

		InitFullscreenQuad();

		myCubemap = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/CubeMap/T_cubemap_level01.dds");

		myFinishedTexture = new Texture();
		myFinishedTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myFinishedSceneTexture = new Texture();
		myFinishedSceneTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	DeferredRenderer::~DeferredRenderer()
	{
		SAFE_DELETE(myFinishedTexture);
		SAFE_DELETE(myFinishedSceneTexture);
		SAFE_DELETE(myViewPort);
		SAFE_DELETE(myDepthStencilTexture);

		SAFE_DELETE(myAmbientPass);
		SAFE_DELETE(myGBufferData);
		SAFE_DELETE(myPointLightPass);
		SAFE_DELETE(mySpotLightPass);
		SAFE_DELETE(mySpotLightTextureProjectionPass);
	}

	void DeferredRenderer::Render(Scene* aScene, Prism::SpriteProxy* aBackground, Prism::SpotLightShadow* aShadowLight, EmitterManager* aParticleEmitterManager)
	{
		Engine::GetInstance()->GetContex()->RSSetViewports(1, myViewPort);
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myDepthStencilTexture->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		myGBufferData->Clear(myClearColor);
		myGBufferData->SetAsRenderTarget(myDepthStencilTexture);

		if (aBackground != nullptr)
		{
			aBackground->Render(Engine::GetInstance()->GetWindowSize() * 0.5f);
		}

		aScene->Render();

		ActivateBuffers();

		RenderDeferred(aScene);


		ID3D11RenderTargetView* targets = myFinishedSceneTexture->GetRenderTargetView();
		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &targets
			, myDepthStencilTexture->GetDepthStencilView());

		aParticleEmitterManager->RenderEmitters(0);


#ifdef SHADOWS
		RenderShadows(aShadowLight, aScene->GetCamera());
#endif
	}

	void DeferredRenderer::RenderShadows(Prism::SpotLightShadow* aShadowLight, const Prism::Camera* aCamera)
	{
		ActivateBuffers();
		Prism::EffectContainer::GetInstance()->SetShadowDepth(aShadowLight);
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		ID3D11RenderTargetView* backbuffer = myFinishedTexture->GetRenderTargetView();
		context->ClearRenderTargetView(backbuffer, myClearColor);
		context->ClearDepthStencilView(Engine::GetInstance()->GetDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->OMSetRenderTargets(1, &backbuffer
			, Engine::GetInstance()->GetDepthView());

		myShadowPass.mySceneAlbedo->SetResource(myFinishedSceneTexture->GetShaderView());
		myShadowPass.mySceneDepth->SetResource(myGBufferData->myDepthTexture->GetShaderView());
		myShadowPass.myShadowDepth->SetResource(aShadowLight->GetTexture()->GetDepthStencilShaderView());
		myShadowPass.myShadowMVP->SetMatrix(&aShadowLight->GetMVP().myMatrix[0]);
		myShadowPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera->GetProjection()).myMatrix[0]);
		myShadowPass.myNotInvertedView->SetMatrix(&aCamera->GetOrientation().myMatrix[0]);
		
		Render(myShadowPass.myEffect);
	}

	void DeferredRenderer::OnResize(float aWidth, float aHeight)
	{
		myGBufferData->myAlbedoTexture->Resize(aWidth, aHeight);
		myGBufferData->myNormalTexture->Resize(aWidth, aHeight);
		myGBufferData->myEmissiveTexture->Resize(aWidth, aHeight);
		myGBufferData->myDepthTexture->Resize(aWidth, aHeight);
		myDepthStencilTexture->Resize(aWidth, aHeight);
		myFinishedTexture->Resize(aWidth, aHeight);
	}

	void DeferredRenderer::SetCubeMap(const std::string& aFilePath)
	{
		ModelLoader::GetInstance()->Pause();
		myCubemap = TextureContainer::GetInstance()->GetTexture("LightData/" + aFilePath + "_cubemap.dds");
		ModelLoader::GetInstance()->UnPause();
	}

	Prism::Texture* DeferredRenderer::GetFinishedTexture()
	{
		return myFinishedTexture;
	}

	Prism::Texture* DeferredRenderer::GetEmissiveTexture()
	{
		return myGBufferData->myEmissiveTexture;
	}

	Prism::Texture* DeferredRenderer::GetDepthStencilTexture()
	{
		return myDepthStencilTexture;
	}

	void DeferredRenderer::InitFullscreenQuad()
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		D3DX11_PASS_DESC passDesc;
		myRenderToScreenData.myEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(vertexDesc
			, ARRAYSIZE(vertexDesc), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &myVertexLayout);
		if (FAILED(hr) != S_OK)
		{
			DL_MESSAGE_BOX("Failed to CreateInputLayout", "Model2D::Init", MB_ICONWARNING);
		}


		InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_IMMUTABLE, 0);
		InitIndexBuffer();

		CreateVertices();
	}

	void DeferredRenderer::CreateVertices()
	{
		CU::GrowingArray<VertexPosUV> vertices;
		vertices.Init(4);
		vertices.Add({ { -1.f, -1.f, 0.f }, { 0.f, 1.f } }); //topleft
		vertices.Add({ { 1.f, -1.f, 0.f }, { 1.f, 1.f } }); //topright
		vertices.Add({ { -1.f, 1.f, 0.f }, { 0.f, 0.f } }); //bottomleft
		vertices.Add({ { 1.f, 1.f, 0.f }, { 1.f, 0.f } }); //bottomright

		CU::GrowingArray<int> indices;
		indices.Init(6);

		indices.Add(0);
		indices.Add(2);
		indices.Add(1);

		indices.Add(1);
		indices.Add(2);
		indices.Add(3);

		SetupVertexBuffer(vertices.Size(), sizeof(VertexPosUV), reinterpret_cast<char*>(&vertices[0]), "DeferredRenderer::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "DeferredRenderer::IndexBuffer");
	}

	void DeferredRenderer::ActivateBuffers()
	{
		ID3D11Buffer* buf = myVertexBuffer->myVertexBuffer;
		Engine::GetInstance()->GetContex()->IASetInputLayout(myVertexLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBuffer->myStartSlot,
			myVertexBuffer->myNumberOfBuffers, &buf,
			&myVertexBuffer->myStride, &myVertexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer,
			myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DeferredRenderer::Render(Effect* aEffect, const std::string& aTechnique)
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		ID3DX11EffectTechnique* technique = aEffect->GetTechnique(aTechnique);

		technique->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			technique->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
			Engine::GetInstance()->GetContex()->DrawIndexed(6, 0, 0);
		}
	}

	void DeferredRenderer::RenderDeferred(Scene* aScene)
	{
		Engine::GetInstance()->RestoreViewPort();
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		ID3D11RenderTargetView* renderTarget = nullptr;

#ifdef SHADOWS
		renderTarget = myFinishedSceneTexture->GetRenderTargetView();
#else
		renderTarget = myFinishedTexture->GetRenderTargetView();
#endif

		context->ClearDepthStencilView(Engine::GetInstance()->GetDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->ClearRenderTargetView(renderTarget, myClearColor);
		context->OMSetRenderTargets(1, &renderTarget, Engine::GetInstance()->GetDepthView());

		RenderAmbientPass(aScene);

		context->OMSetRenderTargets(1, &renderTarget, myDepthStencilTexture->GetDepthStencilView());


#ifdef USE_LIGHT
		myPointLightPass->Render(aScene, *myGBufferData, myCubemap);
		mySpotLightPass->Render(aScene, *myGBufferData, myCubemap, false);
		mySpotLightPass->Render(aScene, *myGBufferData, myCubemap, true);
#endif
	}

	void DeferredRenderer::RenderAmbientPass(Scene* aScene)
	{
		myAmbientPass->SendDataToGPU(*myGBufferData, myCubemap, *aScene->GetCamera());

		if (GC::EnableCheapAmbient == true)
		{
			Render(myAmbientPass->myEffect, "Render_Cheap");
		}
		else
		{
			Render(myAmbientPass->myEffect);
		}

		myAmbientPass->RemoveDataFromGPU();
	}

	void DeferredRenderer::SetupShadowData()
	{
		myShadowPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_shadow.fx");

		myShadowPass.OnEffectLoad();
		myShadowPass.myEffect->AddListener(&myShadowPass);
	}
}