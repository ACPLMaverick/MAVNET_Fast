#pragma once

#include "Common.h"
#include "InputActionConverter.h"
#include "InputLibraryWrapper.h"

#include <unordered_set>

enum class InputActionKey : uint8_t;

class InputActionKeypressRetriever
{
public:

	InputActionKeypressRetriever();
	~InputActionKeypressRetriever();

	void Init();

	// Returns true when the press ended and it can be retrieved.
	bool Tick();

	void RetrieveAndCleanup(std::vector<InputActionKey>& outKeypresses);
	InputActionKey RetrieveAndCleanup();

private:

	inline bool IsInit() const { return m_keyboard != nullptr; }
	inline void ReadKeyboard();
	inline void ReadMouse();
	inline void FindPressed();
	inline bool IsPressEnded();
	void Cleanup();

	std::unordered_set<InputActionKey> m_pressed;
	InputActionConverter m_converter;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	uint8_t m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	bool m_bPressStarted;
};

