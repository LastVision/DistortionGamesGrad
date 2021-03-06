#include "stdafx.h"
#include "Bar3D.h"
#include "Camera.h"
#include "IndexBufferWrapper.h"
#include <d3dx11effect.h>
#include "Surface.h"

namespace Prism
{
	Bar3D::Bar3D(float aInner, float aOuter, int aNumberOfQuads, Effect* aEffect
		, eBarPosition aBarPosition, const std::string& aTexturePath)
		: myInner(aInner)
		, myOuter(aOuter)
		, myValue(0.f)
		, myNbrOfQuads(aNumberOfQuads)
		, myBarPosition(aBarPosition)
	{
		myEffect = aEffect;
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "GUI::InputLayout");
		InitVertexBuffer(sizeof(GUIVertex), D3D11_USAGE_IMMUTABLE, 0);

		//InitIndexBuffer();
		InitSurface("AlbedoTexture", aTexturePath);
		mySurfaces[0]->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		InitBlendState("GUI::BlendState");

		CreateVertices(aNumberOfQuads);
	}


	Bar3D::~Bar3D()
	{
	}

	void Bar3D::Render(const Camera& aCamera, const CU::Matrix44<float>& aWorld, const CU::Vector4<float>& aColor)
	{
		Prism::Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		myEffect->SetWorldMatrix(aWorld);
		//myEffect->SetViewProjectionMatrix(aCamera.GetSpecialFoVViewProjection());
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
		myEffect->SetColor(aColor);
		//BaseModel::Render();

		Render(false);
		Prism::Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);
	}

	void Bar3D::Render(bool)
	{

		Engine::GetInstance()->GetContex()->IASetInputLayout(myVertexLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBuffer->myStartSlot
			, myVertexBuffer->myNumberOfBuffers, &myVertexBuffer->myVertexBuffer
			, &myVertexBuffer->myStride, &myVertexBuffer->myByteOffset);
		//Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer
		//	, myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);

		for (int s = 0; s < mySurfaces.Size(); ++s)
		{
			int verticesToSend = int(abs(myValue) * myNbrOfQuads);
			int rest = verticesToSend * 2;
			verticesToSend = rest + 2;
			mySurfaces[s]->Activate();

			ID3DX11EffectTechnique* tech = nullptr;
			tech = myEffect->GetTechnique(myTechniqueName);

			if (tech->IsValid() == false)
			{
				tech = myEffect->GetTechnique();
				DL_ASSERT("INVALID TECHNIQUE IN BASEMODEL::RENDER: " + myTechniqueName);
			}

			DL_ASSERT_EXP(tech != nullptr, "Technique is nullptr");
			DL_ASSERT_EXP(tech->IsValid() != false, "Technique is not valid");

			D3DX11_TECHNIQUE_DESC techDesc;
			tech->GetDesc(&techDesc);
			for (UINT i = 0; i < techDesc.Passes; ++i)
			{
				tech->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
				/*Engine::GetInstance()->GetContex()->DrawIndexed(indicesToSend
					, mySurfaces[s]->GetIndexStart(), 0);*/
				Engine::GetInstance()->GetContex()->Draw(verticesToSend, 0);
			}

			mySurfaces[s]->DeActivate();
		}
	}

	void Bar3D::SetValue(float aValue)
	{
		myValue = aValue;
	}

	void Bar3D::Rebuild(int aNumberOfQuads)
	{
		myNbrOfQuads = aNumberOfQuads;
		CreateVertices(myNbrOfQuads);
	}

	void Bar3D::CreateVertices(int aNumberOfQuads)
	{
		int numbersToMake = aNumberOfQuads + 1;
		CU::GrowingArray<GUIVertex> vertices(4 * numbersToMake);
		CU::GrowingArray<CU::Vector4<float>> torusPosition(4 * numbersToMake);


		float zPosition = 0.f;

		for (int i = 0; i < numbersToMake; ++i)
		{
			float angle = 0.f;
			if (myBarPosition == eBarPosition::AIM)
			{
				angle = (i / (float)numbersToMake) * 6.24f;
			}
			
			CU::Vector4<float> vertexInner(myInner * cos(angle), myInner * sin(angle), zPosition, 0.f);
			CU::Vector4<float> vertexOuter(myOuter * cos(angle), myOuter * sin(angle), zPosition, 0.f);

			torusPosition.Add(vertexInner);
			torusPosition.Add(vertexOuter);
		}

		float uvPos = 1;
		for (int i = 0; i < torusPosition.Size(); ++i)
		{
			GUIVertex vertex;
			if (i == 0)
			{
				vertex.myPosition = torusPosition[0];
				vertex.myUV = { 1, 0 };
			}
			else if (i == 1)
			{
				vertex.myPosition = torusPosition[1];
				vertex.myUV = { 0, 0 };
			}
			else if (i == 2)
			{
				vertex.myPosition = torusPosition[2];
				vertex.myUV = { 1, 1 };
			}
			else if (i == 3)
			{
				vertex.myPosition = torusPosition[3];
				vertex.myUV = { 0, 1 };
			}
			else
			{
				if (i % 2 == 0)
				{
					++uvPos;
					vertex.myPosition = torusPosition[i];
					vertex.myUV = { 1.f, uvPos };
				}
				else
				{
					vertex.myPosition = torusPosition[i];
					vertex.myUV = { 0, uvPos };
				}
			}

			vertices.Add(vertex);
		}


			SetupVertexBuffer(vertices.Size(), sizeof(GUIVertex), reinterpret_cast<char*>(&vertices[0])
				, "GUI::VertexBuffer");

			mySurfaces[0]->SetVertexCount(vertices.Size());
		}
	}