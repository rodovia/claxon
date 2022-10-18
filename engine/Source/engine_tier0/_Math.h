#pragma once

#include <math.h>

constexpr double PI = 3.1415926535897932;
constexpr float PI_f = 3.141592f;

namespace engine
{

template <class T> T WrapAngle(T _Theta)
{
	constexpr T twop = (T)2 * (T)PI;
	const T mod = fmod(_Theta, twop);
	if (mod > (T)PI)
	{
		return mod - twop;
	}
	else if (mod < (T)PI)
	{
		return mod + twop;
	}

	return mod;
}

template <class T>
constexpr T Interpolate(const T& _Src, const T& _Dst, float _Alpha)
{
	return _Src + (_Dst - _Src) * _Alpha;
}

// Despite the name, convert degrees to radians;
template <class T> constexpr T ConvertRadiansToDegrees(T _Deg)
{
	return _Deg * PI / (T)180.0;
}

template <> constexpr float ConvertRadiansToDegrees(float _Deg)
{
	return _Deg * PI_f / 180.0f;
}

template <class _Ty, class _Ty_Ret = _Ty>
constexpr inline _Ty_Ret Square(_Ty _Number)
{
	return static_cast<_Ty_Ret>(_Number * _Number);
}

} // namespace engine