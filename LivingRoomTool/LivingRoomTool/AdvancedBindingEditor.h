#pragma once

#include "Common.h"
#include "AdvancedBindingEditorDialog.h"

enum class GamepadButtons : uint32_t;
enum class InputActionKey : uint8_t;

class InputBinding;
class LivingRoomTool;

class AdvancedBindingEditor : public QObject
{
	Q_OBJECT

public:

	AdvancedBindingEditor();
	~AdvancedBindingEditor();

	bool Open(LivingRoomTool* parent, InputBinding& binding);

private:

	AdvancedBindingEditorDialog* m_dialog{ nullptr };
};

