#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>
#include <engine_tier1/dxprim/draw/BaseMiddlewareDraw.h>

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
public:
	CNode(std::vector<CMesh*> _Meshptrs, const DirectX::XMMATRIX& _Transform) noexcept;
	void Draw(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _AccumTransform) noexcept;
private:
	void AddChild(std::unique_ptr<CNode> _Child) noexcept;
private:
	std::vector<CMesh*> m_MeshPtrs;
	std::vector<std::unique_ptr<CNode>> m_NodePtrs;
	mutable DirectX::XMFLOAT4X4 m_Transform;
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
	void Draw(CGraphicalOutput& _Gfx, DirectX::XMMATRIX _Transform) const;
private:
	static std::unique_ptr<CMesh> ParseMesh(CGraphicalOutput& _Gfx, const aiMesh& _Mesh);
	std::unique_ptr<CNode> ParseNode(const aiNode& _Node);

	std::unique_ptr<CNode> m_Root;
	std::vector<std::shared_ptr<CMesh>> m_MeshPtrs;
};

}
