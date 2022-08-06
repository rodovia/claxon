/* Model.cpp -- The implementation of a scene graph.
				(and the implementation of CMesh, CNode and CModel) */

// This file does not use Win32 API, but windows.h gets included by indirection,
// defines MINMAX macros, and assimp.h uses std::min and std::max, creating conflict

#define _MDL_SCALE_THRESHOLD 500

#include <tier0lib/Win32.h>

#include "Model.h"
#include "VertexLayout.h"
#include "Surface.h"

#include "dxprim/Shaders.h"
#include "dxprim/InputLayout.h"
#include "dxprim/VertexBuffer.h"
#include "dxprim/Topology.h"
#include "dxprim/TransformConstBuffer.h"
#include "dxprim/Texture.h"
#include <engine_tier0/Exceptions.h>
#include "dxprim/Sampler.h"
#include "BindableCodex.h"
#include "Surface.h"
#include <engine_ui/Console.h>

#include <tier0lib/String0.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

#pragma region CModelDiagWindow

namespace engine
{

// The Model diagnostic window.
class CModelDiagWindow
{
public:
	void Show(const char* _Name, const engine::CNode& _Root);
	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	CNode* SelectedNode() const noexcept;

private:
	struct TransformParams
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
	} m_Pos;
	engine::CNode* m_SelectedNode;
	std::unordered_map<int, TransformParams> m_Transforms;
};

void CModelDiagWindow::Show(const char* _Name, const engine::CNode& _Root)
{
	std::string fmt = std::format("Ferramenta de diagnostico de modelos - {}", _Name ? _Name : "Modelo");
	int cnix = 0;
	if (ImGui::Begin(fmt.c_str()))
	{
		ImGui::Columns(2, nullptr);
		_Root.RenderTree(m_SelectedNode);

		ImGui::NextColumn();
		if (m_SelectedNode != nullptr)
		{
			TransformParams& transform = m_Transforms[m_SelectedNode->GetId()];
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Pitch", &transform.Pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.Yaw, -180.0f, 180.0f);
			ImGui::SliderAngle("Roll", &transform.Roll, -180.0f, 180.0f);

			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.X, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.Y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.Z, -20.0f, 20.0f);
		}
	}

	ImGui::End();
}

DirectX::XMMATRIX CModelDiagWindow::GetTransformMatrix() const noexcept
{
	assert(m_SelectedNode != nullptr);
	const auto& transform = m_Transforms.at(m_SelectedNode->GetId());
	return DirectX::XMMatrixRotationRollPitchYaw(transform.Pitch, transform.Yaw, transform.Roll) * DirectX::XMMatrixTranslation(transform.X, transform.Y, transform.Z);
}

engine::CNode* CModelDiagWindow::SelectedNode() const noexcept
{
	return m_SelectedNode;
}

} // namespace engine

#pragma endregion CModelDiagWindow

#pragma region CMesh

