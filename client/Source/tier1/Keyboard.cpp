#include "Keyboard.h"

using CKeyboardEventType = hl2::CKeyboardEvent::Type;

bool hl2::CKeyboard::IsKeyPressed(uint8_t _Keycode) const noexcept
{
	return m_Keystates[_Keycode];
}

bool hl2::CKeyboard::IsKeyEmpty() const noexcept
{
	return m_Keybuffer.empty();
}

hl2::CKeyboardEvent hl2::CKeyboard::ReadKey() noexcept
{
	if (m_Keybuffer.size() > 0u)
	{
		hl2::CKeyboardEvent e = m_Keybuffer.front();
		m_Keybuffer.pop();
		e.IsInvalid();
		return e;
	}
	return hl2::CKeyboardEvent();
}

char hl2::CKeyboard::ReadChar() noexcept
{
	if (m_Charbuffer.size() > 0u)
	{
		uint8_t charcode = m_Charbuffer.front();
		m_Charbuffer.pop();
		return charcode;
	}

	return 0;
}

bool hl2::CKeyboard::CharIsEmpty() const noexcept
{
	return m_Charbuffer.empty();
}

void hl2::CKeyboard::FlushKey() noexcept
{
	m_Keybuffer = std::queue<Event>();
}

void hl2::CKeyboard::FlushChar() noexcept
{
	m_Charbuffer = std::queue<char>();
}

void hl2::CKeyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void hl2::CKeyboard::SetAutorepeat(bool _Value) noexcept
{
	m_AutorepeatEnabled = _Value;
}

bool hl2::CKeyboard::AutorepeatState() const noexcept
{
	return m_AutorepeatEnabled;
}

void hl2::CKeyboard::OnKeyPressed(uint8_t _Keycode) noexcept
{
	m_Keystates[_Keycode] = true;
	m_Keybuffer.push(hl2::CKeyboardEvent(CKeyboardEventType::Press, _Keycode));
	TrimBuffer(m_Keybuffer);
}

void hl2::CKeyboard::OnKeyReleased(uint8_t _Keycode) noexcept
{
	m_Keystates[_Keycode] = false;
	m_Keybuffer.push(hl2::CKeyboardEvent(CKeyboardEventType::Release, _Keycode));
	TrimBuffer(m_Keybuffer);
}

void hl2::CKeyboard::OnChar(char _Char) noexcept
{
	m_Charbuffer.push(_Char);
	TrimBuffer(m_Charbuffer);
}

void hl2::CKeyboard::ClearState() noexcept
{
	m_Keystates.reset();
}

template<class T>
void hl2::CKeyboard::TrimBuffer(std::queue<T>& _Buffer) noexcept
{
	while (_Buffer.size() > KB_MAX_KEYS)
	{
		_Buffer.pop();
	}
}
