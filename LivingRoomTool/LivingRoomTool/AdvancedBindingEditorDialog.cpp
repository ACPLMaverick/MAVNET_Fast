#include "AdvancedBindingEditorDialog.h"

AdvancedBindingEditorDialog::AdvancedBindingEditorDialog(QWidget* a_parent, const GamepadDevice* a_editedDevice, const std::vector<GamepadButtons>* a_selectedSources /*= nullptr*/, const std::vector<InputActionKey>* a_selectedDestinations /*= nullptr*/)
	: QDialog(a_parent)
	, m_sourceRetriever(a_parent, a_editedDevice)
	, m_destinationRetriever(a_parent)
{
	m_ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	// Connect to selection changed signal.
	InitSources(a_selectedSources);
	InitDestinations(a_selectedDestinations);

	connect(m_ui.Exch_GamepadButtons, &QBindingExchangeWidget::selectionChanged, this, &AdvancedBindingEditorDialog::OnSelectedListContentChanged);
	connect(m_ui.Exch_InputActionKeys, &QBindingExchangeWidget::selectionChanged, this, &AdvancedBindingEditorDialog::OnSelectedListContentChanged);

	connect(m_ui.Exch_GamepadButtons, &QBindingExchangeWidget::editClicked, this, &AdvancedBindingEditorDialog::OnSourcesEditClicked);
	connect(m_ui.Exch_InputActionKeys, &QBindingExchangeWidget::editClicked, this, &AdvancedBindingEditorDialog::OnDestinationsEditClicked);

	connect(&m_sourceRetriever, &SimpleGamepadPressRetrieverDialog::resultsAvailable, this, &AdvancedBindingEditorDialog::OnSourcesEditAccepted);
	connect(&m_destinationRetriever, &SimpleKeypressRetrieverDialog::resultsAvailable, this, &AdvancedBindingEditorDialog::OnDestinationsEditAccepted);

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

void AdvancedBindingEditorDialog::InitSources(const std::vector<GamepadButtons>* a_selectedSources)
{
	m_sourceExchange.Init(m_ui.Exch_GamepadButtons, L"Sources", GamepadButtonsHelper::GetLabels(), GamepadButtonsHelper::GetValues(), a_selectedSources);
}

void AdvancedBindingEditorDialog::InitDestinations(const std::vector<InputActionKey>* a_selectedDestinations)
{
	m_destinationExchange.Init(m_ui.Exch_InputActionKeys, L"Destinations", magic_enum::enum_names<InputActionKey>(), magic_enum::enum_values<InputActionKey>(), a_selectedDestinations);
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

void AdvancedBindingEditorDialog::OnSourcesEditClicked()
{
	m_sourceRetriever.OpenAsync();
}

void AdvancedBindingEditorDialog::OnDestinationsEditClicked()
{
	m_destinationRetriever.OpenAsync(true);
}

void AdvancedBindingEditorDialog::OnSourcesEditAccepted()
{
	const GamepadButtons result = m_sourceRetriever.GetResult();

	if (result == GamepadButtons::kNone)
	{
		return;
	}

	const auto& values = GamepadButtonsHelper::GetValues();
	std::vector<GamepadButtons> sourceButtons;
	for (GamepadButtons button : values)
	{
		if ((button & result) != GamepadButtons::kNone)
		{
			sourceButtons.push_back(button);
		}
	}

	InitSources(&sourceButtons);
}

void AdvancedBindingEditorDialog::OnDestinationsEditAccepted()
{
	std::vector<InputActionKey> newSelection;
	m_destinationRetriever.GetResults(newSelection);
	if (newSelection.size() > 0)
	{
		InitDestinations(&newSelection);
	}
}
