/* Model.cpp -- The implementation of a scene graph.
				(and the implementation of CMesh, CNode and CModel) */

// This file does not use Win32 API, but windows.h gets included by indirection,
// defines MINMAX macros, and assimp.h uses std::min and std::max, creating conflict

#include <tier0lib/Win32.h>

#include "Model.h"

#include <engine_tier1/dxprim/fwd.h>
#include <engine_tier0/Exceptions.h>
#include <engine_tier1/BindableCodex.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/matrix.h>
#include <engine_ui/Console.h>

#include <tier0lib/String0.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <bitset>
#include <type_traits>
#include <unordered_map>

#define Msg Log // compat with old console macro name

static void __stdcall _Fullbright_Chg(engine::CConVar*);

// Whether to fallback to solid colors specified in
// a model's MTL file when a diffuse is missing
engine::CConVar mdl_missing50_when_missing_diffuse(
	"mdl_missing50_when_missing_diffuse", "0"); // clang-format: keep

engine::CConVar mat_mapw_min("mat_mapw_min", "0.0");

engine::CConVar mat_mapw_max("mat_mapw_max", "1000.0");

engine::CConVar mat_fullbright("mat_fullbright",
							   "0", "Makes everything 100% brightness and ignore all light points."
									" Causes a reload upon change.",
							   _Fullbright_Chg);

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
			const int id = m_SelectedNode->GetId();
			std::unordered_map<int, TransformParams>::const_iterator trf = m_Transforms.find(id);
			if (trf == m_Transforms.end())
			{
				const DirectX::XMFLOAT4X4& appl = m_SelectedNode->GetAppliedTransform();
				const DirectX::XMFLOAT3 angl = engine::ExtractEulerAngles(appl);
				const DirectX::XMFLOAT3 transl = engine::ExtractTranslation(appl);

				TransformParams tp;
				tp.Roll = angl.z;
				tp.Pitch = angl.x;
				tp.Yaw = angl.y;
				tp.X = transl.x;
				tp.Y = transl.y;
				tp.Z = transl.z;
				std::tie(trf, std::ignore) = m_Transforms.insert({id, tp});
			}
			TransformParams transform = trf->second;
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

	this->AddBind(std::make_shared<engine::CTransformConstantBuffer>(_Gfx, this));
}

void engine::CMesh::Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept
{
	DirectX::XMStoreFloat4x4(&m_Transform, _AccumTransform);
	CBase_Draw::Draw(_Gfx);
}

DirectX::XMMATRIX engine::CMesh::GetTransformMatrix(const CCamera&) const noexcept
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

const DirectX::XMFLOAT4X4& engine::CNode::GetAppliedTransform() const noexcept
{
	return m_ApplTransform;
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

	if (scene == nullptr)
	{
		throw _CModelException(imp.GetErrorString());
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		m_MeshPtrs.push_back(this->ParseMesh(_Gfx, *scene->mMeshes[i], scene->mMaterials, _Filename));
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

std::unique_ptr<engine::CMesh> engine::CModel::ParseMesh(CGraphicalOutput& _Gfx,
														 const aiMesh& _Mesh,
														 const aiMaterial* const* _Materials,
														 const std::filesystem::path& _Path)
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

	// std::shared_ptr<engine::CBlender> bl = CCodex::Query<engine::CBlender>(_Gfx, true);
	// bindablePtrs.push_back(std::move(bl));

	MESH_LOAD_RESULT result;
	float shininess = 2.0f;
	if (_Mesh.mMaterialIndex >= 0)
	{
		result = this->MaterialCheck(_Gfx, *_Materials[_Mesh.mMaterialIndex], bindablePtrs, _Path);
		bindablePtrs.push_back(CCodex::Query<engine::CSampler>(_Gfx));
	}

	if (result.State.test(hl2_RESULT_DIFFUSE))
	{
		Msg("Mesh '%s' has no DIFFUSE textures! Falling back to missing50", _Mesh.mName.C_Str());
		bindablePtrs.push_back(CCodex::Query<engine::CTexture>(_Gfx, _GETPATH("resources/textures/missing50.png"), 0u));
	}

	std::shared_ptr<CVertexShader> vs{};
	// There is no PhongSpecular_VS.hlsl
	// TODO: make shaders names more uniform (e.g. add a PhongSpecular_VS.hlsl or
	// remove PhongNormalMap_VS.hlsl)
	auto ly = layout::CVertexLayout{}
		.Append(layout::Position3D)
		.Append(layout::Normal)
		.Append(layout::Texture2D);
	if (result.RequiredShader == "PhongSpecular")
	{
		vs = CCodex::Query<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		layout::CVertexBuffer vbuf(ly);

		for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
		{
			// TODO: reimplement scale
			DirectX::XMFLOAT3 xmf = *reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mVertices[i]);
			vbuf.EmplaceBack(
				xmf,
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mTextureCoords[0][i])
			);
		}
	}
	else
	{
		ly
			.Append(layout::Tangent)
			.Append(layout::Bitangent);
		vs = CCodex::Query<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE(result.RequiredShader + "_VS.cso"));
		layout::CVertexBuffer vbuf(ly);

		for (unsigned int i = 0; i < _Mesh.mNumVertices; i++)
		{
			// TODO: reimplement scale
			DirectX::XMFLOAT3 xmf = *reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mVertices[i]);
			vbuf.EmplaceBack(
				xmf,
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mTextureCoords[0][i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&_Mesh.mBitangents[i])
			);
		}
	}

	buffer::CLayoutView vw(result.Layout);
	buffer::CBuffer buffer(vw);
	/*
			rl.Add(buffer::Float, "SpecularPower");
		rl.Add(buffer::Float3, "SpecularColor");
		rl.Add(buffer::Float, "SpecularMapWeight");
		rl.Add(buffer::Bool, "EnableNormalMap");
		rl.Add(buffer::Bool, "EnableSpecularMap");
		rl.Add(buffer::Bool, "HasGloss");
		rl.Add(buffer::Float, "SpecularIntensity");*/
	buffer["SpecularPower"].SetExists(12);

	bindablePtrs.push_back(CCodex::Query<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE(result.RequiredShader + "_PS.cso")));
	bindablePtrs.push_back(CCodex::Query<engine::CRasterizer>(_Gfx, result.State.test(hl2_RESULT_DIFFUSE_ALPHA)));
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

