/* Model.cpp -- The implementation of a scene graph. 
				(and the implementation of CMesh, CNode and CModel) */

// This file does not use Win32 API, but windows.h gets included by indirection,
// defines MINMAX macros, and assimp.h uses std::min and std::max, creating conflict

#include <tier0lib/Win32.h> 

#include "Model.h"
#include "VertexLayout.h"
#include "Surface.h"

#include "dxprim/Shaders.h"
#include "dxprim/InputLayout.h"
#include "dxprim/VertexBuffer.h"
#include "dxprim/Topology.h"
#include "dxprim/TransformConstBuffer.h"

#include <tier0lib/String0.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

engine::CMesh::CMesh(engine::CGraphicalOutput& _Gfx, std::vector<std::unique_ptr<engine::CBase_Bind>> _BindPtrs)
{
	if (!this->IsStaticInit())
	{
		this->AddStaticBind(std::make_unique<CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : _BindPtrs)
	{
		if (auto pi = dynamic_cast<engine::CIndexBuffer*>(pb.get()))
		{
			this->AddIndexBuffer(std::unique_ptr<engine::CIndexBuffer>{ pi });
			pb.release();
		}
		else
		{
			this->AddBind(std::move(pb));
		}
	}

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}

void engine::CMesh::Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept
{
	DirectX::XMStoreFloat4x4(&m_Transform, _AccumTransform);
	CBase_Draw::Draw(_Gfx);
}

DirectX::XMMATRIX engine::CMesh::GetTransformMatrix() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_Transform);
}

// CMesh ^^^^^ // CNode vvvvv

engine::CNode::CNode(std::vector<engine::CMesh*> _MeshPtrs, const DirectX::XMMATRIX& _Transform) noexcept
	:
	m_MeshPtrs(std::move(_MeshPtrs))
{
	DirectX::XMStoreFloat4x4(&m_Transform, _Transform);
}
void engine::CNode::Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept
{
	const auto built = DirectX::XMLoadFloat4x4(&m_Transform) * _AccumTransform;
	for (const auto pm : m_MeshPtrs)
	{
		pm->Draw(_Gfx, built);
	}
	for (const auto& pc : m_NodePtrs)
	{
		pc->Draw(_Gfx, built);
	}
}
void engine::CNode::AddChild(std::unique_ptr<CNode> _Child) noexcept
{
	assert(_Child);
	m_NodePtrs.push_back(std::move(_Child));
}

// engine::CNode ^^^^^ // engine::CModel vvvvv

engine::CModel::CModel(CGraphicalOutput& _Gfx, const std::wstring _Filename)
{
	Assimp::Importer imp;
	const auto scene = imp.ReadFile(tier0::ConvertToMultiByteString(_Filename),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		m_MeshPtrs.push_back(this->ParseMesh(_Gfx, *scene->mMeshes[i]));
	}

	m_Root = ParseNode(*scene->mRootNode);
}

void engine::CModel::Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _Transform) const
{
	m_Root->Draw(_Gfx, _Transform);
}

std::unique_ptr<engine::CMesh> engine::CModel::ParseMesh(CGraphicalOutput& _Gfx, const aiMesh& _Mesh)
{
	layout::CVertexBuffer vbuf(std::move(
		layout::CVertexLayout{}
		.Append(layout::Position3D)
		.Append(layout::Normal)
	));

	for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(_Mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < _Mesh.mNumFaces; i++)
	{
		const auto& face = _Mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		std::printf("%i %i %i\n", face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<CBase_Bind>> bindablePtrs;

	bindablePtrs.push_back(std::make_unique<engine::CVertexBuffer>(_Gfx, vbuf));

	bindablePtrs.push_back(std::make_unique<engine::CIndexBuffer>(_Gfx, indices));

	auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_PS.cso")));

	bindablePtrs.push_back(std::make_unique<engine::CInputLayout>(_Gfx, vbuf.Layout().D3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	auto cpb = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
	cpb->Update(_Gfx, pmc);
	bindablePtrs.push_back(std::move(cpb));

	return std::make_unique<CMesh>(_Gfx, std::move(bindablePtrs));
}

std::unique_ptr<engine::CNode> engine::CModel::ParseNode(const aiNode& _Node)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Node.mTransformation)
	));

	std::vector<engine::CMesh*> curMeshPtrs;
	curMeshPtrs.reserve(_Node.mNumMeshes);
	for (size_t i = 0; i < _Node.mNumMeshes; i++)
	{
		const auto meshIdx = _Node.mMeshes[i];
		curMeshPtrs.push_back(m_MeshPtrs.at(meshIdx).get());
	}

	auto node = std::make_unique<CNode>(std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < _Node.mNumChildren; i++)
	{
		node->AddChild(this->ParseNode(*_Node.mChildren[i]));
	}

	return node;
}