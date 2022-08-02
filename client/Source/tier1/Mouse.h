#pragma once

#include <queue>
#include <optional>

#include "tier0/GetSet.h"
#include "tier0/DLL.h"

namespace hl2
{

struct RawMiceDelta
{
	int X, Y;
};

class DLLEXP CMouse
{
	friend class CWindow;
public:
	class DLLEXP Event
	{
	public:
		enum class EType
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			EnterClientRegion,
			LeaveClientRegion,
			Invalid
		};
	public:
		Event(EType _Type = EType::Invalid)
			: m_Type(_Type),
			m_LeftPressed(false), m_RightPressed(false),
			m_X(0), m_Y(0)
		{};

		Event(EType _Type, CMouse& _Parent)
			: m_Type(_Type),
			m_LeftPressed(_Parent.LeftPressed()), m_RightPressed(_Parent.RightPressed()),
			m_X(_Parent.GetX()), m_Y(_Parent.GetY())
		{	}

		std::pair<int, int> GetCoordinates()
		{
			return { m_X, m_Y };
		}

		_HL2_BASIC_EQCMP(Invalid, m_Type, EType::Invalid);
		_HL2_BASIC_GETTER(EType, Type);

		_HL2_BASIC_GETTER(int, X);
		_HL2_BASIC_GETTER(int, Y);

		_HL2_BASIC_GETTER(bool, LeftPressed);
		_HL2_BASIC_GETTER(bool, RightPressed);
	private:
		EType m_Type;
		int m_X, m_Y;
		bool m_LeftPressed, m_RightPressed;
	};
public:
	CMouse() = default;
	CMouse(const CMouse&) = delete;
	CMouse& operator=(const CMouse&) = delete;

	std::pair<int, int> GetCoordinates() const noexcept;
	std::optional<RawMiceDelta> ReadRawDelta() noexcept;
	int GetX() const noexcept;
	int GetY() const noexcept;

	bool RawEnabled() const noexcept;
	void ToggleRawCapture() noexcept;

	bool LeftPressed() const noexcept;
	bool RightPressed() const noexcept;

	Event Read() noexcept;

	bool IsEmpty() const noexcept;
	void Flush() noexcept;

	bool IsInClientRegion() const noexcept;
private:
	// vvvvv Routines called by hl2::CWindow vvvvv
	void OnMouseMove(int _X, int _Y) noexcept;
	void OnLeftPressed(int _X, int _Y) noexcept;
	void OnLeftReleased(int _X, int _Y) noexcept;
	void OnRightPressed(int _X, int _Y) noexcept;
	void OnRightReleased(int _X, int _Y) noexcept;
	
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;

	void OnWheelUp(int _Ignored, int _Y) noexcept;
	void OnWheelDown(int _Ignored, int _Y) noexcept;
	void OnWheelDelta(int _X, int _Y, int _Delta) noexcept;

	void OnRawDelta(int _X, int _Y);
	// ^^^^^ Routines called by hl2::CWindow ^^^^^

	template<class _Ty>
	void TrimBuffer(std::queue<_Ty> _Queue) noexcept
	{
		while (m_Buffer.size() > BUFFER_SIZE)
		{
			m_Buffer.pop();
		}
	}
	
private:
	static constexpr unsigned int BUFFER_SIZE = 64u;
	int m_X, m_Y;
	bool m_LeftPressed = false, m_RightPressed = false,
		m_IsInWindow = false;
	std::queue<Event> m_Buffer;
	std::queue<RawMiceDelta> m_RawBuffer;
	int m_DeltaCarry;
	bool m_CaptureRaw = false;
};

using CMouseEvent = CMouse::Event;

}
