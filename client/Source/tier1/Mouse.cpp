#include "Mouse.h"
#include "tier0lib/Win32.h"

std::pair<int, int> hl2::CMouse::GetCoordinates() const noexcept
{
	return { m_X, m_Y };
}

int hl2::CMouse::GetX() const noexcept
{
	return m_X;
}

int hl2::CMouse::GetY() const noexcept
{
	return m_Y;
}

bool hl2::CMouse::LeftPressed() const noexcept
{
	return m_LeftPressed;
}

bool hl2::CMouse::RightPressed() const noexcept
{
	return m_RightPressed;
}

bool hl2::CMouse::IsEmpty() const noexcept
{
	return m_Buffer.empty();
}

hl2::CMouseEvent hl2::CMouse::Read() noexcept
{
	if (m_Buffer.size() > 0u)
	{
		hl2::CMouseEvent e = m_Buffer.front();
		m_Buffer.pop();
		return e;
	}
	
	return hl2::CMouseEvent();
}

void hl2::CMouse::Flush() noexcept
{
	m_Buffer = std::queue<CMouseEvent>();
}

bool hl2::CMouse::IsInClientRegion() const noexcept
{
	return m_IsInWindow;
}

void hl2::CMouse::OnMouseMove(int _X, int _Y) noexcept
{
	m_X = _X;
	m_Y = _Y;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::Move, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnLeftPressed(int _X, int _Y) noexcept
{
	m_X = _X;
	m_Y = _Y;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::LPress, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnLeftReleased(int _X, int _Y) noexcept
{
	m_X = _X;
	m_Y = _Y;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::LRelease, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnRightPressed(int _X, int _Y) noexcept
{
	m_X = _X;
	m_Y = _Y;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::RPress, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnRightReleased(int _X, int _Y) noexcept
{
	m_X = _X;
	m_Y = _Y;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::RPress, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnMouseEnter() noexcept
{
	m_IsInWindow = true;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::EnterClientRegion, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnMouseLeave() noexcept
{
	m_IsInWindow = false;
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::LeaveClientRegion, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnWheelUp(int _Ignored, int _Y) noexcept
{
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::WheelUp, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnWheelDown(int _Ignored, int _Y) noexcept
{
	m_Buffer.push(hl2::CMouseEvent(hl2::CMouseEvent::EType::WheelDown, *this));
	this->TrimBuffer();
}

void hl2::CMouse::OnWheelDelta(int _X, int _Y, int _Delta) noexcept
{
	m_DeltaCarry += _Delta;
	while (m_DeltaCarry >= WHEEL_DELTA)
	{
		m_DeltaCarry -= WHEEL_DELTA;
		this->OnWheelUp(0, _Y);
	}

	while (m_DeltaCarry <= -WHEEL_DELTA)
	{
		m_DeltaCarry += WHEEL_DELTA;
		this->OnWheelDown(0, _Y);
	}
}

void hl2::CMouse::TrimBuffer() noexcept
{
	while (m_Buffer.size() > BUFFER_SIZE)
	{
		m_Buffer.pop();
	}
}
