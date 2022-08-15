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

#include <type_traits>
#include <unordered_map>

// Whether to fallback to solid colors specified in
// a model's MTL file when a diffuse is missing
engine::CConVar mdl_missing50_when_missing_diffuse(
	"mdl_missing50_when_missing_diffuse", "0"); // clang-format: keep

// Warn that model may cause crashes if scaled by more than
// this convar
engine::CConVar mdl_model_scale_warn_limit(
	"mdl_model_scale_warn_limit", "500"); // clang-format: keep

engine::CConVar mat_mapw_min("mat_mapw_min", "0.0");

engine::CConVar mat_mapw_max("mat_mapw_max", "1000.0");

engine::CConVar mdl_base_dir("mdl_base_dir", "resources/model/goblin/");

#pragma region CModelDiagWindow

namespace engine
{

// The Model diagnostic window.
class CModelDiagWindow
{
public:
	void Show(CGraphicalOutput& _Gfx, const char* _Name, const engine::CNode& _Root);
	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	CNode* SelectedNode() const noexcept;

private:
	PSMaterialConstant m_Pmc;

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

void CModelDiagWindow::Show(CGraphicalOutput& _Gfx, const char* _Name, const engine::CNode& _Root)
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
			m_SelectedNode->RenderExtendedCtl(_Gfx, m_Pmc);
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

void engine::CNode::RenderExtendedCtl(CGraphicalOutput& _Gfx, PSMaterialConstant& _Pmc)
{
	if (m_MeshPtrs.empty())
	{
		return;
	}

	if (auto pcb = CCodex::QueryExistent<CConstantPixelBuffer<PSMaterialConstant>>())
	{
		ImGui::Text("Material");

		bool normaMape = _Pmc.EnableNormalMap;
		ImGui::Checkbox("Enable Normal Map", &normaMape);
		_Pmc.EnableNormalMap = normaMape ? TRUE : FALSE;

		bool specMape = _Pmc.EnableSpecularMap;
		ImGui::Checkbox("Enable Specular Map", &specMape);
		_Pmc.EnableSpecularMap = specMape ? TRUE : FALSE;

		bool glosse = _Pmc.HasGloss;
		ImGui::Checkbox("Enable Gloss", &glosse);
		_Pmc.HasGloss = glosse ? TRUE : FALSE;

		ImGui::SliderFloat("Specular Weight", &_Pmc.SpecularMapWeight, 0.0f, 1000.0f, "%f");
		ImGui::SliderFloat("Specular Pow", &_Pmc.SpecularPower, 0.0f, 1000.0f, "%f");
		ImGui::ColorPicker3("Specular Color", reinterpret_cast<float*>(&_Pmc.SpecularColor));
		pcb->Update(_Gfx, _Pmc);
	}
}

void engine::CNode::RenderExtendedCtl(CGraphicalOutput& _Gfx, PSSolidColorConstant& _Pmc)
{
	if (m_MeshPtrs.empty())
	{
		return;
	}

	if (auto pcb = CCodex::QueryExistent<CConstantPixelBuffer<PSSolidColorConstant>>())
	{
		ImGui::Text("Material (solid color)");
		ImGui::SliderFloat("Spec Int.", &_Pmc.SpecularIntensity, mat_mapw_min.GetFloat(), mat_mapw_max.GetFloat());
		ImGui::SliderFloat("Spec Pow.", &_Pmc.SpecularPower, mat_mapw_min.GetFloat(), mat_mapw_max.GetFloat());
		ImGui::ColorPicker3("Diffuse Color", reinterpret_cast<float*>(&_Pmc.MaterialColor));
		pcb->Update(_Gfx, _Pmc);
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
	if (m_Desc.Scale > mdl_model_scale_warn_limit.GetInteger())
	{
		Msg("MODEL_DESCRIPTOR::Scale larger than mdl_model_scale_warn_limit is likely to cause crashes! %f",
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

void engine::CModel::ShowDiagWindow(CGraphicalOutput& _Gfx, const char* _Name)
{
	m_ModelWindow->Show(_Gfx, _Name, *m_Root);
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
	bool hasAlphaGloss = false;
	bool hasColorDiffuse = false;
	float shininess = 2.0f;
	DirectX::XMFLOAT4 specularColor = {0.18f, 0.18f, 0.18f, 1.0f};
	DirectX::XMFLOAT4 diffuseColor = {0.45f, 0.45f, 0.85f, 1.0f};
	if (_Mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;
		auto& mat = *_Materials[_Mesh.mMaterialIndex];
		const std::string base = mdl_base_dir.GetString();

		aiString texFilename;
		if (mat.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx,
																   _GETPATH(base + texFilename.C_Str())));
			hasDiffuse = true;
		}
		else
		{
			if (!mdl_missing50_when_missing_diffuse.GetBool() && mat.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor)) == aiReturn_SUCCESS)
			{
				hasDiffuse = true; // Needs to be true, so it won't bind missing50.
				hasColorDiffuse = true;
			}
		}

		if (mat.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS)
		{
			std::shared_ptr<CTexture> tex = CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 1u);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasSpec = true;
		}
		else
		{
			mat.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
		}

		if (!hasAlphaGloss)
		{
			mat.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (mat.GetTexture(aiTextureType_NORMALS, 0, &texFilename) == aiReturn_SUCCESS)
		{
			std::shared_ptr<CTexture> tex = CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 2u);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
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

	std::shared_ptr<CVertexShader> vs{};
	if (hasDiffuse && hasSpec && hasNormals)
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

		std::wstring filename = MAKE_SHADER_RESOURCE("PhongNormalMap_PS.cso");
		bindablePtrs.push_back(CCodex::Query<CPixelShader>(_Gfx, filename));
		PSMaterialConstant pmc;
		pmc.SpecularPower = shininess;

		pmc.HasGloss = hasAlphaGloss ? TRUE : FALSE;
		auto ptr = CCodex::Query<CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		ptr->Update(_Gfx, pmc);
		bindablePtrs.push_back(std::move(ptr));

		vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("PhongNormalMap_VS.cso"));
		ID3DBlob* pvsbc = vs->GetBytecode();
		bindablePtrs.push_back(std::move(vs));

		bindablePtrs.push_back(CCodex::Query<engine::CVertexBuffer>(_Gfx, std::string(_Mesh.mName.C_Str()), vbuf));
		bindablePtrs.push_back(CCodex::Query<engine::CInputLayout>(_Gfx, vbuf.Layout(), pvsbc));
	}
	else if (hasColorDiffuse)
	{
		layout::CVertexBuffer vbuf(std::move(
			layout::CVertexLayout{}
				.Append(layout::Position3D)
				.Append(layout::Normal)
				.Append(layout::Texture2D)));

		for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
		{
			aiVector3D vert = _Mesh.mVertices[i];
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3{vert.x * m_Desc.Scale, vert.y * m_Desc.Scale, vert.z * m_Desc.Scale},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&_Mesh.mTextureCoords[0][i]));
		}

