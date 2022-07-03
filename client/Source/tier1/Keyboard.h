#pragma once

#include <stdint.h>
#include <queue>
#include <bitset>

#include "tier0/DLL.h"
#include "tier0/GetSet.h"

namespace hl2
{

class CWindow;

class DLLEXP CKeyboardEvent
{
public:
	enum class Type
	{
		Press,
		Release,
		Invalid
	};

public:
	CKeyboardEvent(Type _Type = Type::Invalid, uint8_t _Code = 0u)
		: m_Type(_Type),
		  m_Code(_Code)
	{ };

	_HL2_BASIC_GETTER(Type, Type)
	_HL2_BASIC_GETTER(uint8_t, Code)

	_HL2_BASIC_EQCMP(Press, m_Type, Type::Press)
	_HL2_BASIC_EQCMP(Release, m_Type, Type::Release)
	_HL2_BASIC_EQCMP(Invalid, m_Type, Type::Invalid)
private:
	Type m_Type;
	uint8_t m_Code;
};

class DLLEXP CKeyboard
{
	friend class CWindow;
public:
	using Event = CKeyboardEvent;
public:
	CKeyboard() = default;
	CKeyboard(const CKeyboard&) = delete;
	CKeyboard& operator=(const CKeyboard&) = delete;

	bool IsKeyPressed(uint8_t _Keycode) const noexcept;
	Event ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void FlushKey() noexcept;

	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void SetAutorepeat(bool _Value) noexcept;
	bool AutorepeatState() const noexcept;
private:
	void OnKeyPressed(uint8_t _Keycode) noexcept;
	void OnKeyReleased(uint8_t _Keycode) noexcept;
	void OnChar(char _Keycode) noexcept;
	void ClearState() noexcept;

	template<class T>
	static void TrimBuffer(std::queue<T>& _Buffer) noexcept;
private:
	static constexpr unsigned int KB_KEYS = 256u;
	// ^^^^ Maximum keys m_Keystates can track ---- vvvv Maximum keys in queue
	static constexpr unsigned int KB_MAX_KEYS = 16u;

	bool m_AutorepeatEnabled = false;
	std::bitset<KB_KEYS> m_Keystates;
	std::queue<Event> m_Keybuffer;
	std::queue<char> m_Charbuffer;
};

}