engine::CMesh::CMesh(engine::CGraphicalOutput& _Gfx, std::vector<std::shared_ptr<engine::CBase_Bind>> _BindPtrs)
{
	this->AddBind(CCodex::Query<CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : _BindPtrs)
	{
		this->AddBind(std::move(pb));
	}

	this->AddBind(std::make_shared<engine::CTransformConstantBuffer>(_Gfx, *this));
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

#pragma endregion CMesh

#pragma region CNode

engine::CNode::CNode(int _Id, const std::string& _Name, std::vector<engine::CMesh*> _MeshPtrs,
					 const DirectX::XMMATRIX& _Transform) noexcept
	: m_Id(_Id),
	  m_MeshPtrs(std::move(_MeshPtrs)),
	  m_Name(_Name)
{
	DirectX::XMStoreFloat4x4(&m_BaseTransform, _Transform);
	DirectX::XMStoreFloat4x4(&m_ApplTransform, DirectX::XMMatrixIdentity());
}

int engine::CNode::GetId() const noexcept
{
	return m_Id;
}

void engine::CNode::SetAppliedTransform(DirectX::FXMMATRIX _Transform) noexcept
{
	DirectX::XMStoreFloat4x4(&m_ApplTransform, _Transform);
}

void engine::CNode::Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept
{
	const auto built =
		DirectX::XMLoadFloat4x4(&m_ApplTransform) * DirectX::XMLoadFloat4x4(&m_BaseTransform) * _AccumTransform;
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

void engine::CNode::RenderTree(engine::CNode*& _SelectedNode) const noexcept
{
	const int selecDiscrim = (_SelectedNode == nullptr) ? -1 : _SelectedNode->GetId();
	const int flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_Id == selecDiscrim) ? ImGuiTreeNodeFlags_Selected : 0) | ((m_NodePtrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0);

	const bool expnd = ImGui::TreeNodeEx((void*)(intptr_t)m_Id, flags, m_Name.c_str());

	if (ImGui::IsItemClicked())
	{
		_SelectedNode = const_cast<engine::CNode*>(this);
	}

	if (expnd)
	{
		for (const auto& child : m_NodePtrs)
		{
			child->RenderTree(_SelectedNode);
		}
		ImGui::TreePop();
	}
}

#pragma endregion CNode

#pragma region CModel

engine::CModel::CModel(CGraphicalOutput& _Gfx, const std::wstring _Filename,
					   MODEL_DESCRIPTOR _Desc)
	: m_ModelWindow(std::make_unique<CModelDiagWindow>()),
	  m_Desc(_Desc)
{
	Assimp::Importer imp;
	std::string fname = tier0::ConvertToMultiByteString(_Filename);
	Msg("Loading model %s", fname.c_str());
	const auto scene = imp.ReadFile(fname,
									aiProcess_Triangulate
										| aiProcess_JoinIdenticalVertices
										| aiProcess_ConvertToLeftHanded
										| aiProcess_GenNormals
										| aiProcess_CalcTangentSpace);
	if (m_Desc.Scale > _MDL_SCALE_THRESHOLD)
	{
		Msg("MODEL_DESCRIPTOR::Scale larger than _MDL_SCALE_THRESHOLD is likely to cause crashes! %f",
			m_Desc.Scale);
	}

	if (scene == nullptr)
	{
		throw _CModelException(imp.GetErrorString());
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		m_MeshPtrs.push_back(this->ParseMesh(_Gfx, *scene->mMeshes[i], scene->mMaterials));
	}

	int discrim = 0;
	m_Root = ParseNode(discrim, *scene->mRootNode);
}

void engine::CModel::ShowDiagWindow(const char* _Name)
{
	m_ModelWindow->Show(_Name, *m_Root);
}

void engine::CModel::Draw(CGraphicalOutput& _Gfx) const
{
	if (auto node = m_ModelWindow->SelectedNode())
	{
		node->SetAppliedTransform(m_ModelWindow->GetTransformMatrix());
	}
	m_Root->Draw(_Gfx, DirectX::XMMatrixIdentity());
}

std::unique_ptr<engine::CMesh> engine::CModel::ParseMesh(CGraphicalOutput& _Gfx, const aiMesh& _Mesh, const aiMaterial* const* _Materials)
{
	layout::CVertexBuffer vbuf(std::move(
		layout::CVertexLayout{}
			.Append(layout::Position3D)
			.Append(layout::Normal)
			.Append(layout::Tangent)
			.Append(layout::Bitangent)
			.Append(layout::Texture2D)));

	for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
	{
		aiVector3D vert = _Mesh.mVertices[i];
		vbuf.EmplaceBack(
			DirectX::XMFLOAT3{vert.x * m_Desc.Scale, vert.y * m_Desc.Scale, vert.z * m_Desc.Scale},
			*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mTangents[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mBitangents[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&_Mesh.mTextureCoords[0][i]));
	}

	std::vector<unsigned short> indices;
	indices.reserve(_Mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < _Mesh.mNumFaces; i++)
	{
		const auto& face = _Mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::shared_ptr<CBase_Bind>> bindablePtrs;

	bool hasSpec = false;
	bool hasDiffuse = false;
	bool hasNormals = false;
	float shininess = 35.0f;
	if (_Mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;
		auto& mat = *_Materials[_Mesh.mMaterialIndex];
		const std::string base = "resources/model/goblin/"s;

		aiString texFilename;
		if (mat.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx,
																   _GETPATH(base + texFilename.C_Str())));
			hasDiffuse = true;
		}

		if (mat.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx,
																   _GETPATH(base + texFilename.C_Str()),
																   1u));
			hasSpec = true;
		}
		else
		{
			mat.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (mat.GetTexture(aiTextureType_NORMALS, 0, &texFilename) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 2u));
			hasNormals = true;
		}
		
		if (hasDiffuse || hasNormals || hasSpec)
		{
			bindablePtrs.push_back(CCodex::Query<engine::CSampler>(_Gfx));
		}
		
	}

	if (!hasDiffuse)
	{
		Msg("Mesh '%s' has no DIFFUSE textures! Falling back to missing50", _Mesh.mName.C_Str());
		bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx, _GETPATH("resources/textures/missing50.png"), 0u));
		hasDiffuse = true;
	}

	bindablePtrs.push_back(CCodex::Query<engine::CVertexBuffer>(_Gfx, std::string(_Mesh.mName.C_Str()), vbuf));
	bindablePtrs.push_back(CCodex::Query<engine::CIndexBuffer>(_Gfx, indices, std::string(_Mesh.mName.C_Str())));

	if (hasSpec)
	{
		std::wstring filename = MAKE_SHADER_RESOURCE("PhongSpecular_PS.cso");
		bindablePtrs.push_back(CCodex::Query<CPixelShader>(_Gfx, filename));

		struct __declspec(align(16)) PSMaterialConstant
		{
			BOOL NormalMapEnabled = TRUE;
		} pmc;

		auto ptr = CCodex::Query<CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		ptr->Update(_Gfx, pmc);
		bindablePtrs.push_back(std::move(ptr));
	}
	else if (hasNormals)
	{
		std::wstring filename = MAKE_SHADER_RESOURCE("PhongNormalMap_PS.cso");
		bindablePtrs.push_back(CCodex::Query<CPixelShader>(_Gfx, filename));

		struct __declspec(align(16)) PSMaterialConstant
		{
			float specularIntensity = 0.8f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		auto cpb = CCodex::Query<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		cpb->Update(_Gfx, pmc);
		bindablePtrs.push_back(std::move(cpb));
	}
	std::shared_ptr<CVertexShader> vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));

	ID3DBlob* pvsbc = vs->GetBytecode();
	bindablePtrs.push_back(std::move(vs));
	bindablePtrs.push_back(CCodex::Query<engine::CInputLayout>(_Gfx, vbuf.Layout(), pvsbc));

	return std::make_unique<CMesh>(_Gfx, std::move(bindablePtrs));
}

std::unique_ptr<engine::CNode> engine::CModel::ParseNode(int& _Id, const aiNode& _Node)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&_Node.mTransformation)));

	std::vector<engine::CMesh*> curMeshPtrs;
	curMeshPtrs.reserve(_Node.mNumMeshes);
	for (size_t i = 0; i < _Node.mNumMeshes; i++)
	{
		const auto meshIdx = _Node.mMeshes[i];
		curMeshPtrs.push_back(m_MeshPtrs.at(meshIdx).get());
	}

	auto node = std::make_unique<CNode>(_Id++, _Node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < _Node.mNumChildren; i++)
	{
		node->AddChild(this->ParseNode(_Id, *_Node.mChildren[i]));
	}

	return node;
}

engine::CModel::~CModel() noexcept
{
}

#pragma endregion CModel