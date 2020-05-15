#include "QBindingExchangeWidget.h"

#include "Common.h"

QBindingExchangeWidget::QBindingExchangeWidget(QWidget* a_parent)
	: QWidget(a_parent)
{
	m_ui.setupUi(this);
}

QBindingExchangeWidget::~QBindingExchangeWidget()
{
	Cleanup();
}

void QBindingExchangeWidget::InitContents(const Contents& a_initialContents)
{
	Cleanup();

	m_contents = a_initialContents;
	LRT_Assert(m_contents.m_Title != nullptr);

	m_ui.Label_Title->setText(QString::fromWCharArray(m_contents.m_Title));

	// First add all possible indices to source list.
	for (const Element& element : m_contents.m_Elements)
	{
		QListWidgetItem* item = new QListWidgetItemBinding(element.m_bIsSelected ? m_ui.List_Selected : m_ui.List_Source);
		item->setText(QString::fromStdString(element.m_Label));
		item->setData(Qt::UserRole, QVariant(element.m_Value));

		if (element.m_bIsSelected)
		{
			m_selectedValues.push_back(element.m_Value);
		}
	}

	connect(m_ui.Btn_Add, &QPushButton::clicked, this, &QBindingExchangeWidget::OnMoveToSelectedButtonClicked);
	connect(m_ui.Btn_Remove, &QPushButton::clicked, this, &QBindingExchangeWidget::OnMoveToSourceButtonClicked);
	connect(m_ui.Btn_EditByClick, &QPushButton::clicked, this, &QBindingExchangeWidget::OnEditClicked);
}

inline void QBindingExchangeWidget::Cleanup()
{
	m_contents.m_Title = nullptr;
	m_contents.m_Elements.clear();

	m_selectedValues.clear();

	m_ui.List_Source->clear();
	m_ui.List_Selected->clear();
}

void QBindingExchangeWidget::OnMoveToSelectedButtonClicked()
{
	OnMoveTo(m_ui.List_Source, m_ui.List_Selected, &QBindingExchangeWidget::OnMoveToFuncAddValue);
}

void QBindingExchangeWidget::OnMoveToSourceButtonClicked()
{
	OnMoveTo(m_ui.List_Selected, m_ui.List_Source, &QBindingExchangeWidget::OnMoveToFuncRemValue);
}

void QBindingExchangeWidget::OnMoveTo(QListWidget* a_listSource, QListWidget* a_listDestination, OnMoveHelperFunc helperFunc)
{
	QModelIndexList indices = a_listSource->selectionModel()->selectedIndexes();

	if (indices.empty())
	{
		return;
	}

	for (QModelIndex idx : indices)
	{
		const int index = idx.row();

		QListWidgetItem* item = a_listSource->takeItem(index);
		LRT_Assert(item != nullptr);

		a_listDestination->addItem(item);

#ifdef NDEBUG
		const ValueType value = item->data(Qt::UserRole).toULongLong();
#else
		bool bSuccess = false;
		const ValueType value = item->data(Qt::UserRole).toULongLong(&bSuccess);
		LRT_Assert(bSuccess);
#endif
		
		LRT_Assert(helperFunc != nullptr);
		helperFunc(m_selectedValues, value);
	}

	// Sort destination as it may have got its items out of order. Source doesn't need to be sorted as the order didn't change.
	a_listDestination->sortItems();

	// Clear selection on both source and destination (for safety).
	a_listSource->clearSelection();
	a_listDestination->clearSelection();

	// Fire signal.
	selectionChanged();
}

void QBindingExchangeWidget::OnEditClicked()
{
	// Just fire the signal, it will be supported externally.
	editClicked();
}

void QBindingExchangeWidget::OnMoveToFuncAddValue(SelectedValues& a_selectedValues, ValueType a_value)
{
	a_selectedValues.push_back(a_value);
}

void QBindingExchangeWidget::OnMoveToFuncRemValue(SelectedValues& a_selectedValues, ValueType a_value)
{
	QBindingExchangeWidget::SelectedValues::iterator foundVal = std::find(a_selectedValues.begin(), a_selectedValues.end(), a_value);
	LRT_Assert(foundVal != a_selectedValues.end());
	a_selectedValues.erase(foundVal);
}
