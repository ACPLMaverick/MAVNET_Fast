#pragma once

#include "Common.h"
#include "AdvancedBindingEditorDialog.h"

enum class GamepadButtons : uint32_t;
enum class InputActionKey : uint8_t;

class InputBinding;

class AdvancedBindingEditor : public QObject
{
	Q_OBJECT

public:

	AdvancedBindingEditor();
	~AdvancedBindingEditor();

	bool Open(QWidget* parent, InputBinding& binding);

private:

	AdvancedBindingEditorDialog* m_dialog{ nullptr };
};

