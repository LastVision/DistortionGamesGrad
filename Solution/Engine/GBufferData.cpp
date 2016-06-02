#include "stdafx.h"

#include <D3DX11.h>
#include "GBufferData.h"

namespace Prism
{
	
	GBufferData::GBufferData()
	{
		CU::Vector2<float> windowSize = Engine::GetInstance()->GetWindowSize();
		myAlbedoTexture = new Texture();
		myAlbedoTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R16G16B16A16_FLOAT);
		//DXGI_FORMAT_R8G8B8A8_UNORM

		myNormalTexture = new Texture();
		myNormalTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myEmissiveTexture = new Texture();
		myEmissiveTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myDepthTexture = new Texture();
		myDepthTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32_FLOAT);
	}

	GBufferData::~GBufferData()
	{
		SAFE_DELETE(myAlbedoTexture);
		SAFE_DELETE(myNormalTexture);
		SAFE_DELETE(myEmissiveTexture);
		SAFE_DELETE(myDepthTexture);
	}

	void GBufferData::Clear(float* aClearColor)
	{
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		context->ClearRenderTargetView(myAlbedoTexture->GetRenderTargetView(), aClearColor);
		context->ClearRenderTargetView(myNormalTexture->GetRenderTargetView(), aClearColor);
		context->ClearRenderTargetView(myEmissiveTexture->GetRenderTargetView(), aClearColor);
		context->ClearRenderTargetView(myDepthTexture->GetRenderTargetView(), aClearColor);
	}

	void GBufferData::SetAsRenderTarget(Texture* aDepthTexture)
	{
		SetAsRenderTarget(aDepthTexture->GetDepthStencilView());
	}

	void GBufferData::SetAsRenderTarget(ID3D11DepthStencilView* aDepthStencil)
	{
		ID3D11RenderTargetView* targets[4];
		targets[0] = myAlbedoTexture->GetRenderTargetView();
		targets[1] = myNormalTexture->GetRenderTargetView();
		targets[2] = myEmissiveTexture->GetRenderTargetView();
		targets[3] = myDepthTexture->GetRenderTargetView();

		Engine::GetInstance()->GetContex()->OMSetRenderTargets(4, targets
			, aDepthStencil);
	}

	void GBufferData::SetAlbedoAsRenderTarget(Texture* aDepthTexture)
	{
		SetAlbedoAsRenderTarget(aDepthTexture->GetDepthStencilView());
	}

	void GBufferData::SetAlbedoAsRenderTarget(ID3D11DepthStencilView* aDepthStencil)
	{
		ID3D11RenderTargetView* targets[1];
		targets[0] = myAlbedoTexture->GetRenderTargetView();

		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, targets
			, aDepthStencil);
	}

	void GBufferData::Copy(const GBufferData& aSource)
	{
		Engine::GetInstance()->GetContex()->CopyResource(myAlbedoTexture->GetTexture(), aSource.myAlbedoTexture->GetTexture());

#ifdef PBL_DECALS
		Engine::GetInstance()->GetContex()->CopyResource(myEmissiveTexture->GetTexture(), aSource.myEmissiveTexture->GetTexture());
		Engine::GetInstance()->GetContex()->CopyResource(myNormalTexture->GetTexture(), aSource.myNormalTexture->GetTexture());
		Engine::GetInstance()->GetContex()->CopyResource(myDepthTexture->GetTexture(), aSource.myDepthTexture->GetTexture());
#endif
	}

}