engine::MESH_LOAD_RESULT
engine::CModel::MaterialCheck(CGraphicalOutput& _Gfx,
							  const aiMaterial& mat,
							  std::vector<std::shared_ptr<CBase_Bind>>& binds,
							  std::filesystem::path path) noexcept
{
	buffer::CRawLayout rl;
	const std::string base = path.parent_path().string() + "\\";
	std::string requiredShader;
	std::bitset<8> ret;

	aiString texFilename;
	if (mat.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename) == aiReturn_SUCCESS)
	{
		std::shared_ptr<CTexture> tex = CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 0u);
		if (tex->HasAlpha())
		{
			ret.set(hl2_RESULT_DIFFUSE_ALPHA);
		}
		binds.push_back(std::move(tex));
		ret.set(hl2_RESULT_DIFFUSE);
	}

	if (mat.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS)
	{
		std::shared_ptr<CTexture> tex = CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 1u);
		if (tex->HasAlpha())
		{
			ret.set(hl2_RESULT_SPECULAR_ALPHA);
		}

		binds.push_back(std::move(tex));
		ret.set(hl2_RESULT_SPECULAR);
	}

	if (mat.GetTexture(aiTextureType_NORMALS, 0, &texFilename) == aiReturn_SUCCESS)
	{
		std::shared_ptr<CTexture> tex = CCodex::Query<engine::CTexture>(_Gfx, _GETPATH(base + texFilename.C_Str()), 2u);
		if (tex->HasAlpha())
		{
			ret.set(hl2_RESULT_NORMALMAP_ALPHA);
		}
		binds.push_back(std::move(tex));
		ret.set(hl2_RESULT_NORMALMAP);
	}

	if (ret.test(hl2_RESULT_NORMALMAP))
	{
		requiredShader = "PhongNormalMap";
		rl.Add(buffer::Float, "SpecularPower");
		rl.Add(buffer::Float3, "SpecularColor");
		rl.Add(buffer::Float, "SpecularMapWeight");
		rl.Add(buffer::Bool, "EnableNormalMap");
		rl.Add(buffer::Bool, "EnableSpecularMap");
		rl.Add(buffer::Bool, "HasGloss");
		rl.Add(buffer::Float, "SpecularIntensity");
	}

	if ((ret.test(hl2_RESULT_SPECULAR) || ret.test(hl2_RESULT_DIFFUSE)) && !ret.test(hl2_RESULT_NORMALMAP))
	{
		requiredShader = "PhongSpecular";
		rl.Add(buffer::Bool, "EnableSpecular");
		rl.Add(buffer::Bool, "HasGloss");
		rl.Add(buffer::Float, "SpecularPowerConst");
	}

	return { ret, rl, requiredShader };
}

engine::CModel::~CModel() noexcept
{
}

#pragma endregion CModel

static void _Fullbright_Chg(engine::CConVar*)
{
	auto& con = engine::CConsole::Instance();
	engine::CConCmd* cmd = con.QueryConCmd("reload");
	assert(cmd != nullptr);
	cmd->Execute({});
}
