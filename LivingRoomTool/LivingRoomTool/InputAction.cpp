#include "InputAction.h"

namespace InputActionKeyHelper
{
	const InputActionKey g_MouseFirst = InputActionKey::kMouseUp;
	const InputActionKey g_MouseLast = InputActionKey::kMouseScrollDown;
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