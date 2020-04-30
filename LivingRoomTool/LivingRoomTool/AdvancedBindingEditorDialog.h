#pragma once

#include "ui_AdvancedBindingEditorDialog.h"

#include "InputAction.h"
#include "GamepadState.h"

class AdvancedBindingEditorDialog : public QDialog
{
	Q_OBJECT

private:

	template<typename EnumType, size_t EnumStartValue, size_t EnumMaxValue> class ExchangeWrapper
	{
	public:

		using LabelsArray = std::array<std::string_view, EnumMaxValue>;
		using ValuesArray = std::array<EnumType, EnumMaxValue>;
		using Selected = std::vector<EnumType>;

		ExchangeWrapper()
			: m_widget(nullptr)
		{
		}

		~ExchangeWrapper()
		{
			m_widget = nullptr;
		}

		void Init(QBindingExchangeWidget* a_widget,
			const wchar_t* a_title,
			const LabelsArray& a_labels, 
			const ValuesArray& a_values,
			const Selected* a_initialSelected = nullptr)
		{
			m_widget = a_widget;

			QBindingExchangeWidget::Contents contents{};
			contents.m_Title = a_title;

			for (size_t i = EnumStartValue; i < EnumMaxValue; ++i)
			{
				bool bIsSelected = false;

				if (a_initialSelected != nullptr)
				{
					bIsSelected = std::find(a_initialSelected->begin(), a_initialSelected->end(), a_values[i]) != a_initialSelected->end();
				}

				contents.m_Elements.push_back({ 
					std::string(a_labels[i].data(), a_labels[i].size()), 
					static_cast<QBindingExchangeWidget::ValueType>(a_values[i]), 
					bIsSelected 
					});
			}

			// Initialize widget.
			m_widget->InitContents(contents);
		}

		Selected ObtainSelected() const
		{
			const std::vector<QBindingExchangeWidget::ValueType>& rawValues = m_widget->GetSelectedValues();

			Selected selected;
			for (const QBindingExchangeWidget::ValueType rawValue : rawValues)
			{
				selected.push_back(static_cast<EnumType>(rawValue));
			}

			return std::move(selected);
		}

		bool IsNotEmpty() const
		{
			const std::vector<QBindingExchangeWidget::ValueType>& rawValues = m_widget->GetSelectedValues();
			return rawValues.size() > 0;
		}

	private:

		QBindingExchangeWidget* m_widget;
	};

public:

	AdvancedBindingEditorDialog(QWidget* parent, const std::vector<GamepadButtons>* selectedSources = nullptr, const std::vector<InputActionKey>* selectedDestinations = nullptr);
	~AdvancedBindingEditorDialog();

	std::vector<GamepadButtons> ObtainResultSources() const;
	std::vector<InputActionKey> ObtainResultDestinations() const;

	bool IsNotEmpty() const;

private:

	void OnSelectedListContentChanged();

	void EnableAccept();
	void DisableAccept();

	Ui_AdvancedBindingEditorDialog m_ui;

	// Starting from 1, to skip kNone present in both enums.
	ExchangeWrapper<GamepadButtons, 1, GamepadButtonsConvert::k_enumSize> m_sourceExchange;
	ExchangeWrapper<InputActionKey, 1, magic_enum::enum_count<InputActionKey>()> m_destinationExchange;
};