		std::wstring filename = MAKE_SHADER_RESOURCE("PhongSpecular_SolidColor_PS.cso");
		bindablePtrs.push_back(CCodex::Query<CPixelShader>(_Gfx, filename));

		PSSolidColorConstant pmc;
		pmc.SpecularPowerConst = shininess;
		pmc.SpecularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		pmc.MaterialColor = DirectX::XMFLOAT4A{diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w};
		pmc.HasGloss = hasAlphaGloss ? TRUE : FALSE;

		auto ptr = CCodex::Query<CConstantPixelBuffer<PSSolidColorConstant>>(_Gfx, 10u);
		ptr->Update(_Gfx, pmc);
		bindablePtrs.push_back(std::move(ptr));

		vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		ID3DBlob* pvsbc = vs->GetBytecode();
		bindablePtrs.push_back(std::move(vs));

		bindablePtrs.push_back(CCodex::Query<engine::CVertexBuffer>(_Gfx, std::string(_Mesh.mName.C_Str()), vbuf));
		bindablePtrs.push_back(CCodex::Query<engine::CInputLayout>(_Gfx, vbuf.Layout(), pvsbc));
	}
	else if (hasDiffuse)
	{
		layout::CVertexBuffer vbuf(std::move(
			layout::CVertexLayout{}
				.Append(layout::Position3D)
				.Append(layout::Normal)
				.Append(layout::Texture2D)));

		for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
		{
			aiVector3D vert = _Mesh.mVertices[i];
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3{vert.x * m_Desc.Scale, vert.y * m_Desc.Scale, vert.z * m_Desc.Scale},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&_Mesh.mTextureCoords[0][i]));
		}

		std::wstring filename = MAKE_SHADER_RESOURCE("PhongSpecular_PS.cso");
		bindablePtrs.push_back(CCodex::Query<CPixelShader>(_Gfx, filename));
		struct __declspec(align(16)) PSMaterialConstant_Inner
		{
			BOOL EnableSpecular = TRUE;
			BOOL HasGloss;
			float SpecularPowerConst;
			float SpecularIntensity = 0.0f;
			float SpecularPower = 0.0f;
			float padd;
		} pmc;

		pmc.SpecularPowerConst = shininess;
		pmc.HasGloss = hasAlphaGloss ? TRUE : FALSE;

		auto ptr = CCodex::Query<CConstantPixelBuffer<PSMaterialConstant_Inner>>(_Gfx, 10u);
		ptr->Update(_Gfx, pmc);
		bindablePtrs.push_back(std::move(ptr));

		vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		ID3DBlob* pvsbc = vs->GetBytecode();
		bindablePtrs.push_back(std::move(vs));

		bindablePtrs.push_back(CCodex::Query<engine::CVertexBuffer>(_Gfx, std::string(_Mesh.mName.C_Str()), vbuf));
		bindablePtrs.push_back(CCodex::Query<engine::CInputLayout>(_Gfx, vbuf.Layout(), pvsbc));
	}

	bindablePtrs.push_back(CCodex::Query<engine::CIndexBuffer>(_Gfx, indices, std::string(_Mesh.mName.C_Str())));
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