// /==========================================================================\
// |dynamic_constant.h - Constant buffer with "reflection" capabilities		  |
// |==========================================================================|
// |TODO: Should this be actually in dxprim folder? Seems too much complex and|
// |does not call any APIs of D3D.											  |
// \==========================================================================/

#pragma once

#include <string>
#include <optional>
#include <memory>
#include <typeinfo>
#include <vector>
#include <DirectXMath.h>

#ifndef NDEBUG
#define __hl2_CHECK_ATTRIBMAP
#endif

// Leaf types that generate enum elements
#define __hl2_LEAF_ELEMENT_TYPES \
	K(Float)                     \
	K(Float2)                    \
	K(Float3)                    \
	K(Float4)                    \
	K(Matrix)                    \
	K(Bool)

namespace engine
{

namespace buffer
{

enum LeafType
{
#define K(_E) _E,
	__hl2_LEAF_ELEMENT_TYPES
#undef K
		Struct,
	Array,
	Empty
};

#pragma region AttribMap and its specializations

// Attrib on each leaf type
template <LeafType _Ty_Type>
struct AttribMap
{
	using SystemType = void;					 // Type used in CPU side
	static constexpr size_t ShaderSize = 0u;	 // Type size in GPU side
	static constexpr const char* Mangled = "??"; // Mangled code
	static constexpr bool Valid = false;		 // Used by the __hl2_CHECK_ATTRIBMAP static_assert below
};

template <>
struct AttribMap<Float>
{
	using SystemType = float;
	static constexpr size_t ShaderSize = sizeof(SystemType);
	static constexpr const char* Mangled = "F0";
	static constexpr bool Valid = true;
};

template <>
struct AttribMap<Float2>
{
	using SystemType = DirectX::XMFLOAT2;
	static constexpr size_t ShaderSize = sizeof(SystemType);
	static constexpr const char* Mangled = "F2";
	static constexpr bool Valid = true;
};

template <>
struct AttribMap<Float3>
{
	using SystemType = DirectX::XMFLOAT3;
	static constexpr size_t ShaderSize = sizeof(SystemType);
	static constexpr const char* Mangled = "F3";
	static constexpr bool Valid = true;
};

template <>
struct AttribMap<Float4>
{
	using SystemType = DirectX::XMFLOAT4;
	static constexpr size_t ShaderSize = sizeof(SystemType);
	static constexpr const char* Mangled = "F4";
	static constexpr bool Valid = true;
};

template <>
struct AttribMap<Matrix>
{
	using SystemType = DirectX::XMMATRIX;
	static constexpr size_t ShaderSize = sizeof(SystemType);
	static constexpr const char* Mangled = "MT";
	static constexpr bool Valid = true;
};

template <>
struct AttribMap<Bool>
{
	using SystemType = bool;
	static constexpr size_t ShaderSize = sizeof(int); // Bool is 4 bytes in shader code
	static constexpr const char* Mangled = "BL";
	static constexpr bool Valid = true;
};

#pragma endregion

#ifdef __hl2_CHECK_ATTRIBMAP
#define K(_E) static_assert(AttribMap<_E>::Valid, "Missing map implementation for leaf " #_E);
__hl2_LEAF_ELEMENT_TYPES
#undef K
#endif

	template <class _Ty>
	struct ReverseLookupAttribMap // clang-format: keep
{
	static constexpr LeafType Type = Empty;
	static constexpr bool Valid = false;
};

#define K(_E)                                                         \
	template <>                                                       \
	struct ReverseLookupAttribMap<typename AttribMap<_E>::SystemType> \
	{                                                                 \
		static constexpr LeafType Type = _E;                          \
		static constexpr bool Valid = true;                           \
	};
__hl2_LEAF_ELEMENT_TYPES
#undef K

#pragma region Classes

	class CLayoutElement // clang-format: keep
{
private:
	struct ExtraDataBase
	{
		virtual ~ExtraDataBase() = default;
	};

	friend class CRawLayout;
	friend struct ExtraData;

public:
	CLayoutElement() = default;
	CLayoutElement(LeafType _Type);

	std::string GetSignature() const noexcept;
	bool Exists() const noexcept;
	std::pair<size_t, const CLayoutElement*> CalculateIndexingOffset(size_t _Offset,
																	 size_t _Index) const noexcept;
	CLayoutElement& GetTypeOfArray() noexcept;
	const CLayoutElement& GetTypeOfArray() const noexcept;
	void GetOffsetLimits(size_t* _Out_Begin, size_t* _Out_End) const noexcept;
	size_t GetSizeInBytes() const noexcept;
	CLayoutElement& operator[](const std::string& _Key) noexcept;
	const CLayoutElement& operator[](const std::string& _Key) const noexcept;
	CLayoutElement& Add(LeafType _Type, std::string _Name) noexcept;
	CLayoutElement& Set(LeafType _Type, size_t _Index) noexcept;

	template <class _Ty>
	size_t GetOffset() const noexcept
	{
#define K(E)                                             \
	case E:                                              \
		assert(typeid(AttribMap<E>::SystemType) == _Ty); \
		return m_Offset;
		__hl2_LEAF_ELEMENT_TYPES default : assert("Tried to resolve non-leaf element" && false);
		return 0u;
	}
#undef K

private:
	bool ValidateSymbol(const std::string& _Name) noexcept;
	CLayoutElement& GetEmptyElement() const noexcept;
	std::string GetSignatureStruct() const noexcept;
	std::string GetSignatureArray() const noexcept;
	size_t AdvanceIfCrossesBoundary(size_t, size_t) noexcept;
	bool CrossesBoundary(size_t _Offset, size_t _Size) noexcept;
	size_t AdvanceToBoundary(size_t) noexcept;

