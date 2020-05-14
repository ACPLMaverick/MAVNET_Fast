#include "BindingSelectorWidget.h"

#include "InputAction.h"

BindingSelectorWidget::BindingSelectorWidget(QWidget* a_parent)
	: QWidget(a_parent)
{
	m_ui.setupUi(this);

	connect(m_ui.Btn_Edit, &QPushButton::clicked, this, &BindingSelectorWidget::OnEditClicked);
	void (QComboBox::* indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
	connect(m_ui.Cb_Select, indexChangedSignal, this, &BindingSelectorWidget::OnComboBoxIndexChanged);

	connect(&m_keypressRetrieverDialog, &SimpleKeypressRetrieverDialog::resultsAvailable, this, &BindingSelectorWidget::OnResultsAvailable);
}

BindingSelectorWidget::~BindingSelectorWidget()
{
}

void BindingSelectorWidget::Fill()
{
	m_ui.Cb_Select->clear();

	static const size_t k_enumSize = magic_enum::enum_count<InputActionKey>();
	for (size_t i = 0; i < k_enumSize; ++i)
	{
		const std::string_view& str = magic_enum::enum_name(static_cast<InputActionKey>(i));
		LRT_Assert(str.size() > 0);
		if (str.size() == 0)
		{
			continue;
		}

		m_ui.Cb_Select->addItem(QString::fromUtf8(str.data(), str.size()));
	}

	m_ui.Cb_Select->setCurrentIndex(0);
}

void BindingSelectorWidget::Clear()
{
	m_ui.Cb_Select->clear();
}

int BindingSelectorWidget::GetIndex() const
{
	return m_ui.Cb_Select->currentIndex();
}

void BindingSelectorWidget::SetIndex(int a_newIndex)
{
	m_ui.Cb_Select->setCurrentIndex(a_newIndex);
}

void BindingSelectorWidget::OnEditClicked()
{
	m_keypressRetrieverDialog.OpenAsync();
}

void BindingSelectorWidget::OnResultsAvailable()
{
	const InputActionKey newKey = m_keypressRetrieverDialog.GetResult();
	if (newKey != InputActionKey::kNone)
	{
		SetIndex(static_cast<int>(newKey));
	}
}

void BindingSelectorWidget::OnComboBoxIndexChanged(int a_newIndex)
{
	indexChanged(a_newIndex);
}
