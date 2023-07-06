#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>

class Keyboard
{
public:
	enum KeyState
	{
		KEY_NONE = 0,
		KEY_PRESSED = 1 << 0,
		KEY_HELD = 1 << 1,
		KEY_RELEASED = 1 << 2,
	};

	Keyboard();

	void Update();

	bool IsKeyPressed(int keyIndex) const;
	bool IsKeyHeld(int keyIndex) const;
	bool IsKeyReleased(int keyIndex) const;

	unsigned char GetKeyState(int keyIndex) const;

private:
	static const int NUM_KEYS = 256;
	const unsigned char HIGH_BIT = 1 << 7;

	unsigned char m_cKeys[NUM_KEYS];
};

#endif // !KEYBOARD_H