	size_t Finalize(size_t _Offset) noexcept;
	size_t FinalizeStruct(size_t _Offset) noexcept;
	size_t FinalizeArray(size_t _Offset) noexcept;

private:
	size_t m_Offset;
	LeafType m_Type = Empty;
	std::unique_ptr<ExtraDataBase> m_ExtraData;
};

class CLayout
{
	friend class CLayoutRegistry;
	friend class CBuffer;

public:
	size_t GetSizeBytes() const noexcept;
	std::string GetSignature() const noexcept;

protected:
	CLayout(std::shared_ptr<CLayoutElement> _Root);
	std::shared_ptr<CLayoutElement> m_Root;
};

class CRawLayout : public CLayout
{
	friend class CLayoutRegistry;
	friend class CBuffer;

public:
	CRawLayout() noexcept;

	CLayoutElement& operator[](const std::string& _Key) noexcept;

	CLayoutElement& Add(LeafType _type, const std::string& _key) noexcept
	{
		return m_Root->Add(_type, _key);
	}

private:
	void ClearRoot() noexcept;
	std::shared_ptr<CLayoutElement> DeliverRoot() noexcept;
};

class CLayoutView : public CLayout
{
	friend class CLayoutRegistry;
	friend class CBuffer;

public:
	CLayoutView(std::shared_ptr<CLayoutElement> _Root) noexcept;
	CLayoutView(CRawLayout ly) noexcept;
	const CLayoutElement& operator[](const std::string& _Key) const noexcept;
	std::shared_ptr<CLayoutElement> ShareRoot() const noexcept;

private:
	std::shared_ptr<CLayoutElement> MoveRoot() const;
};

class CPtr
{
	friend class CElementRef;

public:
	template <class _Ty>
	_Ty* operator*() const noexcept
	{
		static_assert(ReverseLookupAttribMap<std::remove_const_t<_Ty>>::Valid, "Unsupported system type");
		return &static_cast<_Ty&>(*m_Ref);
	}

private:
	CPtr(class CElementRef*) noexcept;
	class CElementRef* m_Ref;
};

// Const version of CPtr
class CPtrView
{
	friend class CElementRefView;

public:
	template <class _Ty>
	const _Ty* operator*() const noexcept
	{
		static_assert(ReverseLookupAttribMap<std::remove_const_t<_Ty>>::Valid, "Unsupported system type");
		return &static_cast<_Ty&>(*m_Ref);
	}

private:
	CPtrView(class CElementRefView*) noexcept;
	const class CElementRefView* m_Ref;
};

class CElementRefView
{
	friend class CBuffer;
	friend class CElementRef;

public:
	bool Exists() const noexcept;

	// Struct keying
	CElementRefView operator[](const std::string& _key) const noexcept;
	// Array keying
	CElementRefView operator[](size_t _Index) const noexcept;

	CPtrView operator&() const noexcept;

	template <class _Ty>
	operator const _Ty&() const noexcept
	{
		static_assert(ReverseLookupAttribMap<std::remove_const_t<_Ty>>::Valid, "Unsupported system type");
		return *reinterpret_cast<_Ty*>(m_Bytes + m_Offset + m_Layout->GetOffset<_Ty>());
	}

private:
	CElementRefView(const CLayoutElement* _lel, const char* _Bytes, size_t _Offset) noexcept;
	size_t m_Offset;
	const CLayoutElement* m_Layout;
	const char* m_Bytes;
};

class CElementRef
{
	friend class CBuffer;

public:
	operator CElementRefView() const noexcept;
	bool Exists() const noexcept;
	CElementRef operator[](const std::string& _key) const noexcept;
	CElementRef operator[](size_t _index) const noexcept;

	template <class _Ty>
	bool SetExists(const _Ty& val) noexcept
	{
		if (this->Exists())
		{
			(*this) = val;
			return true;
		}
		return false;
	}

	CPtr operator&() const noexcept;

	template <class _Ty>
	operator _Ty&() const noexcept
	{
		static_assert(ReverseLookupAttribMap<std::remove_const_t<_Ty>>::Valid, "Unsupported system type");
		return *reinterpret_cast<_Ty*>(m_Bytes + m_Offset + m_Layout->GetOffset<_Ty>());
	}

	template <class _Ty>
	_Ty& operator=(const _Ty& _Rhs)
	{
		static_assert(ReverseLookupAttribMap<std::remove_const_t<_Ty>>::Valid, "Unsupported system type");
		return static_cast<_Ty&>(*this) = _Rhs;
	}

private:
	CElementRef(const CLayoutElement* _Layout, char* _Bytes, size_t _Offset) noexcept;

private:
	char* m_Bytes;
	size_t m_Offset;
	const CLayoutElement* m_Layout;
};

class CBuffer
{
public:
	CBuffer(CRawLayout&& _Ly) noexcept;
	CBuffer(const CLayoutView& _Ly) noexcept;
	CBuffer(CLayoutView&& _Ly) noexcept;
	CBuffer(const CBuffer&) noexcept;
	CBuffer(CBuffer&&) noexcept;

	CElementRef operator[](const std::string& _Key) noexcept;
	CElementRefView operator[](const std::string& _Key) const noexcept;
	const char* GetData() const noexcept;
	size_t SizeOf() const noexcept;
	const CLayoutElement& GetLayoutElement() const noexcept;
	// Layouts must match
	void CopyFrom(const CBuffer&) noexcept;
	std::shared_ptr<CLayoutElement> ShareLayoutRoot() const noexcept;

private:
	std::shared_ptr<CLayoutElement> m_Layout;
	std::vector<char> m_Bytes;
};

#pragma endregion

} // namespace buffer

} // namespace engine
