#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>

#include <optional>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include "dxprim/draw/BaseDraw.h"
#include <variant>
#include <filesystem>

namespace engine
{

__declspec(align(16)) struct PSMaterialConstant
{
	float SpecularPower = 3.1f;
	DirectX::XMFLOAT3 SpecularColor = {0.75f, 0.75f, 0.75f};
	float SpecularMapWeight = 0.671f;
	BOOL EnableNormalMap = TRUE,
		 EnableSpecularMap = TRUE,
		 HasGloss;
	float padding[4];
};

__declspec(align(16)) struct PSSolidColorConstant
{
	DirectX::XMFLOAT4A MaterialColor = {0.447970f, 0.327254f, 0.176283f, 1.0f};
	BOOL EnableSpecular = TRUE;
	BOOL HasGloss;
	float SpecularPowerConst;
	DirectX::XMFLOAT4A SpecularColor;
	float SpecularPower = 120.0f;
	float padd;
};

struct MODEL_DESCRIPTOR
{
	float Scale = 1.0f;
	bool AffectedByLight = true; // TODO: implement
	enum
	{
		NORMSPC_TANGENT = 0x100,
		NORMSPC_OBJECT
	} NormalMapSpace = NORMSPC_TANGENT; // TODO: implement
};

class CMesh : public CBase_Draw
{
public:
	CMesh(CGraphicalOutput& _Gfx, std::vector<std::shared_ptr<CBase_Bind>> _BindPtrs);
	void Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 m_Transform;
};

class CNode
{
	friend class CModel;
	friend class CModelDiagWindow;

public:
	CNode(int _Id, const std::string& _Name,
		  std::vector<CMesh*> _Meshptrs,
		  const DirectX::XMMATRIX& _Transform) noexcept;
	void Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX _Transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;

private:
	void RenderExtendedCtl(CGraphicalOutput& _Gfx, PSMaterialConstant& _Pmc);
	void RenderExtendedCtl(CGraphicalOutput& _Gfx, PSSolidColorConstant& _Pmc);

	void RenderTree(CNode*& _Node) const noexcept;
	void AddChild(std::unique_ptr<CNode> _Child) noexcept;

private:
	int m_Id;
	std::string m_Name;
	std::vector<CMesh*> m_MeshPtrs;
	std::vector<std::shared_ptr<CNode>> m_NodePtrs;
	DirectX::XMFLOAT4X4 m_BaseTransform;
	DirectX::XMFLOAT4X4 m_ApplTransform;
};

class _ENGINE_DLLEXP CModel
{
public:
	/*
	 * Even though assimp does not accept a wchar_t,
	 * I want to make my life easier by not having to call
	 * tier0::ConvertToMultiByteString explicitly everytime I use _GETPATH.
	 */
	CModel(CGraphicalOutput& _Gfx, std::wstring _Filename, MODEL_DESCRIPTOR _Options = {});
	void Draw(CGraphicalOutput& _Gfx) const;
	void ShowDiagWindow(CGraphicalOutput& _Gfx, const char* _Name);
	~CModel() noexcept;

private:
	std::unique_ptr<CMesh> ParseMesh(CGraphicalOutput& _Gfx, const aiMesh& _Mesh,
									 const aiMaterial* const* _Materials, const std::filesystem::path& _Path);
	std::unique_ptr<CNode> ParseNode(int& _NextId, const aiNode& _Node);

	std::unique_ptr<CNode> m_Root;
	MODEL_DESCRIPTOR m_Desc;
	std::vector<std::unique_ptr<CMesh>> m_MeshPtrs;
	std::unique_ptr<class CModelDiagWindow> m_ModelWindow;
};

} // namespace engine
