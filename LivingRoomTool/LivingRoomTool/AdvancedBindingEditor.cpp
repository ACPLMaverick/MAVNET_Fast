#include "AdvancedBindingEditor.h"

#include "InputBinding.h"

AdvancedBindingEditor::AdvancedBindingEditor()
{
}

AdvancedBindingEditor::~AdvancedBindingEditor()
{
	LRT_SafeDelete(m_dialog);
}

bool AdvancedBindingEditor::Open(QWidget* a_parent, InputBinding& a_binding)
{
	if (m_dialog != nullptr)
	{
		LRT_Fail();
		return true;
	}

	m_dialog = new AdvancedBindingEditorDialog(a_parent, &a_binding.Get_sources(), &a_binding.Get_destinations());
	m_dialog->setObjectName(QString::fromUtf8("Edit advanced binding"));

	QDialog::DialogCode returnCode = static_cast<QDialog::DialogCode>(m_dialog->exec());
	if (returnCode == QDialog::DialogCode::Accepted)
	{
		std::vector<GamepadButtons>& sources = a_binding.Get_sources();
		std::vector<InputActionKey>& destinations = a_binding.Get_destinations();

		sources = m_dialog->ObtainResultSources();
		destinations = m_dialog->ObtainResultDestinations();

		// Make bindings sorted.
		std::sort(sources.begin(), sources.end());
		std::sort(destinations.begin(), destinations.end());

		return true;
	}
	else
	{
		return false;
	}
}