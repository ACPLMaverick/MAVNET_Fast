#pragma once

#include "ui_BindingExchangeWidget.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>

class QBindingExchangeWidget : public QWidget
{
	Q_OBJECT

public:

	using ValueType = uint64_t;
	using SelectedValues = std::vector<ValueType>;

	struct Element
	{
		std::string m_Label;
		ValueType m_Value;
		bool m_bIsSelected;
	};

	struct Contents
	{
		const wchar_t* m_Title{ nullptr };
		std::vector<Element> m_Elements;
	};

	QBindingExchangeWidget(QWidget* parent);
	~QBindingExchangeWidget();

	void InitContents(const Contents& initialContents);
	const SelectedValues& GetSelectedValues() const { return m_selectedValues; }

signals:

	void selectionChanged();
	void editClicked();

private:

	class QListWidgetItemBinding : public QListWidgetItem
	{
	public:
		explicit QListWidgetItemBinding(QListWidget* a_listview)
			: QListWidgetItem(a_listview, Type)
		{
		}

		bool operator<(const QListWidgetItem& a_other) const override final
		{
			return data(Qt::UserRole) < a_other.data(Qt::UserRole);
		}
	};

	using OnMoveHelperFunc = void(*)(SelectedValues&, ValueType);

	inline void Cleanup();

	void OnMoveToSelectedButtonClicked();
	void OnMoveToSourceButtonClicked();
	void OnMoveTo(class QListWidget* listSource, class QListWidget* listDestination, OnMoveHelperFunc helperFunc);

	void OnEditClicked();

	static void OnMoveToFuncAddValue(SelectedValues& a_selectedValues, ValueType a_value);
	static void OnMoveToFuncRemValue(SelectedValues& a_selectedValues, ValueType a_value);

	Ui_BindingExchangeWidget m_ui;
	Contents m_contents;
	SelectedValues m_selectedValues;
};

