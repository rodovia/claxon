#include "ModelTest.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tier0lib/String0.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/VertexBuffer.h>
#include <engine_tier1/dxprim/IndexBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/InputLayout.h>

#include <DirectXMath.h>

engine::CModelTest::CModelTest(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
	std::uniform_real_distribution<float>& _Adist,
	std::uniform_real_distribution<float>& _Ddist,
	std::uniform_real_distribution<float>& _Odist,
	std::uniform_real_distribution<float>& _Rdist,
	std::uniform_real_distribution<float>& _Bdist,
	DirectX::XMFLOAT3 _Material,
	float _Scale)
	: CBase_PrimObject(_Gfx, _Rng, _Adist, _Ddist, _Odist, _Rdist, _Bdist)
{
	if (!this->IsStaticInit())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 norm;
		};

		Assimp::Importer imp;
		const aiScene* model = imp.ReadFile(tier0::ConvertToMultiByteString(_GETPATH("resources/model/Prop_Tree_Palm_1.obj")), 
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);
		aiMesh* mesh = model->mMeshes[0];
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.push_back({
				{ mesh->mVertices[i].x * _Scale, mesh->mVertices[i].y * _Scale, mesh->mVertices[i].z * _Scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i])
			});
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh->mNumFaces * 3);
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace fac = mesh->mFaces[i];
			assert(fac.mNumIndices == 3);
			indices.push_back(fac.mIndices[0]);
			indices.push_back(fac.mIndices[1]);
			indices.push_back(fac.mIndices[2]);
		}

		this->AddStaticBind(std::make_unique<engine::CVertexBuffer>(_Gfx, vertices));
		this->AddStaticIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, indices));

		std::unique_ptr<engine::CVertexShader> pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		ID3DBlob* pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_PS.cso")));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			_ENGINE_POSITION_IED,
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));

		struct PSMaterialConstant
		{
			alignas(16) DirectX::XMFLOAT3 Color;
			float SpecularIntensity = 0.6f;
			float SpecularPower = 30.0f;
			float _unused[2];
		} colorc;
		colorc.Color = _Material;
		auto ma = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 1u);
		ma->Update(_Gfx, colorc);

		this->AddBind(std::move(ma));
	}
	else
	{
		this->SetIndexFromStatic();
	}
}
