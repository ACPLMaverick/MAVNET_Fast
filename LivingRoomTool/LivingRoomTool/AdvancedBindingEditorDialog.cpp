#include "AdvancedBindingEditorDialog.h"

AdvancedBindingEditorDialog::AdvancedBindingEditorDialog(QWidget* a_parent, const std::vector<GamepadButtons>* a_selectedSources /*= nullptr*/, const std::vector<InputActionKey>* a_selectedDestinations /*= nullptr*/)
	: QDialog(a_parent)
{
	m_ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	// Connect to selection changed signal.
	connect(m_ui.Exch_GamepadButtons, &QBindingExchangeWidget::selectionChanged, this, &AdvancedBindingEditorDialog::OnSelectedListContentChanged);
	connect(m_ui.Exch_InputActionKeys, &QBindingExchangeWidget::selectionChanged, this, &AdvancedBindingEditorDialog::OnSelectedListContentChanged);

	// Create exchange widgets.
	m_sourceExchange.Init(m_ui.Exch_GamepadButtons, L"Sources", GamepadButtonsHelper::GetLabels(), GamepadButtonsHelper::GetValues(), a_selectedSources);
	m_destinationExchange.Init(m_ui.Exch_InputActionKeys, L"Destinations", magic_enum::enum_names<InputActionKey>(), magic_enum::enum_values<InputActionKey>(), a_selectedDestinations);

	// Process button availability on init.
	OnSelectedListContentChanged();
}

AdvancedBindingEditorDialog::~AdvancedBindingEditorDialog()
{
}

std::vector<GamepadButtons> AdvancedBindingEditorDialog::ObtainResultSources() const
{
	return std::move(m_sourceExchange.ObtainSelected());
}

std::vector<InputActionKey> AdvancedBindingEditorDialog::ObtainResultDestinations() const
{
	return std::move(m_destinationExchange.ObtainSelected());
}

bool AdvancedBindingEditorDialog::IsNotEmpty() const
{
	return m_sourceExchange.IsNotEmpty() && m_destinationExchange.IsNotEmpty();
}

void AdvancedBindingEditorDialog::OnSelectedListContentChanged()
{
	// Process OK Button availability.
	if (m_ui.Exch_GamepadButtons->GetSelectedValues().size() > 0
		&& m_ui.Exch_InputActionKeys->GetSelectedValues().size() > 0)
	{
		EnableAccept();
	}
	else
	{
		DisableAccept();
	}
}

void AdvancedBindingEditorDialog::EnableAccept()
{
	if (m_ui.Btn_Ok->isEnabled() == false)
	{
		m_ui.Btn_Ok->setEnabled(true);
	}
}

void AdvancedBindingEditorDialog::DisableAccept()
{
	if (m_ui.Btn_Ok->isEnabled())
	{
		m_ui.Btn_Ok->setEnabled(false);
	}
}
