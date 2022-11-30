#pragma once

#include <concepts>
#include <unknwn.h>

namespace engine
{

template<class _Ty>
concept _Cn_IsComObj = std::derived_from<_Ty, IUnknown>;

template<_Cn_IsComObj _Ty>
class CUtl_ComPtr
{
public:
	CUtl_ComPtr() = default;
	CUtl_ComPtr(const CUtl_ComPtr& _Other)
	{
		if (m_Ptr != nullptr)
		{
			m_Ptr->Release();
		}

		m_Ptr = _Other.m_Ptr;
	}

	~CUtl_ComPtr()
	{
		if (m_Ptr != nullptr)
			m_Ptr->Release();
	}

	_Ty* Get() const noexcept
	{
		return m_Ptr;
	}

	_Ty** GetAddressOf() noexcept
	{
		return &m_Ptr;
	}

	_Ty* operator->() noexcept
	{
		return m_Ptr;
	}

	_Ty** operator&() noexcept
	{
		return this->GetAddressOf();
	}
private:
	_Ty* m_Ptr = nullptr;
};

}
