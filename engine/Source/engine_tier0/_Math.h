// LEMBRETE AO CIDADÃO: O nome deste ficheiro seria "Math.h". Porém, devido a
// limitações técnicas quanto à insensibilidade do Windows com caminhos de
// caixa alta e caixa baixa, utilizou-se "_Math.h".

#pragma once

#include <math.h>

constexpr double PI = 3.1415926535897932;
constexpr float PI_f = 3.141592f;

namespace engine
{

template <class T> T WrapAngle(T _Theta)
{
	const T modded = fmod(_Theta, (T)2.0 * PI);
	return (modded > (T)PI) ? (modded - (T)2.0 * PI) : modded;
}

template <class T>
constexpr T Interpolate(const T& _Src, const T& _Dst, float _Alpha)
{
	return _Src + (_Dst - _Src) * _Alpha;
}

template <class T> constexpr T ConvertRadiansToDegrees(T _Deg)
{
	return _Deg * PI / (T)180.0;
}

template <class _Ty, class _Ty_Ret = _Ty>
constexpr inline _Ty_Ret Square(_Ty _Number)
{
	return static_cast<_Ty_Ret>(_Number * _Number);
}

} // namespace engine