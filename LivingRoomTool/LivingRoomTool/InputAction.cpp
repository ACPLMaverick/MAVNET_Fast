#include "InputAction.h"

namespace InputActionKeyHelper
{
	bool IsMouse(InputActionKey key)
	{
		return key >= InputActionKey::kMouseUp && key <= InputActionKey::kMouseScrollDown;
	}

	bool IsMouseMove(InputActionKey key)
	{
		return key >= InputActionKey::kMouseUp && key <= InputActionKey::kMouseRight;
	}
}

InputAction::InputAction(InputActionKey a_key, bool a_bPressed)
	: m_key(a_key)
{
	m_value.m_bPressed = a_bPressed;
}

InputAction::InputAction(InputActionKey a_key, int32_t a_value)
	: m_key(a_key)
{
	m_value.m_IntValue = a_value;
}