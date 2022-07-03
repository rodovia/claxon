#pragma once

#include "DLL.h"

#include <chrono>
#include <concepts>
#include <stdint.h>

namespace hl2
{

template<class T>
concept _DurationCastable = requires {
	typename std::chrono::duration<T>;
};

namespace time_types
{

using nanoseconds = long long;
using microseconds = long long;
using milliseconds = long long;
using seconds = long long;
using minutes = int;
using hours = int;

}

typedef std::chrono::time_point<std::chrono::steady_clock> _MonotonicTimePoint;

// A RAII timer.
class CBasicTimer
{
public:
	CBasicTimer() noexcept
	{
		m_ClStart = std::chrono::steady_clock::now();
	}

	inline std::chrono::duration<float> Mark() noexcept
	{
		std::chrono::duration<float> dur = this->Peek();
		m_ClStart = std::chrono::steady_clock::now();
		return dur;
	}

	inline std::chrono::duration<float> Peek() noexcept
	{
		hl2::_MonotonicTimePoint now = std::chrono::steady_clock::now();
		std::chrono::duration<float> dur = now - m_ClStart;
		return dur;
	}
private:
	_MonotonicTimePoint m_ClStart;
};

}
