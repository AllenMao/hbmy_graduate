#include "Input.h"

CInput::CInput( HWND window )
{
	// Store the handle to the parent window.
	m_window = window;

	// Create a DirectInput interface.
	DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_di, NULL );

	// Create, prepare, and aquire the keyboard device.
	m_di->CreateDevice( GUID_SysKeyboard, &m_keyboard, NULL );
	m_keyboard->SetDataFormat( &c_dfDIKeyboard );
	m_keyboard->SetCooperativeLevel( m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	m_keyboard->Acquire();

	// Create, prepare, and aquire the mouse device.
	m_di->CreateDevice( GUID_SysMouse, &m_mouse, NULL );
	m_mouse->SetDataFormat( &c_dfDIMouse );
	m_mouse->SetCooperativeLevel( m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	m_mouse->Acquire();

	// Start the press stamp.
	m_pressStamp = 0;
}

//-----------------------------------------------------------------------------
// The input class destructor.
//-----------------------------------------------------------------------------
CInput::~CInput()
{
	SafeRelease( m_di );
	SafeRelease( m_keyboard );
	SafeRelease( m_mouse );
}

//-----------------------------------------------------------------------------
// Updates the state of both the keyboard and mouse device.
//-----------------------------------------------------------------------------
void CInput::Update()
{
	static HRESULT result;

	// Poll the keyboard until it succeeds or returns an unknown error.
	while( true )
	{
		m_keyboard->Poll();
		if( SUCCEEDED( result = m_keyboard->GetDeviceState( 256, (LPVOID)&m_keyState ) ) )
			break;
		if( result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return;

		// Reacquire the device if the focus was lost.
		if( FAILED( m_keyboard->Acquire() ) )
			return;
	}

	// Poll the mouse until it succeeds or returns an unknown error.
	while( true )
	{
		m_mouse->Poll();
		if( SUCCEEDED( result = m_mouse->GetDeviceState( sizeof( DIMOUSESTATE ), &m_mouseState ) ) )
			break;
		if( result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return;

		// Reacquire the device if the focus was lost.
		if( FAILED( m_mouse->Acquire() ) )
			return;
	}

	// Get the relative position of the mouse.
	GetCursorPos( &m_position );
	ScreenToClient( m_window, &m_position );

	// Increment the press stamp.
	m_pressStamp++;
}

//-----------------------------------------------------------------------------
// Returns true if the given key is pressed.
// Note: Consistent presses will return false when using the press stamp.
//-----------------------------------------------------------------------------
bool CInput::GetKeyPress( char key, bool ignorePressStamp )
{
	if( ( m_keyState[key] & 0x80 ) == false )
		return false;

	bool pressed = true;

	if( ignorePressStamp == false )
		if( m_keyPressStamp[key] == m_pressStamp - 1 || m_keyPressStamp[key] == m_pressStamp )
			pressed = false;

	m_keyPressStamp[key] = m_pressStamp;

	return pressed;
}

//-----------------------------------------------------------------------------
// Returns true if the given button is pressed.
// Note: Consistent presses will return false when using the press stamp.
//-----------------------------------------------------------------------------
bool CInput::GetButtonPress( char button, bool ignorePressStamp )
{
	if( ( m_mouseState.rgbButtons[button] & 0x80 ) == false )
		return false;

	bool pressed = true;

	if( ignorePressStamp == false )
		if( m_buttonPressStamp[button] == m_pressStamp - 1 || m_buttonPressStamp[button] == m_pressStamp )
			pressed = false;

	m_buttonPressStamp[button] = m_pressStamp;

	return pressed;
}

//-----------------------------------------------------------------------------
// Returns the x position of the mouse.
//-----------------------------------------------------------------------------
long CInput::GetPosX()
{
	return m_position.x;
}

//-----------------------------------------------------------------------------
// Returns the y position of the mouse.
//-----------------------------------------------------------------------------
long CInput::GetPosY()
{
	return m_position.y;
}

//-----------------------------------------------------------------------------
// Returns the change in the mouse's x coordinate.
//-----------------------------------------------------------------------------
long CInput::GetDeltaX()
{
	return m_mouseState.lX;
}

//-----------------------------------------------------------------------------
// Returns the change in the mouse's y coordinate.
//-----------------------------------------------------------------------------
long CInput::GetDeltaY()
{
	return m_mouseState.lY;
}

//-----------------------------------------------------------------------------
// Returns the change in the mouse's scroll wheel.
//-----------------------------------------------------------------------------
long CInput::GetDeltaWheel()
{
	return m_mouseState.lZ;
}