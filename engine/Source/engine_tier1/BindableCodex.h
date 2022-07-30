#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <type_traits>

namespace engine
{

class CCodex
{
public:
	template<class _Ty, typename... _Ty_Params>
	static std::shared_ptr<_Ty> Query(CGraphicalOutput& _Gfx, _Ty_Params&&..._Params) noexcept
	{
		static_assert(std::is_base_of<CBase_Bind, _Ty>(), "Cannot query types who not inherit from CBase_Bind");
		return Instance().Query1<_Ty>(_Gfx, std::forward<_Ty_Params>(_Params)...);
	}

private:
	template<class _Ty, typename... _Ty_Params>
	std::shared_ptr<_Ty> Query1(CGraphicalOutput& _Gfx, _Ty_Params&&... _Params) noexcept
	{
		const std::string key = _Ty::Discriminate(std::forward<_Ty_Params>(_Params)...);
		const auto& i = m_Binds.find(key);
		if (i == m_Binds.end())
		{
			auto bind = std::make_shared<_Ty>(_Gfx, std::forward<_Ty_Params>(_Params)...);
			m_Binds[key] = bind;
			return bind;
		}

		return std::dynamic_pointer_cast<_Ty>(i->second);
	}

	static CCodex& Instance()
	{
		static CCodex i;
		return i;
	}

	std::unordered_map<std::string, std::shared_ptr<CBase_Bind>> m_Binds;
};

}
