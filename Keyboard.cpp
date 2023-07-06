#include "Keyboard.h"

Keyboard::Keyboard()
{
	ZeroMemory(m_cKeys, sizeof(m_cKeys));
}

void Keyboard::Update()
{
	BYTE keyState[256];
	ZeroMemory(keyState, sizeof(keyState));
	GetKeyboardState(keyState);

	for (int i = 0; i <= NUM_KEYS; i++)
	{
		bool isDown = (keyState[i] & HIGH_BIT) != 0;
		bool wasDown = (m_cKeys[i] & KEY_HELD) != 0;

		unsigned char newState = 0;

		if (isDown)
			newState |= KEY_HELD;
		
		if (isDown && !wasDown)
			newState |= KEY_PRESSED;
		
		if (!isDown && wasDown)
			newState |= KEY_RELEASED;

		m_cKeys[i] = newState;
	}
}

bool Keyboard::IsKeyPressed(int virtualKey) const
{
	return (m_cKeys[virtualKey] & KEY_PRESSED) != 0;
}

bool Keyboard::IsKeyHeld(int virtualKey) const
{
	return (m_cKeys[virtualKey] & KEY_HELD) != 0;
}

bool Keyboard::IsKeyReleased(int virtualKey) const
{
	return (m_cKeys[virtualKey] & KEY_RELEASED) != 0;
}

unsigned char Keyboard::GetKeyState(int virtualKey) const
{
	return m_cKeys[virtualKey];
}
