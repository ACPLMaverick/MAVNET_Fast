#include "AdvancedBindingEditor.h"

#include "InputBinding.h"
#include "LivingRoomTool.h"

AdvancedBindingEditor::AdvancedBindingEditor()
{
}

AdvancedBindingEditor::~AdvancedBindingEditor()
{
	LRT_SafeDelete(m_dialog);
}

bool AdvancedBindingEditor::Open(LivingRoomTool* a_parent, InputBinding& a_binding)
{
	if (m_dialog != nullptr)
	{
		LRT_Fail();
		return true;
	}

	const GamepadDevice* editedDevice = a_parent->GetCurrentlySelectedDevice();

	m_dialog = new AdvancedBindingEditorDialog(a_parent, editedDevice,
		&a_binding.Get_sources(), &a_binding.Get_destinations());
	m_dialog->setObjectName(QString::fromUtf8("Edit advanced binding"));

	QDialog::DialogCode returnCode = static_cast<QDialog::DialogCode>(m_dialog->exec());
	if (returnCode == QDialog::DialogCode::Accepted)
	{
		a_binding.SetData(m_dialog->ObtainResultSources(), m_dialog->ObtainResultDestinations());
		return true;
	}
	else
	{
		return false;
	}
}