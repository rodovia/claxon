#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>
#include <engine_tier1/dxprim/draw/BaseMiddlewareDraw.h>

#include <optional>

#include <assimp/mesh.h>

namespace engine
{

class CMesh : public CBase_MiddlewareDraw<CMesh>
{
public:
	CMesh(CGraphicalOutput& _Gfx, std::vector<std::unique_ptr<CBase_Bind>> _BindPtrs);
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
	CNode(const std::string& _Name, std::vector<CMesh*> _Meshptrs, const DirectX::XMMATRIX& _Transform) noexcept;
	void Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX _Transform) noexcept;
private:
	void RenderTree(int& _NodeIndex, std::optional<int>& _SelectedNodeIndex, CNode*& _Node) const noexcept;
	void AddChild(std::unique_ptr<CNode> _Child) noexcept;
private:
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
	CModel(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Draw(CGraphicalOutput& _Gfx) const;
	void ShowDiagWindow(const char* _Name);
	~CModel() noexcept;
private:
	static std::unique_ptr<CMesh> ParseMesh(CGraphicalOutput& _Gfx, const aiMesh& _Mesh);
	std::unique_ptr<CNode> ParseNode(const aiNode& _Node);

	std::unique_ptr<CNode> m_Root;
	std::vector<std::unique_ptr<CMesh>> m_MeshPtrs;
	std::unique_ptr<class CModelDiagWindow> m_ModelWindow;
};

}
